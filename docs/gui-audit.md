# GUI audit

Audited on 2026-10-12 against the header-only implementation in `lib/gui/`, the
application demo in `src/game/_gui.h`, and the Linux scripts in `run/`.

## Existing design and capabilities

Broky's GUI remains an immediate-mode, procedural C API. `GUI_State` owns durable
window and interaction state, `GUI_Temp` owns frame-local layout and drawing state,
and `GUI_Setup` owns fonts, icons, cursors, and theme data. Widgets are emitted
directly from application callbacks; there is no retained widget tree.

The existing API provides windows with z-order, moving, resizing and vertical
scrolling; grid rows and columns; buttons, icon buttons, labels, numeric/text
inputs, checks, images, menu overlays; themes, fonts and custom cursors. Windows
clip controls to their workspace. Overlays are redrawn above regular content.

The main application already served as a partial demo. A dedicated gallery now
lives in `examples/gui-gallery.c` and covers all public controls, empty and long
content, wrapping, UTF-8, repeated widgets, resizing, overlap, scrolling, a modal
overlay, and keyboard/gamepad focus.

## Confirmed defects fixed

- Text cursor/blink state was shared by every input. Each stable owner now gets an
  independent fixed-capacity state slot.
- Cursor movement, Backspace and Delete operated on bytes and could split UTF-8.
  They now move and edit at codepoint boundaries; typed Raylib codepoints are
  encoded as UTF-8.
- Text input accepted ASCII only. It now accepts Unicode codepoints and supports
  selection plus Ctrl+A/C/X/V.
- Buttons and checks activated on mouse press. They now capture one control and
  activate on release inside it, with cancellation when released elsewhere.
- Overlays allowed the outside click that dismissed them to reach controls below.
  The dismissing press is consumed; Escape/gamepad B cancels an overlay.
- Grid spans wider/taller than two cells had the wrong size. Spans now use their
  actual first and last cell bounds.
- Available grid bounds could become negative. They are clamped.
- Window wheel input could affect a covered/non-target window. It is limited to
  the target workspace.
- Scrollbar thumb position used incompatible ratios. It now maps the clamped
  offset to the actual thumb travel.
- Float editors shared two static buffers, causing collisions across widgets.
  Their buffers are now per-owner.
- GUI textures, fonts, cursors and the render target had no aggregate teardown.
  `GUI_UnloadSetup` and `GUI_UnloadState` make ownership explicit.

## Added behavior

- Stable 64-bit widget IDs combine window identity with owner or label/bounds.
- Deterministic spatial navigation uses the previous frame's control list.
- D-pad, left stick, Tab/arrows, A/Enter/Space and B/Escape are supported.
- `GUI_WantsKeyboard` lets the host avoid gameplay/UI key conflicts.
- `GUI_SetTextInputHook` lets a platform request an on-screen keyboard when a text
  control gains focus without embedding Steam-specific behavior.
- `GUI_TextWrapped` adds clipped multiline word wrapping without allocation.
- Pure C99 tests cover IDs, UTF-8 editing, selection, scroll clamping and spatial
  focus transitions.

## Preserved intentionally

- Existing widget signatures and immediate-mode callback flow.
- Fixed-capacity state and no per-frame heap allocation.
- The grid, window, overlay, theme and font organization.
- Existing visual language and public enum values.
- Raylib as the only GUI/platform dependency.

## Known limitations and potential improvements

These are missing features, not known regressions:

- Text inputs remain single-line. Wrapped display is multiline, but a full
  multiline editor with vertical selection and scrolling is not included.
- Selection is editable and clipboard-aware but is not yet painted as a highlight.
- Nested independent scroll regions are not a public primitive; window scrolling
  is robust, but arbitrary nested panes need a future begin/end region API.
- Scrollbar dragging is not implemented; wheel, focus navigation and clamping are.
- Navigation uses a fixed 256-item frame list and text state uses 256 owner slots.
- Atlas-backed fonts intentionally contain ASCII only and substitute `?` for
  other codepoints. The default Raylib font path is used for the gallery's UTF-8.
- Docking is not present in the original implementation and was not introduced.
- Runtime behavior still needs manual verification on SteamOS hardware and with
  multiple gamepad models.

## Performance and reliability notes

Widget identity, navigation, text state and wrapping use fixed storage. Normal
editing is linear in the affected suffix, as expected for bounded C strings.
Text measurement while placing the cursor is performed only on a focus click;
there is no cache with ambiguous invalidation. No busy waiting was added.

Automated verification is documented in `docs/gui-usage.md`. Graphical builds
compile in this environment; pixel-level appearance and physical gamepad behavior
remain manual checks.
