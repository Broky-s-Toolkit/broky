#pragma once
#ifndef IDE_SYNTAX_HL
 #define STB_TRUETYPE_IMPLEMENTATION
 #include <stb_truetype.h>
 #include "gui_base.h"
#endif

GUI_FontAtlas GUI_LoadFontAtlasASCII(const char *path, int pixel_size);
void          GUI_DrawFontAtlasText(const GUI_FontAtlas *atlas, const char *text, Vector2 position, Color tint, float scale, float spacing);
Vector2       GUI_MeasureFontAtlasText(const GUI_FontAtlas *atlas, const char *text, float scale, float spacing);

#ifdef IMPLEMENT_ALL

GUI_FontAtlas GUI_LoadFontAtlasASCII(const char *path, int pixel_size)
{
    GUI_FontAtlas atlas = { 0 };
    int font_bytes = 0;
    unsigned char *font_data = LoadFileData(path, &font_bytes);
    if (font_data == NULL || font_bytes <= 0) {
        return atlas;
    }

    stbtt_fontinfo font = { 0 };
    if (!stbtt_InitFont(&font, font_data, 0)) {
        UnloadFileData(font_data);
        return atlas;
    }

    float scale = stbtt_ScaleForPixelHeight(&font, (float)pixel_size);
    int ascent = 0;
    int descent = 0;
    int line_gap = 0;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);

    atlas.pixel_size     = pixel_size;
    atlas.ascent         = (int)roundf((float)ascent * scale);
    atlas.descent        = (int)roundf((float)(-descent) * scale);
    atlas.line_height    = (int)roundf((float)(ascent - descent + line_gap) * scale);
    if (atlas.line_height <= 0) {
        atlas.line_height = pixel_size;
    }

    const int first_codepoint = 32;
    const int last_codepoint = 126;
    const int glyph_count = last_codepoint - first_codepoint + 1;
    const int atlas_sizes[] = { 128, 256, 512, 1024, 2048 };
    const int glyph_padding = 2;
    stbtt_packedchar packed_chars[glyph_count];
    unsigned char *bitmap = NULL;

    for (unsigned int i = 0; i < sizeof(atlas_sizes) / sizeof(atlas_sizes[0]); ++i) {
        atlas.atlas_width = atlas_sizes[i];
        atlas.atlas_height = atlas_sizes[i];
        bitmap = (unsigned char *)MemAlloc((size_t)atlas.atlas_width * (size_t)atlas.atlas_height);
        if (bitmap == NULL) {
            break;
        }

        memset(bitmap, 0, (size_t)atlas.atlas_width * (size_t)atlas.atlas_height);

        stbtt_pack_context pack_context = { 0 };
        if (!stbtt_PackBegin(&pack_context, bitmap, atlas.atlas_width, atlas.atlas_height, 0, glyph_padding, NULL)) {
            MemFree(bitmap);
            bitmap = NULL;
            break;
        }

        // Improve glyph edges when the atlas is rendered at fractional scales.
        stbtt_PackSetOversampling(&pack_context, 2, 2);

        int packed = stbtt_PackFontRange(&pack_context, font_data, 0, (float)pixel_size, first_codepoint, glyph_count, packed_chars);
        stbtt_PackEnd(&pack_context);
        if (packed) {
            break;
        }

        MemFree(bitmap);
        bitmap = NULL;
    }

    if (bitmap == NULL) {
        UnloadFileData(font_data);
        memset(&atlas, 0, sizeof(atlas));
        return atlas;
    }

    Image image = GenImageColor(atlas.atlas_width, atlas.atlas_height, BLANK);
    Color *pixels = (Color *)image.data;

    for (int codepoint = first_codepoint; codepoint <= last_codepoint; ++codepoint) {
        stbtt_packedchar packed = packed_chars[codepoint - first_codepoint];

        atlas.glyphs[codepoint] = (GUI_FontGlyph) {
            .src        = (Rectangle){
                (float)packed.x0,
                (float)packed.y0,
                (float)(packed.x1 - packed.x0),
                (float)(packed.y1 - packed.y0)
            },
            .advance    = packed.xadvance,
            .bearing_x  = packed.xoff,
            .bearing_y  = packed.yoff,
            .bearing_x2 = packed.xoff2,
            .bearing_y2 = packed.yoff2
        };
    }

    for (int y = 0; y < atlas.atlas_height; ++y) {
        for (int x = 0; x < atlas.atlas_width; ++x) {
            unsigned char alpha = bitmap[y * atlas.atlas_width + x];
            pixels[y * atlas.atlas_width + x] = (Color){ 255, 255, 255, alpha };
        }
    }

    atlas.texture = LoadTextureFromImage(image);
    UnloadImage(image);
    MemFree(bitmap);
    UnloadFileData(font_data);

    if (atlas.texture.id != 0) {
        SetTextureFilter(atlas.texture, TEXTURE_FILTER_POINT);
        atlas.ready = true;
    }

    return atlas;
}

void GUI_DrawFontAtlasText(const GUI_FontAtlas *atlas, const char *text, Vector2 position, Color tint, float scale, float spacing)
{
    if (atlas == NULL || !atlas->ready || text == NULL) {
        return;
    }

    float pen_x = position.x;
    float pen_y = position.y;
    float baseline_y = pen_y + (float)atlas->ascent * scale;

    for (const unsigned char *ptr = (const unsigned char *)text; *ptr != '\0'; ++ptr) {
        unsigned char codepoint = *ptr;
        if (codepoint == '\n') {
            pen_x = position.x;
            pen_y += (float)atlas->line_height * scale;
            baseline_y = pen_y + (float)atlas->ascent * scale;
            continue;
        }

        if (codepoint >= GUI_FONT_ATLAS_GLYPHS) {
            codepoint = '?';
        }

        GUI_FontGlyph glyph = atlas->glyphs[codepoint];
        if (glyph.advance == 0.0f && codepoint != ' ') {
            glyph = atlas->glyphs[(unsigned char)'?'];
        }

        float glyph_w = (glyph.bearing_x2 - glyph.bearing_x) * scale;
        float glyph_h = (glyph.bearing_y2 - glyph.bearing_y) * scale;
        if (glyph_w > 0.0f && glyph_h > 0.0f) {
            Rectangle src = glyph.src;
            Rectangle dst = {
                SnapFloat(pen_x + glyph.bearing_x * scale),
                SnapFloat(baseline_y + glyph.bearing_y * scale),
                glyph_w,
                glyph_h
            };
            DrawTexturePro(atlas->texture, src, dst, (Vector2){ 0 }, 0.0f, tint);
        }

        pen_x += glyph.advance * scale + spacing;
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
        if (glyph.advance == 0.0f && codepoint != ' ') {
            glyph = atlas->glyphs[(unsigned char)'?'];
        }

        current_x += glyph.advance * scale + spacing;
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
