#pragma once
#ifndef IDE_SYNTAX_HL
 #define IDE_SYNTAX_HL 1
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
    Rectangle   src;
    float       advance;
    float       bearing_x;
    float       bearing_y;
    float       bearing_x2;
    float       bearing_y2;
} GUI_FontGlyph;

#define GUI_FONT_ATLAS_GLYPHS 128

typedef struct {
    bool            ready;
    Texture2D       texture;
    int             pixel_size;
    int             atlas_width;
    int             atlas_height;
    int             line_height;
    int             ascent;
    int             descent;
    GUI_FontGlyph   glyphs[GUI_FONT_ATLAS_GLYPHS];
} GUI_FontAtlas;

typedef struct {
    float           default_height;
    float           border;

    float           scale;
    Vector2         delta;             // Delta adjustment
    Font            custom;
    int             texture_filter;
    GUI_FontAtlas   atlas;
    int             atlas_reload_size;
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
    int value;
    const char *label;
    Texture2D *icon;
} GUI_MenuItem;

typedef struct {
    // TODO@dc: review int?
    int *selected_value;
    int count;
    const GUI_MenuItem *elements;
} GUI_MenuItems;

typedef struct {
    const void      *id_ptr;            // Control Owner. A unique pointer representing the control owner
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


// > FUNCTIONS
//   INDEX

// > CONTEXT
GUI_State       GUI_MakeStateDefault(Vector2 screen_max);
GUI_Temp        GUI_MakeTempDefault(void);
void            GUI_SetContext(GUI_State* state, GUI_Setup* setup, GUI_Temp* temp);
GUI_State*      GUI_GetState(void);
GUI_Setup*      GUI_GetSetup(void);
// > WINDOW RUNTIME EVENTS
void            GUI_ProcessWindow(GUI_Window* window, Rectangle limits);
void            GUI_AfterWindowContents(GUI_Window* window);
// > CURSOR
bool            GUI_IsCursorOverGui(void);
bool            GUI_IsCurrentWindowTarget(int window_id);
bool            GUI_IsCursorOverOverlay(void);
// > FRAME PIPELINE
void            GUI_BeginDraw(EGUI_Cursor cursor_style);
void            GUI_ResetStyleDefaults(void);
void            GUI_EndDraw(void);
// > CONTROL HELPERS
bool            GUI_CheckCollisionCursorControl(Rectangle shape, GUI_Window *window);
bool            GUI_CheckCollisionCursorControlWin(Rectangle shape);
Rectangle       GUI_ControlShapeCut(Rectangle shape, float border, float scale, bool intersect_window);
void            GUI_BeginControlScissor(void);
void            GUI_BeginInnerControlScissor(Rectangle shape, float border, float scale);

// > FORWARD DECLARATIONS
bool            GUI_OverlayIsDrawing(void);
GUI_Window*     GUI_GetWindow(int id);
// < FROM _SUBMODULES

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
