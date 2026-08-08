#include "main.h"
#include "_font.h"
#include "_setup.h"
#include "_grid.h"
#include "_overlay.h"
#include "_controls.h"
#include "_window.h"

GUI_Context GUI_CTX = { 0 };


// > FUNCTIONS
//   IMPLEMENTATION

const GUI_MenuItem *GUI_MenuItemGetSelected(GUI_MenuItems *items)
{
    Assert(items != NULL);
    Assert(items->selected_value != NULL);
    Assert(items->count > 0);
    Assert(items->elements != NULL);

    for (int i = 0; i < items->count; i++) {
        if (items->elements[i].value == *items->selected_value) {
            return &items->elements[i];
        }
    }

    return &items->elements[0];
}
// > CONTROL HELPERS
bool GUI_CheckCollisionCursorControl(Rectangle shape, GUI_Window *window)
{
    GUI_State *state    = GUI_CTX.state;
    Vector2 cursor      = GUI_CTX.temp->cursor_current;

    // Overlay
    if (GUI_OverlayIsDrawing()) {
        return CheckCollisionPointRec(cursor, shape);
    } else if (GUI_IsCursorOverOverlay()) {
        return false;
    }

    // Outside a window
    int focused_window_id   = state->z_index[0];
    bool outside_window     = window == NULL || focused_window_id == GUI_NO_WIN;
    if (outside_window) {
        return CheckCollisionPointRec(cursor, shape);
    }

    // Inside a window
    Assert(window != NULL);
    bool current_target = GUI_IsCurrentWindowTarget(window->id);
    if (current_target == false) {
        return false;
    }
    // Window
    bool collide_window     = CheckCollisionPointRec(cursor, window->shape);
    bool collide_control    = CheckCollisionPointRec(cursor, shape);
    bool collide_workspace  = CheckCollisionPointRec(cursor, window->workspace);
    bool overflow           = GUI_CTX.temp->grid.force_overflow;
    bool collide                = collide_control && (collide_workspace || overflow);
    bool result                 = collide && (focused_window_id == window->id || !collide_window);
    return result;
}

bool GUI_CheckCollisionCursorControlWin(Rectangle shape)
{
    GUI_Window *window  = GUI_GetWindow(GUI_CTX.temp->window_current_id);
    bool collide        = GUI_CheckCollisionCursorControl(shape, window);
    return collide;
}

Rectangle GUI_ControlShapeCut(Rectangle shape, float border, float scale, bool intersect_window) {
    Rectangle result = AddRect(shape, border * scale, border * scale, -border * scale * 2, -border * scale * 2);
    //TODO@dc: result.y += GUI_CTX.temp->grid.current_scroll;

    int window_id = GUI_CTX.temp->window_current_id;
    if (intersect_window && window_id != GUI_NO_WIN) {
        GUI_Window *window          = GUI_GetWindow(window_id);
        Rectangle intersection      = RectIntersection(result, window->workspace);
#if DEV_DEBUG_GUI_SCROLL == 1
        if (GUI_CTX.temp->window_current_id == GUI_CTX.state->z_index[0]) {
            GUI_DrawBorders(window->workspace, RED, RED, 1, false);
            DrawDebugRect(result, ColorAlpha(GREEN, 0.1f));
            DrawDebugRect(intersection, ColorAlpha(ORANGE, 0.9f));
        }
#endif
        result = intersection;
    }
    return result;
}

void GUI_BeginControlScissor(void)
{
    bool not_overflow   = GUI_CTX.temp->grid.force_overflow == false;
    int window_id       = GUI_CTX.temp->window_current_id;
    bool inside_window  = window_id != GUI_NO_WIN;
    if (not_overflow && inside_window) {
        GUI_Window *window = GUI_GetWindow(window_id);
        if (window == NULL) {
            return;
        }
        BeginScissorModeRect(window->workspace);
    }
}

// Cut text not only by window but by the control itself
// Useful to cut text inside a control
void GUI_BeginInnerControlScissor(Rectangle shape, float border, float scale)
{
    bool not_overflow   = GUI_CTX.temp->grid.force_overflow == false;
    bool inside_window  = GUI_CTX.temp->window_current_id != GUI_NO_WIN;
    BeginScissorModeRect(GUI_ControlShapeCut(shape, border, scale, inside_window && not_overflow));
}
// < END CONTROL HELPERS

// > CONTEXT

GUI_State GUI_MakeStateDefault(Vector2 screen_max)
{
    GUI_State state = {
        .buffer         = LoadRenderTexture((int)screen_max.x, (int)screen_max.y),
        .scale          = 1.0f,
        .force_z_index  = 0
    };

    // IMPORTANT: keep GUI buffer pixel-perfect
    SetTextureFilter(state.buffer.texture, TEXTURE_FILTER_POINT);

    for (int i = 0; i < GUI_MAX_OPEN_WINS; i++) {
        state.window_s[i] = GUI_MakeEmptyWindow();
    }
    memset(state.z_index, 0, sizeof(state.z_index));
    return state;
}

GUI_Temp GUI_MakeTempDefault(void)
{
    GUI_Temp temp = {
        .status                     = EGUI_Status_Off,
        .control_focus_ptr          = NULL,
        .opt                        = {
            .font                   = EGUI_Font_Default,
            .colors                 = EGUI_ThemeColor_Gray,
        },
        .grid                       = GUI_MakeGrid(),
        .overlay                    = GUI_MakeOverlay(),
        .cursor                     = EGUI_Cursor_Default,
        .cursor_last                = (Vector2){ 0.0f, 0.0f },
        .cursor_current             = (Vector2){ 0.0f, 0.0f },
        .cursor_over_gui            = false,
        .cursor_trail               = {{0}},
        .window_current_id          = GUI_NO_WIN,
        .window_target_id           = GUI_NO_WIN,
        .window_current_action      = EGUI_WindowAction_None,
    };
    return temp;
}

void GUI_SetContext(GUI_State* state, GUI_Setup* setup, GUI_Temp* temp)
{
    // Update statuses
    // Outgoing temp
    if (GUI_CTX.temp != NULL) {
        GUI_CTX.temp->status = EGUI_Status_Off;
    }
    // Incoming
    temp->status = EGUI_Status_Ready;

    GUI_CTX.setup = setup;
    GUI_CTX.state = state;
    GUI_CTX.temp  = temp;
}

// To be used out-side this module
GUI_State* GUI_GetState(void)
{
    return GUI_CTX.state;
}

// To be used out-side this module
GUI_Setup* GUI_GetSetup(void)
{
    return GUI_CTX.setup;
}

// > WINDOW RUNTIME EVENTS
void GUI_ProcessWindow(GUI_Window* window, Rectangle limits)
{
    Assert(window->id > 0);
    Assert(window->contents != NULL);
    // Grant min dimensions
    Rectangle min_size      = GUI_MIN_WIN_RECT;
    window->shape.width     = FloatMax(min_size.width, window->shape.width);
    window->shape.height    = FloatMax(min_size.height, window->shape.height);

    GUI_UpdateAndDrawWindow(window, limits);

    GUI_GridReset(window->workspace);
    GUI_CTX.temp->window_current_id = window->id;
    GUI_SetFont(EGUI_Font_GUI);
    GUI_SetThemeColors(window->colors);

    // Vertical scroll
    GUI_CTX.temp->grid.current_scroll = -window->scroll_offset;
}

void GUI_AfterWindowContents(GUI_Window* window)
{
    // End window stuff
    GUI_GridAutoJump();
    // Vertical scroll
    // Stored grid height
    window->content_height = GUI_CTX.temp->grid.used_height;

    //rlPopMatrix();
    GUI_CTX.temp->window_current_id = GUI_NO_WIN;
    GUI_DrawOverlay();
}
// < WINDOW RUNTIME EVENTS

// > CURSOR
// GUI_IsCursorOverGui() is meant to be called after EndDraw
bool GUI_IsCursorOverGui(void)
{
    Assert(GUI_CTX.temp->status == EGUI_Status_Ready);
    return GUI_CTX.temp->cursor_over_gui;
}

// Returns true if window id is the interactable target (cursor is over and z-index is the lowest possible)
bool GUI_IsCurrentWindowTarget(int window_id)
{
    int target_id   = GUI_CTX.temp->window_target_id;
    bool no_target  = target_id == GUI_NO_WIN;
    bool is_target  = target_id == window_id;
    return no_target || is_target;
}

bool GUI_IsCursorOverOverlay(void)
{
    Vector2 cursor      = GUI_CTX.temp->cursor_current;
    Rectangle shape     = GUI_CTX.temp->overlay.final_shape;
    bool is_cursor_over = CheckCollisionPointRec(cursor, shape);
    return is_cursor_over;
}
// < CURSOR

// > FRAME PIPELINE
void GUI_BeginDraw(EGUI_Cursor cursor_style)
{
    Assert(GUI_CTX.temp->status == EGUI_Status_Ready);
    GUI_CTX.temp->status = EGUI_Status_Drawing;

    GUI_CTX.temp->cursor            = cursor_style;
    GUI_CTX.temp->cursor_over_gui   = false;
    GUI_CTX.temp->cursor_current    = LimitVector2Rect(GetMousePosition(), GetScreenRect());

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        GUI_CTX.temp->control_focus_ptr = NULL;
    }
}

void GUI_ResetStyleDefaults(void)
{
    GUI_GridReset(GetScreenRect());
    GUI_SetFont(EGUI_Font_Default);
    GUI_SetThemeColors(EGUI_ThemeColor_Gray);
}

void GUI_EndDraw(void)
{
    GUI_CTX.temp->cursor_last = GUI_CTX.temp->cursor_current;
    GUI_DrawOverlay();

    Assert(GUI_CTX.temp->status == EGUI_Status_Drawing);
    GUI_CTX.temp->status = EGUI_Status_Ready;
}
// < FRAME PIPELINE
