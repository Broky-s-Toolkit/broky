#pragma once
#ifndef IDE_SYNTAX_HL
 #include "gui_base.h"
#endif


// > SUBMODULE: SETUP
// > INDEX

// > CURSORS
GUI_CursorSetup     GUI_LoadCursorSetupForType(EGUI_Cursor cursor);
GUI_CursorSetup*    GUI_GetCursorSetup(void);
// > FONTS
GUI_FontSetup       GUI_LoadFontSetupDefault(EGUI_Font font);
void                GUI_ReloadFontSetupAsset(EGUI_Font font);
GUI_FontSetup*      GUI_GetFontSetup(EGUI_Font font);
Font                GUI_GetFontAsset(EGUI_Font font);
EGUI_Font           GUI_GetFont(void);
void                GUI_SetFont(EGUI_Font font);
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
float               GUI_GetIconWidth(void);
float               GUI_GetIconWidthForShape(Rectangle shape, float border);
float               GUI_GetIconSmallWidth(void);
// > COMMON
GUI_Setup           GUI_LoadSetupDefault(void);

// > IMPLEMENTATION
#ifdef IMPLEMENT_ALL

// > CURSORS
GUI_CursorSetup GUI_LoadCursorSetupForType(EGUI_Cursor cursor)
{
    GUI_CursorSetup setup = { 0 };
    _Static_assert(EGUI_Cursor_Count  == 5,  "Update cursors here!");

    switch (cursor)
    {
        case EGUI_Cursor_AGS:
            setup.texture                   = LoadTexture(BROKY_ICO_ROOT "/cursor.png");
            setup.delta_normalized          = (Vector2){ 0.5f, 0.5f };
            setup.scale                     = 2.0f;
            setup.alpha                     = 1.0f;
            setup.trail_delta_normalized    = (Vector2){ 1.0f, 1.0f };
            setup.additional_cursor         = EGUI_Cursor_None;
            break;

        case EGUI_Cursor_Text:
            setup.texture                   = LoadTexture(BROKY_ICO_ROOT "/pointer_txt.png");
            setup.delta_normalized          = (Vector2){ -1.0f, 0.0f };
            setup.scale                     = 2.0f;
            setup.alpha                     = 0.85f;
            setup.trail_delta_normalized    = (Vector2){ 1.0f, 1.0f };
            setup.additional_cursor         = EGUI_Cursor_Default;
            break;

        case EGUI_Cursor_Resize:
            setup.texture                   = LoadTexture(BROKY_ICO_ROOT "/pointer_resize.png");
            setup.delta_normalized          = (Vector2){ 0.0f, 0.0f };
            setup.scale                     = 2.0f;
            setup.alpha                     = 1.0f;
            setup.trail_delta_normalized    = (Vector2){ 1.0f, 1.0f };
            setup.additional_cursor         = EGUI_Cursor_None;
            break;

        case EGUI_Cursor_Default:
        default:
            setup.texture                   = LoadTexture(BROKY_ICO_ROOT "/pointer.png");
            setup.delta_normalized          = (Vector2){ 0.0f, 0.0f };
            setup.scale                     = 2.0f;
            setup.alpha                     = 1.0f;
            setup.trail_delta_normalized    = (Vector2){ 1.0f, 1.0f };
            setup.additional_cursor         = EGUI_Cursor_None;
            break;

    }

    return setup;
}

GUI_CursorSetup* GUI_GetCursorSetup(void)
{
    EGUI_Cursor cursor = GUI_CTX.temp->cursor;
    return &GUI_CTX.setup->cursors[cursor];
}
// < CURSORS

// > FONTS
static const char *GUI_GetFontAssetPath(EGUI_Font font)
{
    switch (font) {
    case EGUI_Font_ShareTech:
        return BROKY_FNT_ROOT "/ShareTech-Regular.ttf";
    case EGUI_Font_Default:
        return BROKY_FNT_ROOT "/unifont-17.0.01.otf";
    case EGUI_Font_GUI:
    default:
        return NULL;
    }
}

GUI_FontSetup GUI_LoadFontSetupDefault(EGUI_Font font)
{
    _Static_assert(EGUI_Font_Count == 3,  "Update fonts here");

    switch (font) {
    case EGUI_Font_GUI: {
        GUI_FontSetup result = {
            .default_height = 36,
            .border         = 2.0f,
            .scale          = 2.0f,
            .delta          = (Vector2){ 6.0f, 6.0f },
            .custom         = { 0 },
            .use_custom     = false,
            .use_atlas      = false,
            .atlas          = { 0 },
            .atlas_reload_size = 32,
            .spacing        = 1.0f,
            .blink_size     = (Vector2){ 1.0f, 30.0f },
            .blink_delta    = (Vector2){ 0.0f, 0.0f },
            .blink_alpha    = 0.95f
        };
        return result;
    }
    case EGUI_Font_ShareTech: {
        GUI_FontSetup result = {
            .default_height = 36,
            .border         = 2.0f,
            .scale          = 1.0f,
            .delta          = (Vector2){ 6.0f, 6.0f },
            .custom         = { 0 },
            .use_custom     = false,
            .use_atlas      = true,
            .atlas          = { 0 },
            .atlas_reload_size = 32,
            .spacing        = 1.0f,
            .blink_size     = (Vector2){ 1.0f, 30.0f },
            .blink_delta    = (Vector2){ 0.0f, 0.0f },
            .blink_alpha    = 0.95f
        };
        result.atlas = GUI_LoadFontAtlasASCII(BROKY_FNT_ROOT "/ShareTech-Regular.ttf", 32);
        return result;
    }
    case EGUI_Font_Default:
    default: {
        GUI_FontSetup result = {
            .default_height = 36,
            .border         = 2.0f,
            .scale          = 1.0f,
            .delta          = (Vector2){ 6.0f, 6.0f },
            .custom         = LoadFontEx(BROKY_FNT_ROOT "/unifont-17.0.01.otf", 16, 0, 0),
            .use_custom     = true,
            .use_atlas      = false,
            .atlas          = { 0 },
            .atlas_reload_size = 32,
            .spacing        = 1.0f,
            .blink_size     = (Vector2){ 1.0f, 30.0f },
            .blink_delta    = (Vector2){ 0.0f, 0.0f },
            .blink_alpha    = 0.95f
        };
        SetTextureFilter(result.custom.texture, TEXTURE_FILTER_BILINEAR);
        return result;
    }
    } // Switch
}
GUI_FontSetup* GUI_GetFontSetup(EGUI_Font font)
{
    Assert(font >= 0 && font < EGUI_Font_Count);
    return &GUI_CTX.setup->fonts[font];
}

Font GUI_GetFontAsset(EGUI_Font font)
{
    GUI_Setup *setup = GUI_CTX.setup;
    if (setup->fonts[font].use_custom)
        return setup->fonts[font].custom;
    else
        return GetFontDefault();
}
EGUI_Font GUI_GetFont(void)
{
    EGUI_Font font = GUI_CTX.temp->current_font;
    return font;
}

void GUI_ReloadFontSetupAsset(EGUI_Font font)
{
    GUI_FontSetup *font_setup = GUI_GetFontSetup(font);
    const char *asset_path = GUI_GetFontAssetPath(font);

    if (font_setup->use_atlas) {
        int pixel_size = font_setup->atlas_reload_size > 0
            ? font_setup->atlas_reload_size
            : (font_setup->atlas.pixel_size > 0 ? font_setup->atlas.pixel_size : 32);

        if (font_setup->atlas.ready || font_setup->atlas.texture.id != 0) {
            GUI_UnloadFontAtlas(&font_setup->atlas);
        }

        if (asset_path != NULL) {
            font_setup->atlas = GUI_LoadFontAtlasASCII(asset_path, pixel_size);
            font_setup->atlas_reload_size = pixel_size;
        }
    }

    if (font_setup->use_custom) {
        if (font_setup->custom.texture.id != 0) {
            UnloadFont(font_setup->custom);
            font_setup->custom = (Font){ 0 };
        }

        if (asset_path != NULL) {
            int font_size = 16;
            font_setup->custom = LoadFontEx(asset_path, font_size, 0, 0);
            if (font_setup->custom.texture.id != 0) {
                SetTextureFilter(font_setup->custom.texture, TEXTURE_FILTER_BILINEAR);
            }
        }
    }
}

void GUI_SetFont(EGUI_Font font)
{
    GUI_CTX.temp->current_font = font;
}
// < FONTS

float GUI_CalcDefaultHeightScaled(EGUI_Font font)
{
    GUI_Setup* setup = GUI_CTX.setup;
    GUI_State* state = GUI_CTX.state;
    return setup->fonts[font].default_height * state->scale;
}

// > THEME
GUI_ThemeColors GUI_LoadThemeColorsDefault(EGUI_ThemeColor color)
{
    _Static_assert(EGUI_ThemeColor_Count == 4, "Update theme colors here");

    switch (color) {
    case EGUI_ThemeColor_Red:
        return GUI_GenerateThemeColors(3.0f);
    case EGUI_ThemeColor_Green:
        return GUI_GenerateThemeColors(97.0f);
    case EGUI_ThemeColor_Abstractica:
        return GUI_MakeThemeColors(
            MakeColor(153, 155, 163), MakeColor(150, 149, 150),
            MakeColor(65,67,72), MakeColor(43,45,48), MakeColor(25,26,28), MakeColor(29,25,30));
    case EGUI_ThemeColor_Gray:
    default:
        return GUI_GenerateThemeColors(180.0f);
    }
}

GUI_ThemeColors GUI_MakeThemeColors(Color tx_color_0, Color tx_color_1,
    Color bg_color_0, Color bg_color_1, Color bg_color_2, Color bg_color_3)
{
    GUI_ThemeColors colors = {
        .tx_color_0 = tx_color_0,
        .tx_color_1 = tx_color_1,
        .bg_color_0 = bg_color_0,
        .bg_color_1 = bg_color_1,
        .bg_color_2 = bg_color_2,
        .bg_color_3 = bg_color_3
    };
    return colors;
}

// Generates a color with a specified hue and interpolated saturation and value.
// Parameters:
//   hue: Hue value in degrees (e.g., 180.0f for cyan).
//   intensity: Interpolation factor [0.0, 1.25] controlling saturation and value .
// Returns:
//   A Raylib Color struct with RGB values (0-255) and full alpha (255).
Color GUI_GenerateThemeColor(float hue, float intensity) {
    // Normalize hue
    hue = fmodf(hue, 360.0f);
    if (hue < 0.0f) hue += 360.0f;

    // Clamp intensity
    if (intensity < 0.0f)  intensity = 0.0f;
    if (intensity > 1.25f) intensity = 1.25f;

    // Interpolation
    float saturation    = 0.13f + intensity * (0.24f - 0.13f); // 0.13 → 0.24
    float value         = 0.89f - intensity * (0.89f - 0.33f); // 0.89 → 0.33

    return ColorFromHSV(hue, saturation, value);
}

GUI_ThemeColors GUI_GenerateThemeColors(float hue)
{
    GUI_ThemeColors colors = {
        .tx_color_0 = GUI_GenerateThemeColor(hue, 0.0f),
        .tx_color_1 = GUI_GenerateThemeColor(hue, 1.15f), // darker than bg_color_3
        .bg_color_0 = GUI_GenerateThemeColor(hue, 0.25f),
        .bg_color_1 = GUI_GenerateThemeColor(hue, 0.5f),
        .bg_color_2 = GUI_GenerateThemeColor(hue, 0.75f),
        .bg_color_3 = GUI_GenerateThemeColor(hue, 1.0f)
    };
    return colors;
}

GUI_Theme GUI_GenerateTheme(void)
{
    GUI_Theme theme = {
        .bg_alpha       = 1.0f,
        .color_change   = 0.05f
    };

    for (int i = 0; i < EGUI_ThemeColor_Count; i++) {
        theme.colors[i] = GUI_LoadThemeColorsDefault((EGUI_ThemeColor)i);
    }

    return theme;
}
GUI_Theme GUI_GetTheme(void)
{
    return GUI_CTX.setup->theme;
}
GUI_ThemeColors GUI_GetThemeColors(EGUI_ThemeColor color)
{
    Assert(color >= 0 && color < EGUI_ThemeColor_Count);
    return GUI_CTX.setup->theme.colors[color];
}

EGUI_ThemeColor GUI_GetThemeColor(void)
{
    return GUI_CTX.temp->current_theme_colors;
}

void GUI_SetThemeColors(EGUI_ThemeColor color)
{
    GUI_CTX.temp->current_theme_colors = color;
}
// < THEME

// > ICONS
GUI_Icons GUI_LoadIcons(void)
{
    GUI_Icons icons = {
        .None           = (Texture2D){0},
        .New            = LoadTexture(BROKY_ICO_ROOT "/new.png"),
        .Open           = LoadTexture(BROKY_ICO_ROOT "/open.png"),
        .Save           = LoadTexture(BROKY_ICO_ROOT "/save.png"),
        .Setup          = LoadTexture(BROKY_ICO_ROOT "/setup.png"),
        .Error          = LoadTexture(BROKY_ICO_ROOT "/error.png"),
        .Face           = LoadTexture(BROKY_ICO_ROOT "/face.png"),
        .Dog            = LoadTexture(BROKY_ICO_ROOT "/dog.png"),
        .Close          = LoadTexture(BROKY_ICO_ROOT "/close.png"),
        .CloseSmall     = LoadTexture(BROKY_ICO_ROOT "/close-sm.png"),
        .MinimizeSmall  = LoadTexture(BROKY_ICO_ROOT "/minimize-sm.png"),
        .Layouts        = LoadTexture(BROKY_ICO_ROOT "/layouts.png"),
    };
    return icons;
}

GUI_IconSetup GUI_MakeIconSetup(GUI_Icons icons)
{
    GUI_IconSetup setup = {
        .icon_size      = 32,
        .icon_size_sm   = 16,
        .icon_delta     = (Vector2){ 0, 0 },
        .icons          = icons
    };
    return setup;
}

GUI_Icons* GUI_GetIcons(void)
{
    return &GUI_CTX.setup->icons.icons;
}

float GUI_GetIconWidth(void)
{
    return GUI_CTX.setup->icons.icon_size * GUI_CTX.state->scale;
}

float GUI_GetIconWidthForShape(Rectangle shape, float border)
{
    return shape.height - border * 2 * GUI_CTX.state->scale;
}

float GUI_GetIconSmallWidth(void)
{
    return GUI_CTX.setup->icons.icon_size_sm * GUI_CTX.state->scale;
}
// < ICONS

// > COMMON
GUI_Setup GUI_LoadSetupDefault(void)
{
    GUI_Setup setup = {
        .theme      = GUI_GenerateTheme(),
        .icons      = GUI_MakeIconSetup(GUI_LoadIcons())
    };

    // Fonts
    for (int i = 0; i < EGUI_Font_Count; i++) {
        setup.fonts[i] = GUI_LoadFontSetupDefault((EGUI_Font)i);
    }

    // Cursors
    // 0 is None
    for (int i = 1; i < EGUI_Cursor_Count; i++) {
        setup.cursors[i] = GUI_LoadCursorSetupForType((EGUI_Cursor)i);
    }
    return setup;
}
// < COMMON
#endif
