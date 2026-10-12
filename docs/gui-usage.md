# GUI usage

## Frame lifecycle

```c
GUI_State state = GUI_MakeStateDefault(screen_max);
GUI_Setup setup = GUI_LoadSetupDefault();
GUI_Temp temp = GUI_MakeTempDefault();
GUI_SetContext(&state, &setup, &temp);

while (!WindowShouldClose()) {
    BeginDrawing();
    GUI_BeginDraw(EGUI_Cursor_Default);
    GUI_UpdateAndDrawWindows((Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight() });
    GUI_EndDraw();
    GUI_DrawCursor();
    EndDrawing();
}

GUI_UnloadSetup(&setup);
GUI_UnloadState(&state);
```

Owners passed to editable controls must have a stable address. This isolates
cursor, selection and scratch state:

```c
static char name[128] = "Broky";
GUI_Input(GUI_MakeBox(shape), name, name, sizeof(name), EGUI_Input_Text);
```

## Wrapped text

```c
GUI_TextWrapped(GUI_MakeBoxColor(paragraph_bounds, EGUI_ThemeColor_Gray), paragraph);
```

The supplied rectangle is both the wrapping width and clipping boundary.

## Gamepad and host input

Gamepad navigation is enabled by default. D-pad/left stick move focus, A confirms,
and B cancels. Keyboard equivalents are Tab/arrows, Enter/Space, and Escape.

Gate application controls while the GUI owns keyboard navigation:

```c
if (!GUI_WantsKeyboard()) {
    UpdateGameKeyboardInput();
}
```

Platforms can attach an on-screen-keyboard request without platform code in the
GUI library:

```c
static void RequestKeyboard(void *owner, Rectangle bounds) {
    (void)owner;
    PlatformShowKeyboard(bounds);
}

GUI_SetTextInputHook(RequestKeyboard);
```

## Build, tests and gallery

```sh
sh run/build-linux.sh
sh run/test-gui-linux.sh
sh run/build-gui-gallery-linux.sh
sh run/run-gui-gallery-linux.sh
```

The gallery starts at 1280x800, is resizable, and can be tested at 1920x1080.
Use mouse, Tab/arrows, or a standard gamepad to traverse controls. Open the modal
to verify input capture, resize overlapping windows, and scroll the long list.
