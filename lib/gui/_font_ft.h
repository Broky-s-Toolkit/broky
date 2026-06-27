#pragma once
#ifndef IDE_SYNTAX_HL
 #include "gui_base.h"
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

GUI_FontAtlas GUI_LoadFontAtlasASCII(const char *path, int pixel_size);
void          GUI_DrawFontAtlasText(const GUI_FontAtlas *atlas, const char *text, Vector2 position, Color tint, float scale, float spacing);
Vector2       GUI_MeasureFontAtlasText(const GUI_FontAtlas *atlas, const char *text, float scale, float spacing);

#ifdef IMPLEMENT_ALL

GUI_FontAtlas GUI_LoadFontAtlasASCII(const char *path, int pixel_size)
{
    GUI_FontAtlas atlas = { 0 };
    FT_Library library = NULL;
    FT_Face face = NULL;

    if (FT_Init_FreeType(&library) != 0) {
        return atlas;
    }

    if (FT_New_Face(library, path, 0, &face) != 0) {
        FT_Done_FreeType(library);
        return atlas;
    }

    if (FT_Set_Pixel_Sizes(face, 0, (FT_UInt)pixel_size) != 0) {
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return atlas;
    }

    atlas.pixel_size     = pixel_size;
    atlas.ascent         = (int)(face->size->metrics.ascender >> 6);
    atlas.descent        = (int)(-(face->size->metrics.descender >> 6));
    atlas.line_height    = (int)(face->size->metrics.height >> 6);
    if (atlas.line_height <= 0) {
        atlas.line_height = pixel_size;
    }

    const int first_codepoint = 32;
    const int last_codepoint = 126;
    const int padding = 1;
    const int atlas_max_width = 1024;
    int pen_x = padding;
    int pen_y = padding;
    int row_h = 0;
    int atlas_w = padding;

    for (int codepoint = first_codepoint; codepoint <= last_codepoint; ++codepoint) {
        if (FT_Load_Char(face, (FT_ULong)codepoint, FT_LOAD_RENDER) != 0) {
            continue;
        }

        FT_GlyphSlot glyph = face->glyph;
        int glyph_w = (int)glyph->bitmap.width;
        int glyph_h = (int)glyph->bitmap.rows;

        if (pen_x + glyph_w + padding > atlas_max_width) {
            pen_x = padding;
            pen_y += row_h + padding;
            row_h = 0;
        }

        atlas.glyphs[codepoint] = (GUI_FontGlyph) {
            .src        = (Rectangle){ (float)pen_x, (float)pen_y, (float)glyph_w, (float)glyph_h },
            .advance    = (int)(glyph->advance.x >> 6),
            .bearing_x  = glyph->bitmap_left,
            .bearing_y  = glyph->bitmap_top,
            .width      = glyph_w,
            .height     = glyph_h
        };

        pen_x += glyph_w + padding;
        if (glyph_h > row_h) {
            row_h = glyph_h;
        }
        if (pen_x > atlas_w) {
            atlas_w = pen_x;
        }
    }

    atlas.atlas_width  = atlas_w + padding;
    atlas.atlas_height = pen_y + row_h + padding;

    Image image = GenImageColor(atlas.atlas_width, atlas.atlas_height, BLANK);
    Color *pixels = (Color *)image.data;

    for (int codepoint = first_codepoint; codepoint <= last_codepoint; ++codepoint) {
        GUI_FontGlyph glyph_info = atlas.glyphs[codepoint];
        if (glyph_info.advance == 0 && codepoint != ' ') {
            continue;
        }
        if (glyph_info.width == 0 || glyph_info.height == 0) {
            continue;
        }

        if (FT_Load_Char(face, (FT_ULong)codepoint, FT_LOAD_RENDER) != 0) {
            continue;
        }

        FT_GlyphSlot glyph = face->glyph;
        for (int y = 0; y < glyph_info.height; ++y) {
            for (int x = 0; x < glyph_info.width; ++x) {
                unsigned char alpha = glyph->bitmap.buffer[y * glyph->bitmap.pitch + x];
                int dst_x = (int)glyph_info.src.x + x;
                int dst_y = (int)glyph_info.src.y + y;
                pixels[dst_y * atlas.atlas_width + dst_x] = (Color){ 255, 255, 255, alpha };
            }
        }
    }

    atlas.texture = LoadTextureFromImage(image);
    UnloadImage(image);

    if (atlas.texture.id != 0) {
        SetTextureFilter(atlas.texture, TEXTURE_FILTER_POINT);
        atlas.ready = true;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return atlas;
}

void GUI_DrawFontAtlasText(const GUI_FontAtlas *atlas, const char *text, Vector2 position, Color tint, float scale, float spacing)
{
    if (atlas == NULL || !atlas->ready || text == NULL) {
        return;
    }

    float pen_x = position.x;
    float pen_y = position.y;

    for (const unsigned char *ptr = (const unsigned char *)text; *ptr != '\0'; ++ptr) {
        unsigned char codepoint = *ptr;
        if (codepoint == '\n') {
            pen_x = position.x;
            pen_y += (float)atlas->line_height * scale;
            continue;
        }

        if (codepoint >= GUI_FONT_ATLAS_GLYPHS) {
            codepoint = '?';
        }

        GUI_FontGlyph glyph = atlas->glyphs[codepoint];
        if (glyph.advance == 0 && codepoint != ' ') {
            glyph = atlas->glyphs[(unsigned char)'?'];
        }

        if (glyph.width > 0 && glyph.height > 0) {
            Rectangle src = glyph.src;
            Rectangle dst = {
                SnapFloat(pen_x + (float)glyph.bearing_x * scale),
                SnapFloat(pen_y + (float)(atlas->ascent - glyph.bearing_y) * scale),
                (float)glyph.width * scale,
                (float)glyph.height * scale
            };
            DrawTexturePro(atlas->texture, src, dst, (Vector2){ 0 }, 0.0f, tint);
        }

        pen_x += (float)glyph.advance * scale + spacing;
    }
}

Vector2 GUI_MeasureFontAtlasText(const GUI_FontAtlas *atlas, const char *text, float scale, float spacing)
{
    if (atlas == NULL || !atlas->ready || text == NULL) {
        return (Vector2){ 0 };
    }

    float current_x = 0.0f;
    float max_x = 0.0f;
    int lines = 1;

    for (const unsigned char *ptr = (const unsigned char *)text; *ptr != '\0'; ++ptr) {
        unsigned char codepoint = *ptr;
        if (codepoint == '\n') {
            if (current_x > max_x) {
                max_x = current_x;
            }
            current_x = 0.0f;
            lines++;
            continue;
        }

        if (codepoint >= GUI_FONT_ATLAS_GLYPHS) {
            codepoint = '?';
        }

        GUI_FontGlyph glyph = atlas->glyphs[codepoint];
        if (glyph.advance == 0 && codepoint != ' ') {
            glyph = atlas->glyphs[(unsigned char)'?'];
        }

        current_x += (float)glyph.advance * scale + spacing;
    }

    if (current_x > max_x) {
        max_x = current_x;
    }

    return (Vector2){
        max_x,
        (float)atlas->line_height * scale * (float)lines
    };
}

#endif
