#pragma once
#include "main.h"

GUI_FontAtlas GUI_LoadFontAtlasASCII(const char *path, int pixel_size);
void          GUI_UnloadFontAtlas(GUI_FontAtlas *atlas);
void          GUI_DrawFontAtlasText(const GUI_FontAtlas *atlas, const char *text, Vector2 position, Color tint, float scale, float spacing);
Vector2       GUI_MeasureFontAtlasText(const GUI_FontAtlas *atlas, const char *text, float scale, float spacing);

