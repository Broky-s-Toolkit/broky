#pragma once
#include "main.h"

// > SUBMODULE: CONTROLS
// > INDEX
// > BASE MACROS
//   - GUI_BASE_CONTROL_ACTIVATED
//   - GUI_BASE_CONTROL_FOCUSED

// > DRAW PRIMITIVES
void        GUI_DrawShadow(Rectangle shape);
void        GUI_DrawBorders(Rectangle shape, Color dark, Color light, float border, bool remove_corner);
Vector2     GUI_MeasureText(const char* text, EGUI_Font font, float scale);
void        GUI_DrawAdjustedTextEx(const char* text, Vector2 position, Color tint, float scale, EGUI_Font font);
Vector2     GUI_MeasureAdjustedText(const char* text, EGUI_Font font);
// > CURSOR
void        GUI_DrawCursorFor(EGUI_Cursor cursor);
void        GUI_DrawCursor(void);
void        GUI_DrawCursorTrail(void);
// > ICONS
float       GUI_DrawIcon(Rectangle shape, Texture2D* texture2d, Color tint);
float       GUI_Icon(Texture2D* texture2d, GUI_Pin pin, Color tint);
bool        GUI_IconButton(Texture2D* texture2d, GUI_Pin pin, Color tint);
// > IMAGES
void        GUI_Face(GUI_Pin pin);
void        GUI_Image(Texture2D texture, Rectangle shape);
// > BUTTON
void GUI_DrawButton(GUI_Box box, const char *text, Texture2D *icon, EGUI_ControlStatus status);
bool GUI_Button(GUI_Box box, const char* text, Texture2D* icon);
bool GUI_ButtonMenu(GUI_Box box, const void* owner_id, const char* text, Texture2D* icon, void (*draw_function)(void));
bool GUI_ButtonMenuContents(int x, int x_end, int start_row, EGUI_ThemeColor colors, GUI_MenuItems *items, Rectangle *final_shape);
// > TEXT
void GUI_DrawText(GUI_Box box, const char* text);
void GUI_Text(GUI_Box box, const char* text);
// > INPUTS
void GUI_DrawInput(GUI_Box box, char* buffer, int blink_cursor, EGUI_ControlStatus status, bool blink);
void GUI_Input(GUI_Box box, void *owner, char *buffer, int buffer_size, EGUI_InputType type);
void GUI_Float(GUI_Box box, float *value, float min, float max);
// > CHECK
void GUI_DrawCheck(GUI_Box box, bool value, const char *on_txt, const char *off_txt, EGUI_ControlStatus status);
void GUI_Check(GUI_Box box, bool *value, const char *on_txt, const char *off_txt);


