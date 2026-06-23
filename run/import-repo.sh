#!/usr/bin/env bash
set -euo pipefail

SOURCE_REPO_URL="https://github.com/TheScienceOfCodeEDU/raylib-examples-c.git"
SOURCE_BRANCH="mini-game"
SOURCE_PATH="src"
TARGET_BRANCH="main"

REMOTE_NAME="raylib_examples_src"

WORKDIR="$(mktemp -d)"
SOURCE_DIR="$WORKDIR/raylib-examples-c-filtered"

run_git_filter_repo() {
    if git filter-repo --version >/dev/null 2>&1; then
        git filter-repo "$@"
    elif command -v git-filter-repo >/dev/null 2>&1; then
        git-filter-repo "$@"
    elif python3 -m git_filter_repo --version >/dev/null 2>&1; then
        python3 -m git_filter_repo "$@"
    else
        echo "ERROR: git-filter-repo is not available."
        echo
        echo "Try one of these:"
        echo "  sudo dnf install git-filter-repo"
        echo "  pipx install git-filter-repo"
        echo "  pip install git-filter-repo"
        exit 1
    fi
}

cleanup() {
    git remote remove "$REMOTE_NAME" >/dev/null 2>&1 || true
}

trap cleanup EXIT

echo "==> Checking target repo..."

if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    echo "ERROR: Run this script from inside your target Git repo."
    exit 1
fi

CURRENT_BRANCH="$(git branch --show-current)"

if [ "$CURRENT_BRANCH" != "$TARGET_BRANCH" ]; then
    echo "ERROR: Target repo must be on '$TARGET_BRANCH'. Current branch: '$CURRENT_BRANCH'."
    exit 1
fi

if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "ERROR: You have uncommitted changes."
    echo
    echo "Save them first with:"
    echo "  git stash push -u -m \"WIP before importing mini-game src history\""
    echo
    echo "or commit them."
    exit 1
fi

echo "==> Checking git-filter-repo..."
run_git_filter_repo --version >/dev/null

echo "==> Cloning source repo branch '$SOURCE_BRANCH'..."
git clone \
    --branch "$SOURCE_BRANCH" \
    --single-branch \
    "$SOURCE_REPO_URL" \
    "$SOURCE_DIR"

cd "$SOURCE_DIR"

echo "==> Keeping only '$SOURCE_PATH/' while preserving path..."
run_git_filter_repo --path "$SOURCE_PATH/"

echo "==> Verifying filtered repo contains only '$SOURCE_PATH/'..."
UNEXPECTED_FILES="$(git ls-tree -r --name-only HEAD | grep -v "^$SOURCE_PATH/" || true)"

if [ -n "$UNEXPECTED_FILES" ]; then
    echo "ERROR: Filtered repo still has files outside '$SOURCE_PATH/':"
    echo "$UNEXPECTED_FILES"
    exit 1
fi

echo "==> Filtered files:"
git ls-tree -r --name-only HEAD

cd - >/dev/null

echo "==> Adding filtered repo as temporary remote..."
git remote remove "$REMOTE_NAME" >/dev/null 2>&1 || true
git remote add "$REMOTE_NAME" "$SOURCE_DIR"
git fetch "$REMOTE_NAME" "$SOURCE_BRANCH"

echo "==> Previewing files that will be imported..."
git ls-tree -r --name-only "$REMOTE_NAME/$SOURCE_BRANCH"

echo
echo "==> Merging filtered src history into '$TARGET_BRANCH'..."
git merge "$REMOTE_NAME/$SOURCE_BRANCH" \
    --allow-unrelated-histories \
    --no-ff \
    -m "Import mini-game src history"

echo "==> Cleaning temporary remote..."
git remote remove "$REMOTE_NAME"
trap - EXIT

echo "==> Done."
echo
echo "Verify history with:"
echo "  git log -- src"
echo "  git log --follow -- src/main.c"
echo
echo "Verify no source-root extras were imported with:"
echo "  git show --name-only --stat HEAD"
echo
echo "Temporary clone was:"
echo "  $SOURCE_DIR"