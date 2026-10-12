#pragma once

#ifndef IDE_SYNTAX_HL
 #define IDE_SYNTAX_HL 1
 #define IMPLEMENT_ALL
 #include "global.h"
#endif

#include "__core.h"

// > SUBMODULES
#include "_font.h"
#include "_setup.h"
#include "_grid.h"
#include "_overlay.h"
#include "_controls.h"
#include "_window.h"



// > FUNCTIONS
//   IMPLEMENTATION

#ifdef IMPLEMENT_ALL
// > CONTEXT

GUI_State GUI_MakeStateDefault(Vector2 screen_max)
{
    GUI_State state = {
        .buffer         = LoadRenderTexture((int)screen_max.x, (int)screen_max.y),
        .scale          = 1.0f,
        .force_z_index  = 0,
        .gamepad_navigation = true
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

void GUI_UnloadState(GUI_State *state)
{
    if (state == NULL) return;
    if (state->buffer.id != 0) UnloadRenderTexture(state->buffer);
    memset(state, 0, sizeof(*state));
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

bool GUI_WantsKeyboard(void)
{
    return GUI_CTX.temp->control_focus_ptr != NULL
        || GUI_CTX.state->navigation_focus != 0
        || GUI_CTX.temp->overlay.id_ptr != NULL;
}
// < CURSOR

// > FRAME PIPELINE
void GUI_BeginDraw(EGUI_Cursor cursor_style)
{
    Assert(GUI_CTX.temp->status == EGUI_Status_Ready);
    GUI_CTX.temp->status = EGUI_Status_Drawing;

    GUI_CTX.temp->cursor            = cursor_style;
    GUI_CTX.temp->cursor_over_gui   = false;
    GUI_CTX.temp->pointer_press_consumed = false;
    GUI_CTX.temp->cursor_current    = LimitVector2Rect(GetMousePosition(), GetScreenRect());
    GUI_CTX.temp->nav_count         = 0;

    float nav_x = 0.0f;
    float nav_y = 0.0f;
    if (IsKeyPressed(KEY_TAB)) nav_x = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT) ? -1.0f : 1.0f;
    if (GUI_CTX.state->navigation_focus != 0) {
        if (IsKeyPressed(KEY_LEFT))  nav_x = -1.0f;
        if (IsKeyPressed(KEY_RIGHT)) nav_x = 1.0f;
        if (IsKeyPressed(KEY_UP))    nav_y = -1.0f;
        if (IsKeyPressed(KEY_DOWN))  nav_y = 1.0f;
    }
    if (GUI_CTX.state->gamepad_navigation && IsGamepadAvailable(0)) {
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))  nav_x = -1.0f;
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) nav_x = 1.0f;
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))    nav_y = -1.0f;
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))  nav_y = 1.0f;

        float axis_x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        float axis_y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        if (fabsf(axis_x) > 0.65f && !GUI_CTX.state->nav_axis_x_held) nav_x = axis_x < 0 ? -1.0f : 1.0f;
        if (fabsf(axis_y) > 0.65f && !GUI_CTX.state->nav_axis_y_held) nav_y = axis_y < 0 ? -1.0f : 1.0f;
        GUI_CTX.state->nav_axis_x_held = fabsf(axis_x) > 0.35f;
        GUI_CTX.state->nav_axis_y_held = fabsf(axis_y) > 0.35f;
    }
    if (nav_x != 0.0f || nav_y != 0.0f) {
        GUI_CTX.state->navigation_focus = GUI_NavFind(
            GUI_CTX.state->nav_items, GUI_CTX.state->nav_count,
            GUI_CTX.state->navigation_focus, nav_x, nav_y);
        GUI_CTX.temp->control_focus_ptr = NULL;
    }
    if (IsKeyPressed(KEY_ESCAPE)
        || (GUI_CTX.state->gamepad_navigation && IsGamepadAvailable(0)
            && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))) {
        if (GUI_CTX.temp->overlay.id_ptr != NULL) GUI_OverlayClose();
        else GUI_ClearFocus();
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        GUI_Overlay *overlay = &GUI_CTX.temp->overlay;
        if (overlay->id_ptr != NULL
            && !CheckCollisionPointRec(GUI_CTX.temp->cursor_current, overlay->final_shape)
            && !CheckCollisionPointRec(GUI_CTX.temp->cursor_current, overlay->anchor_shape)) {
            GUI_OverlayClose();
            GUI_CTX.temp->pointer_press_consumed = true;
        }
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
    GUI_CTX.state->nav_count = GUI_CTX.temp->nav_count;
    memcpy(GUI_CTX.state->nav_items, GUI_CTX.temp->nav_items,
        (size_t)GUI_CTX.temp->nav_count * sizeof(GUI_CTX.temp->nav_items[0]));
}

void GUI_SetGamepadNavigationEnabled(bool enabled)
{
    GUI_CTX.state->gamepad_navigation = enabled;
}

void GUI_SetTextInputHook(void (*hook)(void *owner, Rectangle shape))
{
    GUI_CTX.state->text_input_hook = hook;
}

void GUI_ClearFocus(void)
{
    GUI_CTX.temp->control_focus_ptr = NULL;
    GUI_CTX.state->navigation_focus = 0;
    GUI_CTX.state->active_control = 0;
}
// < FRAME PIPELINE
#endif
