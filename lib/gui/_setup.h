#pragma once
#include "main.h"


// > SUBMODULE: SETUP
// > INDEX

// > CURSORS
GUI_CursorSetup     GUI_LoadCursorSetupForType(EGUI_Cursor cursor);
GUI_CursorSetup*    GUI_GetCursorSetup(void);
// > FONTS
GUI_FontSetup       GUI_LoadFontSetupDefault(EGUI_Font font);
void                GUI_ReloadFontSetupAsset(EGUI_Font font);
GUI_FontSetup*      GUI_GetFontSetup(EGUI_Font font);
GUI_MenuItems       GUI_GetFontMenuItems(int *selected_value);
GUI_MenuItems       GUI_GetTextureFilterMenuItems(int *selected_value);
const char*         GUI_GetTextureFilterLabel(int texture_filter);
void                GUI_ApplyFontTextureFilter(EGUI_Font font);
Font                GUI_GetFontAsset(EGUI_Font font);
EGUI_Font           GUI_GetFont(void);
void                GUI_SetFont(EGUI_Font font);
float               GUI_CalcDefaultHeightScaled(EGUI_Font font);
// > THEME
GUI_ThemeColors     GUI_LoadThemeColorsDefault(EGUI_ThemeColor color);
GUI_ThemeColors     GUI_MakeThemeColors(Color tx_color_0, Color tx_color_1, Color bg_color_0, Color bg_color_1, Color bg_color_2, Color bg_color_3);
Color               GUI_GenerateThemeColor(float hue, float intensity);
GUI_ThemeColors     GUI_GenerateThemeColors(float hue);
GUI_Theme           GUI_GenerateTheme(void);
GUI_Theme           GUI_GetTheme(void);
GUI_ThemeColors     GUI_GetThemeColors(EGUI_ThemeColor color);
EGUI_ThemeColor     GUI_GetThemeColor(void);
void                GUI_SetThemeColors(EGUI_ThemeColor color);
// > ICONS
GUI_Icons           GUI_LoadIcons(void);
GUI_IconSetup       GUI_MakeIconSetup(GUI_Icons icons);
GUI_Icons*          GUI_GetIcons(void);
Texture2D*          GUI_GetIconTexture(EGUI_Icon icon);
float               GUI_GetIconWidth(void);
float               GUI_GetIconWidthForShape(Rectangle shape, float border);
float               GUI_GetIconSmallWidth(void);
// > COMMON
GUI_Setup           GUI_LoadSetupDefault(void);

// > IMPLEMENTATION
