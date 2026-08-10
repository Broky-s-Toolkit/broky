#pragma once

#include "main.h"


// > SUBMODULE: WINDOW
// > INDEX
Rectangle       GUI_MakeWorkspace(void);
GUI_Window      GUI_MakeEmptyWindow(void);

// > WINDOW CONTROLS
void GUI_WindowButtonPanel(GUI_Window* window, EGUI_Font font);
void GUI_WindowEndingPanel(GUI_Window* window, EGUI_Font font);
void GUI_DrawWindow(GUI_Window* window,  EGUI_ControlStatus status, EGUI_Font font);
void GUI_UpdateAndDrawWindow(GUI_Window *window, Rectangle limits);

// > WINDOW RUNTIME
void            GUI_CleanAndPrepareZIndex(void);
void            GUI_UpdateAndDrawWindows(Rectangle limits);
// > WINDOW STATE
GUI_Window*     GUI_OpenWindow(
                    int id, const char *title, EGUI_ThemeColor colors,
                    Texture2D *icon, bool focused_face, void (*contents)(GUI_Window*));
void            GUI_RemoveWindow(int id);

GUI_Window*     GUI_GetWindow(int id);
GUI_Window*     GUI_GetWindowByZindex(int z);
void            GUI_ForceZindex(int win_id);

Rectangle       GUI_GetWindowTitle(Rectangle shape);
Rectangle       GUI_GetWindowPanel(Rectangle shape);
Rectangle       GUI_GetWindowBottom(Rectangle shape);
void            GUI_WindowUpdateShapeForContent(GUI_Window *window);

