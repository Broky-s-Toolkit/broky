#define IDE_SYNTAX_HL 0
#define IMPLEMENT_ALL 1
#define STB_TRUETYPE_IMPLEMENTATION
#include "3rd/stb_truetype.h"
#include "common.h"
#include "gui/gui.h"

static char gallery_text[256] = "UTF-8: Español, 日本語, Кириллица, 🙂";
static bool gallery_check = true;
static float gallery_number = 1.25f;
static bool gallery_dialog = false;
static int gallery_dialog_owner;

static void GalleryDialog(void)
{
    bool confirmed = false;
    float width = 420.0f;
    float height = 180.0f;
    Rectangle dialog = {
        ((float)GetScreenWidth() - width) * 0.5f,
        ((float)GetScreenHeight() - height) * 0.5f,
        width, height
    };
    GUI_BeginOverlay(true, false);
    DrawRectangleRec(GetScreenRect(), ColorAlpha(BLACK, 0.55f));
    GUI_DrawShadow(dialog);
    GUI_GridReset(dialog);
    GUI_GridForXY(width, 48.0f, 0.0f);
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextY(), EGUI_ThemeColor_Abstractica), "Modal dialog");
    GUI_TextWrapped(GUI_MakeBoxColor(GUI_GridNextY(), EGUI_ThemeColor_Abstractica),
        "Input behind this dialog is blocked. Press B or Escape to cancel.");
    if (GUI_Button(GUI_MakeBoxColor(GUI_GridNextY(), EGUI_ThemeColor_Green), "Confirm (A / Enter)", NULL)) {
        gallery_dialog = false;
        confirmed = true;
    }
    GUI_EndOverlay(dialog);
    if (confirmed) GUI_OverlayClose();
}

static void GalleryControls(GUI_Window *window)
{
    GUI_Icons *icons = GUI_GetIcons();
    float row = GUI_CalcDefaultHeightScaled(EGUI_Font_Default);
    GUI_SetFont(EGUI_Font_Default);
    GUI_GridForCols(3, row);

    GUI_Text(GUI_MakeBox(GUI_GridNextX()), "Text input");
    GUI_Input(GUI_MakeBox(GUI_GridNextXn(2)), gallery_text, gallery_text,
        (int)sizeof(gallery_text), EGUI_Input_Text);
    GUI_GridRepeat();
    GUI_Text(GUI_MakeBox(GUI_GridNextX()), "Number");
    GUI_Float(GUI_MakeBox(GUI_GridNextXn(2)), &gallery_number, -100.0f, 100.0f);
    GUI_GridRepeat();
    GUI_Text(GUI_MakeBox(GUI_GridNextX()), "Checkbox");
    GUI_Check(GUI_MakeBoxColor(GUI_GridNextXn(2), EGUI_ThemeColor_Red),
        &gallery_check, "Enabled", "Disabled");
    GUI_GridRepeat();
    if (GUI_Button(GUI_MakeBoxColor(GUI_GridNextX(), EGUI_ThemeColor_Green), "New", &icons->New)) { }
    if (GUI_Button(GUI_MakeBoxColor(GUI_GridNextX(), EGUI_ThemeColor_Abstractica), "Open", &icons->Open)) { }
    if (GUI_Button(GUI_MakeBoxColor(GUI_GridNextX(), EGUI_ThemeColor_Gray), "Modal", &icons->Error)) {
        gallery_dialog = true;
        GUI_OverlayOpenFor(&gallery_dialog_owner);
    }

    GUI_GridForXY(window->workspace.width, row * 3.0f, 0.0f);
    GUI_TextWrapped(GUI_MakeBoxColor(GUI_GridNextY(), EGUI_ThemeColor_Abstractica),
        "A long paragraph demonstrates wrapping and clipping. Resize this window at runtime: controls retain stable bounds, long content remains inside the workspace, and the window scroll position is clamped.");

    GUI_GridForXY(window->workspace.width, row, 0.0f);
    for (int i = 0; i < 48; ++i) {
        GUI_Button(GUI_MakeBoxColor(GUI_GridNextY(), (EGUI_ThemeColor)(i % EGUI_ThemeColor_Count)),
            TextFormat("Repeated row %02d -- D-pad / stick navigation", i + 1), NULL);
    }

    if (gallery_dialog && GUI_OverlayOpenedBy(&gallery_dialog_owner)) {
        GUI_OverlaySetDrawCall(false, EGUI_ThemeColor_Abstractica, GalleryDialog);
    }
}

static void GalleryEmpty(GUI_Window *window)
{
    GUI_SetFont(EGUI_Font_Default);
    GUI_GridForXY(window->workspace.width, 48.0f, 0.0f);
    GUI_Text(GUI_MakeBox(GUI_GridNextY()), "Empty state: no items yet.");
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 800, "Broky GUI Gallery");
    SetWindowMinSize(960, 540);
    SetTargetFPS(60);

    Vector2 maximum = { (float)GetMonitorWidth(GetCurrentMonitor()), (float)GetMonitorHeight(GetCurrentMonitor()) };
    GUI_State state = GUI_MakeStateDefault(maximum);
    GUI_Setup setup = GUI_LoadSetupDefault();
    GUI_Temp temp = GUI_MakeTempDefault();
    GUI_SetContext(&state, &setup, &temp);

    GUI_Window *controls = GUI_OpenWindow(1, "Widget gallery", EGUI_ThemeColor_Gray,
        &setup.icons.icons.Layouts, true, GalleryControls);
    GUI_Window *empty = GUI_OpenWindow(2, "Overlapping window", EGUI_ThemeColor_Abstractica,
        &setup.icons.icons.Dog, false, GalleryEmpty);
    controls->shape = (Rectangle){ 32, 32, 760, 700 };
    empty->shape = (Rectangle){ 680, 120, 480, 300 };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){ 21, 23, 28, 255 });
        GUI_BeginDraw(EGUI_Cursor_Default);
        GUI_UpdateAndDrawWindows((Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() });
        GUI_EndDraw();
        GUI_DrawCursor();
        EndDrawing();
    }

    GUI_UnloadSetup(&setup);
    GUI_UnloadState(&state);
    CloseWindow();
    return 0;
}
