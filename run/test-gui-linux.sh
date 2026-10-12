#!/bin/sh
set -eu

ROOT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cd "$ROOT_DIR"

mkdir -p bin/test-linux
cc -std=c99 -Wall -Wextra -Werror -pedantic -Ilib tests/gui_logic_test.c -o bin/test-linux/gui_logic_test
bin/test-linux/gui_logic_test
