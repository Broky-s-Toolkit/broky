#pragma once
#ifndef NON_EDITOR_BUILD
#define NON_EDITOR_BUILD 0
#define IMPLEMENT_ALL
#include "global.h"
#endif


#define GUI_MAX_TRAIL       30
#define GUI_MIN_WIN_RECT    (Rectangle){ 0, 0, 320.f, 240.f }
#define GUI_MAX_OPEN_WINS   16
#define GUI_MAX_TEXTBOXES   256
#define GUI_SCROLL_SPEED    16
#define GUI_NO_WIN          (-1)


// > ENUMS
//
typedef enum {
    EGUI_Status_Off,
    EGUI_Status_Ready,
    EGUI_Status_Drawing
} EGUI_Status;

typedef enum {
    EGUI_ControlStatus_Default,
    EGUI_ControlStatus_Collide,
    EGUI_ControlStatus_Focused
} EGUI_ControlStatus;

typedef enum {
    EGUI_Input_Text,      // editable ASCII text
    EGUI_Input_Int,       // integer numeric input
    EGUI_Input_Float      // floating numeric input
} EGUI_InputType;

/*
EXAMPLE: Enum order matters
typedef enum {
    EGUI_Focus_Available,
    EGUI_Focus_CanOverride,
    EGUI_Focus_Granted
} EGUI_Focus;

// NOTE: define a function near the type instead of using it everywhere.
//       Now, we know that the order matters for this Enum.
bool FocusOverridable(EGUI_Focus focus)
{
    return focus <= EGUI_Focus_CanOverride;
}
*/

//
// BEGIN SUBMODULES >>>>>>>
//

// > TYPES
//   SUBMODULE: SETUP
typedef struct {
    Texture2D None;
    Texture2D New;
    Texture2D Open;
    Texture2D Save;
    Texture2D Setup;
    Texture2D Error;
    Texture2D Face;
    Texture2D Dog;
    Texture2D Close;
    Texture2D CloseSmall;
    Texture2D MinimizeSmall;
    Texture2D Layouts;
} GUI_Icons;

typedef struct {
    float       icon_size;
    float       icon_size_sm;
    Vector2     icon_delta;
    GUI_Icons   icons;
} GUI_IconSetup;

typedef struct {
    Color tx_color_0;
    Color tx_color_1;
    Color bg_color_0;
    Color bg_color_1;
    Color bg_color_2;
    Color bg_color_3;
} GUI_ThemeColors;

typedef enum {
    EGUI_ThemeColor_Gray,
    EGUI_ThemeColor_Red,
    EGUI_ThemeColor_Green,
    EGUI_ThemeColor_Abstractica,
    EGUI_ThemeColor_Count
} EGUI_ThemeColor;

typedef struct {
    GUI_ThemeColors     colors[EGUI_ThemeColor_Count];
    float               bg_alpha;
    float               color_change;
} GUI_Theme;

typedef enum {
    EGUI_Font_Default,
    EGUI_Font_GUI,
    EGUI_Font_ShareTech,
    EGUI_Font_Count
} EGUI_Font;

typedef struct {
    float           default_height;
    float           border;

    float           scale;
    Vector2         delta;             // Delta adjustment
    Font            custom;
    bool            use_custom;        // Indicates if a custom font is used
    float           spacing;
    Vector2         blink_size;        // Size of the blinking cursor
    Vector2         blink_delta;      // Blink adjustment
    float           blink_alpha;
} GUI_FontSetup;

typedef enum {
    EGUI_Cursor_None,
    EGUI_Cursor_Default,
    EGUI_Cursor_AGS,
    EGUI_Cursor_Text,
    EGUI_Cursor_Resize,
    EGUI_Cursor_Count,
} EGUI_Cursor;

typedef struct {
    Texture2D       texture;
    Vector2         delta_normalized;
    float           scale;
    float           alpha;
    Vector2         trail_delta_normalized;
    EGUI_Cursor     additional_cursor;
} GUI_CursorSetup;

typedef struct {
    GUI_Theme           theme;
    GUI_IconSetup       icons;
    GUI_FontSetup       fonts[EGUI_Font_Count];
    GUI_CursorSetup     cursors[EGUI_Cursor_Count];
} GUI_Setup;
// < END SUBMODULE: SETUP

// > TYPES
//   SUBMODULE: WINDOW
#define MAX_WINDOW_TITLE 16

typedef enum {
    EGUI_WindowAction_None,
    EGUI_WindowAction_Moving,
    EGUI_WindowAction_Resizing
} EGUI_WindowAction;

typedef struct GUI_Window {
    int             id;
    Rectangle       shape;
    EGUI_ThemeColor colors;
    const char      *title;
    Texture2D       *icon;
    float           scroll_offset;
    bool            focused_face;
    Rectangle       workspace;
    // Calc fields
    float           content_height; // Automatically stored by GUI_EndWindowContents. Calculated during grid processing.
    // Handlers
    void (*contents) (struct GUI_Window*);
} GUI_Window;

// < END SUBMODULE: WINDOW

// > TYPES
//   SUBMODULE: GRID
#define GUI_RESET_COUNT     0
#define GUI_ADD_COUNT       1
#define GUI_ONLY_GET_COUNT  2
#define GUI_NO_SIZE         0.0

typedef struct {
    Rectangle       current_workspace;  // Current available workspace (consumed by GUI_GridNext)
    int             vertical_count;
    float           vertical_size;
    int             horizontal_count;
    float           horizontal_size;
    float           used_height;        // Used to auto calc vertical scroll bar
    float           current_scroll;

    // Window that is being processed right now
    // This is NOT the active window focused by the player.
    bool            force_overflow;
} GUI_GridTemp;
// < END SUBMODULE: GRID

//
// <<<<<<< END SUBMODULES
//


// > TYPES
//


typedef struct {
    const char      *id_ptr;            // Control Owner. A unique pointer representing the control owner
    GUI_GridTemp    grid;               // Grid state when the overlay draw was queued
    EGUI_ThemeColor colors;             // Colors when the overlay draw was queued
    bool            just_enabled;       // TRUE if the overlay was JUST ENABLED on this frame
    Rectangle       final_shape;        // Final shape of the overlay
    bool            is_drawing;         // True if *function is being executed
    void            (*function)(void);  // Draw function
}  GUI_Overlay;

typedef struct {
    // Globals
    EGUI_Status      status;
    void             *control_focus_ptr;            // Keeps track between frames if there is a focused control
    EGUI_Font        current_font;
    EGUI_ThemeColor  current_theme_colors;

    // Submodules
    GUI_GridTemp    grid;
    GUI_Overlay     overlay;
    // Cursor
    EGUI_Cursor     cursor;
    Vector2         cursor_last;
    Vector2         cursor_current;
    bool            cursor_over_gui;                // True if the pointer is over any of the elements in the GUI
    Vector2         cursor_trail[GUI_MAX_TRAIL];
    // Window
    int                 window_current_id;          // ID of the window that is being processed
    int                 window_target_id;           // Targeted window. SEE: UPDATE WINDOW TARGET ID inside _window.h
    EGUI_WindowAction   window_current_action;
} GUI_Temp;

typedef struct {
    RenderTexture2D buffer;
    float           scale;
    int             force_z_index;

    GUI_Window      window_s[GUI_MAX_OPEN_WINS];

    // z_index stores windows indexes or zero as empty.
    int             z_index[GUI_MAX_OPEN_WINS];
} GUI_State;


// > STATE CONTEXT
static struct {
    GUI_State*  state;
    GUI_Setup*  setup;
    GUI_Temp*   temp;
} GUI_CTX = { 0 };


#include "_setup.h"

// > FUNCTIONS
//   INDEX

// > CONTEXT
GUI_State       GUI_MakeStateDefault(Vector2 screen_max);
GUI_Temp        GUI_MakeTempDefault();
void            GUI_SetContext(GUI_State* state, GUI_Setup* setup, GUI_Temp* temp);
GUI_State*      GUI_GetState();
GUI_Setup*      GUI_GetSetup();
// > WINDOW RUNTIME EVENTS
void            GUI_ProcessWindow(GUI_Window* window, Rectangle limits);
void            GUI_AfterWindowContents(GUI_Window* window);
// > CURSOR
bool            GUI_IsCursorOverGui();
bool            GUI_IsCurrentWindowTarget(int window_id);
bool            GUI_IsCursorOverOverlay();
// > FRAME PIPELINE
void            GUI_BeginDraw(EGUI_Cursor cursor_style);
void            GUI_ResetStyleDefaults();
void            GUI_EndDraw();

// > SUBMODULES
#include "_grid.h"
#include "_window.h"
#include "_overlay.h"
#include "_controls.h"

// > FUNCTIONS
//   IMPLEMENTATION

#ifdef IMPLEMENT_ALL

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

GUI_Temp GUI_MakeTempDefault()
{
    GUI_Temp temp = {
        .status                     = EGUI_Status_Off,
        .control_focus_ptr          = NULL,
        .current_font               = EGUI_Font_Default,
        .current_theme_colors       = EGUI_ThemeColor_Gray,
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
GUI_State* GUI_GetState()
{
    return GUI_CTX.state;
}

// To be used out-side this module
GUI_Setup* GUI_GetSetup()
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
bool GUI_IsCursorOverGui()
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

bool GUI_IsCursorOverOverlay()
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

void GUI_ResetStyleDefaults()
{
    GUI_GridReset(GetScreenRect());
    GUI_SetFont(EGUI_Font_Default);
    GUI_SetThemeColors(EGUI_ThemeColor_Gray);
}

void GUI_EndDraw()
{
    GUI_CTX.temp->cursor_last = GUI_CTX.temp->cursor_current;
    GUI_DrawOverlay();

    Assert(GUI_CTX.temp->status == EGUI_Status_Drawing);
    GUI_CTX.temp->status = EGUI_Status_Ready;
}
// < FRAME PIPELINE

#endif
