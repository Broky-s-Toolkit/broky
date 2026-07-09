#pragma once
#ifndef IDE_SYNTAX_HL
 #include "__core.h"
 #include "_setup.h"
 #include "_overlay.h"
 #include "_font.h"
#endif

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
float       GUI_Icon(Texture2D* texture2d, Vector2 position, float height, Color tint);
bool        GUI_IconButton(Texture2D* texture2d, Vector2 position, float height, Color tint);
// > IMAGES
void        GUI_Face(Vector2 position, float height);
void        GUI_Image(Texture2D texture, Rectangle shape);
// > BUTTON
void GUI_DrawButton(Rectangle shape, const char *text, Texture2D *icon, EGUI_ControlStatus status, EGUI_ThemeColor colors, EGUI_Font font);
bool GUI_Button(Rectangle shape, const char* text, Texture2D* icon, EGUI_ThemeColor colors);
bool GUI_ButtonMenu(Rectangle shape, const void* owner_id, const char* text, Texture2D* icon, EGUI_ThemeColor colors, void (*draw_function)(void));
bool GUI_ButtonMenuContents(int x, int x_end, int start_row, EGUI_ThemeColor colors, GUI_MenuItems *items, Rectangle *final_shape);
// > TEXT
void GUI_DrawText(Rectangle shape, const char* text, EGUI_ThemeColor colors, EGUI_Font font);
void GUI_Text(Rectangle shape, const char* text, EGUI_ThemeColor colors);
// > INPUTS
void GUI_DrawInput(Rectangle shape, char* buffer, int blink_cursor, EGUI_ControlStatus status, EGUI_ThemeColor colors, bool blink, EGUI_Font font);
void GUI_Input(Rectangle shape, void *owner, char *buffer, int buffer_size, EGUI_InputType type, EGUI_ThemeColor colors);
void GUI_Float(Rectangle shape, float *value, EGUI_ThemeColor colors, float min, float max);
// > CHECK
void GUI_DrawCheck(Rectangle shape, bool value, const char *on_txt, const char *off_txt, EGUI_ControlStatus status, EGUI_ThemeColor colors, EGUI_Font font);
void GUI_Check(Rectangle shape, bool *value, const char *on_txt, const char *off_txt, EGUI_ThemeColor colors);


// > IMPLEMENTATION
#ifdef IMPLEMENT_ALL

// > BASE MACROS
#define GUI_MACRO_IS_CURSOR_ACTIVE

#define GUI_BASE_CONTROL_ACTIVATED(shape) \
    /* > GUI_BASE_CONTROL_ACTIVATED                                         */\
    /*   is_activable       : if there is no resize or moving action        */\
    /*   is_cursor_over    : cursor currently within control bounds         */\
    /*   is_cursor_active  : user pressed mouse or enter key this frame     */\
    /*   is_active          : control activated                             */\
    /* Conditions */ \
    bool is_activable       = GUI_CTX.temp->window_current_action == EGUI_WindowAction_None;    \
    bool is_cursor_over     = is_activable && GUI_CheckCollisionCursorControlWin(shape);        \
    bool is_cursor_active   = is_activable && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);          \
    \
    /* Activation */                                            \
    bool is_active = is_cursor_over && is_cursor_active;        \
    /* Update cursor_over_gui */                                \
    if (is_cursor_over) GUI_CTX.temp->cursor_over_gui = true;   \

#define GUI_BASE_CONTROL_FOCUSED(value, shape) \
    /* > GUI_BASE_CONTROL_ACTIVATED                                         */\
    /*   is_activable       : if there is no resize or moving action        */\
    /*   is_cursor_over    : cursor currently within control bounds         */\
    /*   is_cursor_active  : user pressed mouse or enter key this frame     */\
    /*   just_focused       : control gained focus on this frame            */\
    /*   is_focused         : control retains focus state                   */\
    /* Conditions */ \
    bool is_activable       = GUI_CTX.temp->window_current_action == EGUI_WindowAction_None;    \
    bool is_cursor_over     = is_activable && GUI_CheckCollisionCursorControlWin(shape);        \
    bool is_cursor_active   = is_activable && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT));        \
    \
    /* Gains focus */                                           \
    bool just_focused = is_cursor_over && is_cursor_active;     \
    if (just_focused) GUI_CTX.temp->control_focus_ptr = value;  \
    \
    /* Focused control */                                       \
    bool is_focused = GUI_CTX.temp->control_focus_ptr == value; \
    /* Update cursor_over_gui */                                \
    if (is_cursor_over) GUI_CTX.temp->cursor_over_gui = true;   \

// < END BASE MACROS


// > DRAW PRIMITIVES
void GUI_DrawShadow(Rectangle shape)
{
    Rectangle shadow = shape;
    // TODO@dc: improve
    shadow.x -= 2.0f;
    shadow.y += 2.0f;
    DrawRectangleRec(shadow, ColorAlpha(BLACK, 0.35f));
}

void GUI_DrawBorders(Rectangle shape, Color dark, Color light, float border, bool remove_corner)
{
    if (!remove_corner) {
        // Top
        DrawRectangleRec((Rectangle){ shape.x, shape.y, shape.width, border }, dark);
        // Left
        DrawRectangleRec((Rectangle){ shape.x, shape.y, border, shape.height }, dark);
        // Bottom
        DrawRectangleRec((Rectangle){ shape.x, shape.y + shape.height - border, shape.width, border }, light);
        // Right
        DrawRectangleRec((Rectangle){ shape.x + shape.width - border, shape.y, border, shape.height }, light);
    } else {
        // Top (with corner gaps)
        DrawRectangleRec((Rectangle){ shape.x + border, shape.y, shape.width - 2 * border, border }, dark);

        // Left (start after top gap)
        DrawRectangleRec((Rectangle){ shape.x, shape.y + border, border, shape.height - border }, dark);

        // Right (start after top gap)
        DrawRectangleRec((Rectangle){ shape.x + shape.width - border, shape.y + border, border, shape.height - border }, light);

        // Bottom (full width)
        DrawRectangleRec((Rectangle){ shape.x, shape.y + shape.height - border, shape.width, border }, light);
    }
}

void GUI_DrawAdjustedTextEx(const char* text, Vector2 position, Color tint, float scale, EGUI_Font font)
{
    GUI_State *state        = GUI_CTX.state;
    GUI_FontSetup *setup    = GUI_GetFontSetup(font);
    Font font_asset         = GUI_GetFontAsset(font);
    float font_scaled       = (float)font_asset.baseSize * setup->scale * scale;
    Vector2 delta_scaled    = Vector2Scale(setup->delta, state->scale);
    Vector2 position_final  = Vector2Add(position, delta_scaled);
    position_final.x        = SnapFloat(position_final.x);
    position_final.y        = SnapFloat(position_final.y);

    if (setup->atlas.ready) {
        GUI_DrawFontAtlasText(&setup->atlas, text, position_final, tint, setup->scale * scale, setup->spacing);
        return;
    }

    DrawTextEx(font_asset, text, position_final, font_scaled, setup->spacing, tint);
}

Vector2 GUI_MeasureText(const char* text, EGUI_Font font, float scale)
{
    GUI_FontSetup* setup    = GUI_GetFontSetup(font);

    if (setup->atlas.ready) {
        return GUI_MeasureFontAtlasText(&setup->atlas, text, setup->scale * scale, setup->spacing);
    }

    Font font_asset         = GUI_GetFontAsset(font);
    float font_scaled       = (float)font_asset.baseSize * setup->scale * scale;
    return MeasureTextEx(font_asset, text, font_scaled, setup->spacing);
}

Vector2 GUI_MeasureAdjustedText(const char* text, EGUI_Font font)
{
    // Extract data
    GUI_State *state        = GUI_CTX.state;
    GUI_FontSetup* setup    = &GUI_GetSetup()->fonts[font];
    Vector2 text_measure    = GUI_MeasureText(text, font, state->scale);

    // Results (or statements)
    Vector2 result = {
        text_measure.x + setup->blink_delta.x * state->scale * setup->scale,
        text_measure.y + setup->blink_delta.y * state->scale * setup->scale
    };

    // Adjust
    Vector2 delta_scaled = Vector2Scale(setup->delta, state->scale);
    return Vector2Add(result, delta_scaled);
}
// < END DRAW PRIMITIVES

// > CURSOR
void GUI_DrawCursorFor(EGUI_Cursor cursor)
{
    GUI_CursorSetup* setup      = &GUI_CTX.setup->cursors[cursor];
    Vector2 mouse_current       = GUI_CTX.temp->cursor_current;
    Texture texture             = setup->texture;
    Vector2 delta_normalized    = setup->delta_normalized;
    float scale                 = setup->scale * GUI_CTX.state->scale;
    if (scale == 0) {
        return;
    }

    Vector2 mouse_shape = (Vector2) {
        mouse_current.x - ((float)texture.width * delta_normalized.x * scale),
        mouse_current.y - ((float)texture.height * delta_normalized.y * scale)
    };
    DrawTextureEx(texture, mouse_shape, 0, scale, ColorAlpha(WHITE, setup->alpha));
}

void GUI_DrawCursor(void)
{
    EGUI_Cursor cursor              = GUI_CTX.temp->cursor;
    GUI_CursorSetup* cursor_setup   = &GUI_CTX.setup->cursors[cursor];
    if (cursor_setup->additional_cursor != EGUI_Cursor_None) {
        GUI_DrawCursorFor(cursor_setup->additional_cursor);
    }
    GUI_DrawCursorFor(cursor);
}

// raylib [shapes] example - Draw a mouse trail (position history)
void GUI_DrawCursorTrail(void)
{
    GUI_CursorSetup *setup          = GUI_GetCursorSetup();
    Vector2 mouse                   = GUI_CTX.temp->cursor_current;
    float scale                     = setup->scale * GUI_CTX.state->scale;
    Vector2 delta_normalized        = setup->trail_delta_normalized;
    Vector2 delta                   = (Vector2) {
        .x = delta_normalized.x * (float)setup->texture.width * scale,
        .y = delta_normalized.y * (float)setup->texture.height * scale
    };

    // Shift all existing positions backward by one slot in the array
    // The last element (the oldest position) is dropped
    Vector2 *trail = GUI_CTX.temp->cursor_trail;
    for (int i = GUI_MAX_TRAIL - 1; i > 0; i--) {
        trail[i] = trail[i - 1];
    }
    trail[0] = Vector2Add(mouse, delta);

    for (int i = 0; i < GUI_MAX_TRAIL; i++) {
        // Ensure we skip drawing if the array hasn't been fully filled on startup
        if ((trail[i].x != 0.0f) || (trail[i].y != 0.0f))
        {
            #define TRAIL_ALPHA_MIN 0.001f
            #define TRAIL_ALPHA_MAX 0.05f
            float current_ratio = (float)(GUI_MAX_TRAIL - i) / GUI_MAX_TRAIL;
            float trail_alpha   = current_ratio * (TRAIL_ALPHA_MAX - TRAIL_ALPHA_MIN) + TRAIL_ALPHA_MIN;
            Color trail_color   = Fade(BLACK, trail_alpha);
            float trail_radius  = current_ratio * scale;

            DrawCircleV(Vector2AddValue(trail[i], -trail_radius), trail_radius, trail_color);
        }
    }
}
// < END CURSOR

// > ICONS
// Returns used texture_scale to draw the texture in the available height
float GUI_DrawIcon(Rectangle shape, Texture2D* texture2d, Color tint)
{
    GUI_Setup *setup    = GUI_CTX.setup;
    float texture_scale = shape.height / (float)texture2d->height;
    Vector2 position    = Vector2Add((Vector2) { shape.x, shape.y }, setup->icons.icon_delta);

    GUI_BeginControlScissor();
        DrawTextureEx(*texture2d, position, 0, texture_scale, tint);
    EndScissorMode();
    return texture_scale;
}

float GUI_Icon(Texture2D* texture2d, Vector2 position, float height, Color tint)
{
    Assert(height > 0);
    if (texture2d == NULL) return 0;
    Rectangle shape = RectFromVector2(position, height, height);
    return GUI_DrawIcon(shape, texture2d, tint);
}

bool GUI_IconButton(Texture2D* texture2d, Vector2 position, float height, Color tint)
{
    Rectangle shape = RectFromVector2(position, height, height);
    GUI_BASE_CONTROL_ACTIVATED(shape);

    GUI_Theme *theme    = &GUI_CTX.setup->theme;
    float color_change  = theme->color_change;

    if (is_cursor_over)
        GUI_Icon(texture2d, position, height, WHITE);
    else
        GUI_Icon(texture2d, position, height, ColorBrightness(tint, -color_change));
    return is_active;
}
// < END ICONS

// > IMAGES
void GUI_Face(Vector2 position, float height)
{
    Rectangle shape = { position.x, position.y, height, height };

    position.x = shape.x;
    position.y = shape.y;
    Assert(height > 0);

    GUI_Icons *icons = GUI_GetIcons();
    Vector2 mouse = GUI_CTX.temp->cursor_current;

    // Center of the face
    Vector2 center = (Vector2){ position.x + height / 2.0f, position.y + height / 2.0f };

    // --- Distance-based color ---
    float dx = mouse.x - center.x;
    float dy = mouse.y - center.y;
    float dist = sqrtf(dx * dx + dy * dy);

    float max_dist = height * 4;

    // Clamp and normalizing
    float t = 1.0f - fminf(dist / max_dist, 1.0f);

    // Interpolation white - red
    Color color = (Color){
        255,
        (unsigned char)(255 * (1.0f - t)),
        (unsigned char)(255 * (1.0f - t)),
        255
    };
    float texture_scale = GUI_DrawIcon(shape, &icons->Face, color);
    Assert(texture_scale > 0);

    // Determine quadrant relative to center
    int px = 0, py = 0;
    if (mouse.x >= center.x) px = 1; // right
    if (mouse.y >= center.y) py = 1; // bottom

    // Position of pixel (2x2 area)
    // float pixel_size = 2.0f * texture_scale;
    Vector2 pixel_pos = (Vector2){
        position.x + (float)px * texture_scale,
        position.y + (float)py * texture_scale
    };

    DrawRectangleV(Vector2Add((Vector2){ 4 * texture_scale, 6 * texture_scale }, pixel_pos), (Vector2){ texture_scale, texture_scale }, WHITE);
    DrawRectangleV(Vector2Add((Vector2){ 10 * texture_scale, 6 * texture_scale }, pixel_pos), (Vector2){ texture_scale, texture_scale }, WHITE);
}

void GUI_Image(Texture2D texture, Rectangle shape)
{


    if (texture.id == 0) return;

    float tex_ratio = (float)texture.width / (float)texture.height;
    float dst_ratio = shape.width / shape.height;

    Rectangle src = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle dst = { 0 };

    if (dst_ratio > tex_ratio) {
        // limit height
        dst.height = shape.height;
        dst.width  = shape.height * tex_ratio;
        dst.x = shape.x + (shape.width - dst.width) * 0.5f;
        dst.y = shape.y;
    } else {
        // limit width
        dst.width  = shape.width;
        dst.height = shape.width / tex_ratio;
        dst.x = shape.x;
        dst.y = shape.y + (shape.height - dst.height) * 0.5f;
    }

    GUI_BeginControlScissor();
        DrawTexturePro(texture, src, dst, (Vector2){0,0}, 0.0f, WHITE);
    EndScissorMode();
}
// < END IMAGES

// > BUTTON
void GUI_DrawButton(
    Rectangle shape, const char *text, Texture2D *icon,
    EGUI_ControlStatus status, EGUI_ThemeColor colors, EGUI_Font font)
{
    GUI_State *state            = GUI_CTX.state;
    GUI_FontSetup *font_setup   = GUI_GetFontSetup(font);
    GUI_Theme *theme            = &GUI_CTX.setup->theme;
    GUI_ThemeColors theme_colors = GUI_GetThemeColors(colors);

    float border        = font_setup->border;
    float scale         = state->scale;
    float color_change  = theme->color_change;
    float bg_alpha      = theme->bg_alpha;
    float icon_w        = icon != NULL ? GUI_GetIconWidthForShape(shape, border) : 0;

    Color bg_color =    status == EGUI_ControlStatus_Focused  ? theme_colors.bg_color_3 :
                        status == EGUI_ControlStatus_Collide  ? ColorBrightness(theme_colors.bg_color_2, color_change) :
                                                                theme_colors.bg_color_2;
    Color b_color_a =   status == EGUI_ControlStatus_Focused  ? theme_colors.bg_color_3:
                                                                theme_colors.bg_color_0;
    Color b_color_b =   theme_colors.bg_color_2;
    GUI_BeginControlScissor();
        DrawRectangleRec(shape,  ColorAlpha(bg_color, bg_alpha));
        GUI_DrawBorders(shape, b_color_a, b_color_b, border * scale, false);
    EndScissorMode();

    GUI_BeginInnerControlScissor(shape, border, scale);
        GUI_DrawAdjustedTextEx(text,
            (Vector2){ shape.x + icon_w + (border) * scale, shape.y + (border) * scale},
            theme_colors.tx_color_0, scale, font);

    if (icon_w > 0) {
        GUI_Icon(icon, (Vector2) { shape.x + font_setup->border * state->scale, shape.y + font_setup->border * state->scale }, icon_w, WHITE);
    }
    EndScissorMode();
}

bool GUI_Button(
    Rectangle shape, const char* text, Texture2D* icon,
    EGUI_ThemeColor colors)
{
    GUI_BASE_CONTROL_ACTIVATED(shape);

    EGUI_Font font              = GUI_GetFont();
    EGUI_ControlStatus status   = EGUI_ControlStatus_Default;
    bool cursor_active          = is_cursor_over && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    if (is_cursor_over)  status = EGUI_ControlStatus_Collide;
    if (cursor_active)   status = EGUI_ControlStatus_Focused;
    GUI_DrawButton(shape, text, icon, status, colors, font);
    // TODO@dc: improve colors DrawDebugRect(shape, ColorAlpha(is_cursor_over? RED : BLUE, 0.2));
    return is_active;
}

bool GUI_ButtonMenu(
    Rectangle shape, const void* owner_id, const char* text, Texture2D* icon,
    EGUI_ThemeColor colors, void (*draw_function)(void))
{
    bool scrolled           = GetMouseWheelMove() != 0;
    bool is_open            = GUI_OverlayOpenedBy(owner_id);
    bool just_interacted    = GUI_Button(shape, text, icon, colors);
    if (just_interacted) {
        if (is_open == false) {
            GUI_OverlayOpenFor(owner_id);
        } else {
            GUI_OverlayClose();
        }
    }

    // Update condition
    is_open = GUI_OverlayOpenedBy(owner_id);

    // Update
    if (is_open) {
        if (scrolled) {
            GUI_OverlayClose();
        } else {
            // Set overlay call
            GUI_OverlaySetDrawCall(just_interacted, colors, draw_function);
        }
    }

    // Update condition
    is_open = GUI_OverlayOpenedBy(owner_id);
    return is_open;
}

bool GUI_ButtonMenuContents(int x, int x_end, int start_row, EGUI_ThemeColor colors, GUI_MenuItems *items, Rectangle *final_shape)
{
    Assert(items != NULL);
    Assert(items->count > 0);
    Assert(items->selected_value != NULL);
    Assert(final_shape != NULL);

    int visible_rows = items->count - 1;
    if (visible_rows <= 0) {
        *final_shape = (Rectangle){ 0 };
        return false;
    }

    int selected_index = 0;
    for (int i = 0; i < items->count; i++) {
        if (items->elements[i].value == *items->selected_value) {
            selected_index = i;
            break;
        }
    }

    bool changed        = false;
    int final_row       = start_row + visible_rows - 1;
    *final_shape        = GUI_GridBetween(x, start_row, x_end, final_row);
    GUI_DrawShadow(*final_shape);
    if (GUI_OverlayWasJustEnabled() == false) {
        int draw_row = start_row;
        for (int item_index = 0; item_index < items->count; item_index++) {
            if (item_index == selected_index) continue;

            const GUI_MenuItem *item = &items->elements[item_index];
            Rectangle item_shape = GUI_GridBetween(x, draw_row, x_end, draw_row);
            if (GUI_Button(item_shape, item->label, GUI_GetIconTexture(item->icon), colors)) {
                changed = *items->selected_value != item->value;
                *items->selected_value = item->value;
            }
            draw_row++;
        }
    }
    return changed;
}
// < END BUTTON

// > TEXT
void GUI_DrawText(
    Rectangle shape, const char* text,
    EGUI_ThemeColor colors, EGUI_Font font)
{
    GUI_State *state            = GUI_CTX.state;
    GUI_FontSetup *font_setup   = GUI_GetFontSetup(font);
    GUI_ThemeColors theme_colors = GUI_GetThemeColors(colors);

    float border    = font_setup->border;
    float scale     = state->scale;

    GUI_BeginControlScissor();
        GUI_DrawAdjustedTextEx(text,
            (Vector2){ shape.x + (border) * scale, shape.y + (border) * scale},
            theme_colors.tx_color_0, scale, font);
    EndScissorMode();
}

void GUI_Text(Rectangle shape, const char* text, EGUI_ThemeColor colors)
{

    GUI_DrawText(shape, text, colors, GUI_GetFont());
}
// < END TEXT

// > INPUTS
void GUI_DrawInput(
    Rectangle shape, char* buffer, int blink_cursor,
    EGUI_ControlStatus status, EGUI_ThemeColor colors, bool blink, EGUI_Font font)
{
    GUI_State       *state          = GUI_CTX.state;
    GUI_FontSetup   *font_setup     = GUI_GetFontSetup(font);
    GUI_Theme       *theme          = &GUI_CTX.setup->theme;
    GUI_ThemeColors theme_colors    = GUI_GetThemeColors(colors);

    float border        = font_setup->border;
    float scale         = state->scale;
    float color_change  = theme->color_change;
    float bg_alpha      = theme->bg_alpha;

    GUI_BeginControlScissor();
        if (status == EGUI_ControlStatus_Default)
            DrawRectangleRec(shape, ColorAlpha(theme_colors.bg_color_1, bg_alpha));
        else if (status == EGUI_ControlStatus_Collide)
            DrawRectangleRec(shape, ColorAlpha(ColorBrightness(theme_colors.bg_color_1, color_change), bg_alpha));
        else if (status == EGUI_ControlStatus_Focused)
            DrawRectangleRec(shape, ColorAlpha(ColorBrightness(theme_colors.bg_color_1, -color_change), bg_alpha));

        if (status == EGUI_ControlStatus_Focused)
            GUI_DrawBorders(shape, ColorBrightness(theme_colors.bg_color_2, -color_change), ColorBrightness(theme_colors.bg_color_0, color_change), border * scale, false);
        else
            GUI_DrawBorders(shape, theme_colors.bg_color_2, theme_colors.bg_color_0, border * scale, false);
    EndScissorMode();

    // Auto horizontal scroll
    float auto_scroll_x = 0.0f;
    if (status == EGUI_ControlStatus_Focused) {
        float auto_scroll_right = 0.9f;
        float auto_scroll_left = 0.1f;
        int text_size = StringSize(buffer);
        int cursor_idx = Clamp(blink_cursor, 0, text_size);
        Vector2 cursor_pos = GUI_MeasureAdjustedText(TextSubtext(buffer, 0, cursor_idx), font);

        float visible_w = shape.width - (2.0f * border * scale);
        float cursor_x  = cursor_pos.x;

        // Right
        if (cursor_x - auto_scroll_x > visible_w * auto_scroll_right)
            auto_scroll_x = cursor_x - visible_w * auto_scroll_right;
        // Left
        else if (cursor_x - auto_scroll_x < visible_w * auto_scroll_left)
            auto_scroll_x = cursor_x - visible_w * auto_scroll_left;

        if (auto_scroll_x < 0) auto_scroll_x = 0;
    }

    GUI_BeginInnerControlScissor(shape, border, scale);
        GUI_DrawAdjustedTextEx(buffer,
            (Vector2) {
                shape.x + (border) * scale - auto_scroll_x,
                shape.y + (border) * scale
            }, theme_colors.tx_color_0, scale, font);


        if (status == EGUI_ControlStatus_Focused && blink) {
            int current_text_size = StringSize(buffer);
            int cursor_idx = Clamp(blink_cursor, 0, current_text_size);
            Vector2 text_size = GUI_MeasureAdjustedText(TextSubtext(buffer, 0, cursor_idx), font);
            DrawRectangleRec((Rectangle){
                shape.x + (border + font_setup->blink_delta.x) * scale + text_size.x - auto_scroll_x,
                shape.y + (border + font_setup->blink_delta.y) * scale,
                font_setup->blink_size.x * scale,
                font_setup->blink_size.y * scale
            }, ColorAlpha(theme_colors.tx_color_0, font_setup->blink_alpha));
        }
    EndScissorMode();
}

void GUI_Input(
    Rectangle shape, void *owner, char *buffer, int buffer_size,
    EGUI_InputType type, EGUI_ThemeColor colors)
{
    Assert(buffer != NULL);
    Assert(buffer_size > 0);
    int max_text_size = buffer_size - 1;

    GUI_BASE_CONTROL_FOCUSED(owner, shape)

    // Blink (text cursor)
    static void *last_control_focus_ptr = NULL;

    if (is_cursor_over) {
        GUI_CTX.temp->cursor = EGUI_Cursor_Text;
    }

    // Blink data
    const float     blink_speed     = 0.5f;
    static float    blink_timer     = 0.0f;
    static bool     blink_state     = 0;
    static int      blink_cursor    = 0;

    // Font type
    EGUI_Font font         = GUI_GetFont();

    // Gain focus
    if (just_focused) {
        // Activate blink cursor
        blink_state = true;
        blink_timer = 0;

        // Re-locate cursor
        if (last_control_focus_ptr != buffer) {
            blink_cursor = 0;
        }
        last_control_focus_ptr = buffer;

        float mouse_x   = GUI_CTX.temp->cursor_current.x - shape.x;
        int text_size   = StringSize(buffer);
        bool right_test = mouse_x > GUI_MeasureAdjustedText(buffer, font).x;
        if (right_test) {
            blink_cursor = text_size;
        } else {
            for (blink_cursor = 0; blink_cursor < text_size; blink_cursor++) {
                float w = GUI_MeasureAdjustedText(TextSubtext(buffer, 0, blink_cursor), font).x;
                if (mouse_x < w) break;
            }
            // Move one position to the left for a more accurate mouse-to-text alignment
            blink_cursor = IntMax(0, --blink_cursor);
        }
    }

    // Focused
    if (is_focused) {
        int text_size = StringSize(buffer);
        blink_cursor = Clamp(blink_cursor, 0, text_size);

        // Handle text input
        int key = GetCharPressed();
        while (key > 0) {
            // Special cases
            //
            if (type == EGUI_Input_Int || type == EGUI_Input_Float) {
                if (key == '-') {
                    // Remove minus
                    if (buffer[0] == '-') {
                        int len = StringSize(buffer);
                        for (int i = 0; i < len; i++)
                            buffer[i] = buffer[i + 1];
                        blink_cursor--;
                        if (blink_cursor < 0) blink_cursor = 0;
                    // Add minus
                    } else {
                        int len = StringSize(buffer);
                        if (len < max_text_size) {
                            for (int i = len; i >= 0; i--)
                                buffer[i + 1] = buffer[i];
                            buffer[0] = '-';
                            blink_cursor++;
                        }
                    }

                    // Already handled, skip.
                    key = GetCharPressed();
                    continue;
                }
            }

            // Default cases
            bool valid = false;
            switch (type) {
            case EGUI_Input_Text:
                valid = (key >= 32 && key <= 126);
                break;
            case EGUI_Input_Int:
                if ((key >= '0' && key <= '9')) {
                    valid = true;
                }
                break;
            case EGUI_Input_Float:
                if ((key >= '0' && key <= '9') ||
                    (key == '.' && strchr(buffer, '.') == NULL)) {
                    valid = true;
                }
                break;
            }

            if (valid && text_size < max_text_size) {
                // Move chars to the right
                for (int i = text_size; i >= blink_cursor; i--) {
                    buffer[i + 1] = buffer[i];
                }
                buffer[blink_cursor] = (char)key;
                (blink_cursor)++;
                text_size++;
            }
            key = GetCharPressed();
        }

        // Erase
        if (IsKeyPressed(KEY_BACKSPACE) && blink_cursor > 0) {
            for (int i = blink_cursor - 1; i < text_size; i++) {
                buffer[i] = buffer[i+1];
            }
            blink_cursor--;
            text_size--;
        }
        if (IsKeyPressed(KEY_DELETE) && blink_cursor < text_size) {
            for (int i = blink_cursor; i < text_size; i++) {
                buffer[i] = buffer[i+1];
            }
            text_size--;
        }

        // Cursor movement
        if (IsKeyPressed(KEY_LEFT) && blink_cursor > 0)
            blink_cursor--;
        if (IsKeyPressed(KEY_RIGHT) && blink_cursor < text_size)
            blink_cursor++;
        if (IsKeyPressed(KEY_HOME))
            blink_cursor = 0;
        if (IsKeyPressed(KEY_END))
            blink_cursor = text_size;

        // Blink update
        if (blink_state)
            blink_timer += GetFrameTime();
        else
            blink_timer -= GetFrameTime();
        // Blink loop
        if (blink_timer > blink_speed)
            blink_state = 0;
        if (blink_timer < 0)
            blink_state = 1;
    }

    EGUI_ControlStatus status =
        is_focused      ? EGUI_ControlStatus_Focused :
        is_cursor_over ? EGUI_ControlStatus_Collide :
                          EGUI_ControlStatus_Default;

    GUI_DrawInput(shape, buffer, blink_cursor, status, colors, blink_state, font);
}

void GUI_Float(Rectangle shape, float *value, EGUI_ThemeColor colors, float min, float max)
{
    Rectangle shape_original = shape;

    static char buf_default[256] = {0};
    static char buf_focused[256] = {0};
    GUI_BASE_CONTROL_FOCUSED(value, shape)

    if (just_focused) {
        snprintf(buf_focused, sizeof(buf_focused), "%.6g", (double)*value);
    } else {
        snprintf(buf_default, sizeof(buf_default), "%.6g", (double)*value);
    }
    char *buf = is_focused ? buf_focused : buf_default;
    GUI_Input(shape_original, value, buf,
        sizeof(buf_default) /* Intentionally not using buf */,
        EGUI_Input_Float, colors);

    if (is_focused) {
        float parsed;
        if (ParseFloatStrict(buf, &parsed)) {
            *value = Clamp(parsed, min, max);
        }
    }
}
// < END INPUTS


// > CHECK
void GUI_DrawCheck(
    Rectangle shape, bool value, const char *on_txt, const char *off_txt,
    EGUI_ControlStatus status, EGUI_ThemeColor colors, EGUI_Font font)
{
    GUI_State       *state          = GUI_CTX.state;
    GUI_FontSetup   *font_setup     = GUI_GetFontSetup(font);
    GUI_Theme       *theme          = &GUI_CTX.setup->theme;
    GUI_ThemeColors theme_colors    = GUI_GetThemeColors(colors);

    float border        = font_setup->border;
    float scale         = state->scale;
    float color_change  = theme->color_change;
    float bg_alpha      = theme->bg_alpha;

    Color tx = value ? theme_colors.tx_color_0 : theme_colors.bg_color_0;
    Color bg = value ? theme_colors.bg_color_3 : theme_colors.bg_color_2;
    Color b1 = value ? theme_colors.bg_color_2 : theme_colors.bg_color_0;
    Color b2 = value ? theme_colors.bg_color_0 : theme_colors.bg_color_2;

    GUI_BeginControlScissor();
        if (status == EGUI_ControlStatus_Default)
            DrawRectangleRec(shape, ColorAlpha(bg, bg_alpha));
        else if (status == EGUI_ControlStatus_Collide)
            DrawRectangleRec(shape, ColorAlpha(ColorBrightness(bg, color_change), bg_alpha));
        else if (status == EGUI_ControlStatus_Focused)
            DrawRectangleRec(shape, ColorAlpha(ColorBrightness(bg, -color_change), bg_alpha));

        if (status == EGUI_ControlStatus_Focused)
            GUI_DrawBorders(shape, ColorBrightness(b1, -color_change), ColorBrightness(b2, color_change), border * scale, false);
        else
            GUI_DrawBorders(shape, b1, b2, border * scale, false);
    EndScissorMode();

    GUI_BeginInnerControlScissor(shape, border, scale);
        GUI_DrawAdjustedTextEx(value ? on_txt : off_txt,
            (Vector2){ shape.x + (border) * scale, shape.y + (border) * scale},
            tx, scale, font);
    EndScissorMode();
}

void GUI_Check(
    Rectangle shape, bool *value, const char *on_txt, const char *off_txt,
    EGUI_ThemeColor colors)
{

    GUI_BASE_CONTROL_FOCUSED(value, shape)

    // Focused
    if (is_focused) {
        if (is_cursor_active) {
            // Toggle checkbox
            *value = !(*value);
        }
    }

    EGUI_Font font   = GUI_GetFont();
    EGUI_ControlStatus status =
        is_focused      ? EGUI_ControlStatus_Focused :
        is_cursor_over ? EGUI_ControlStatus_Collide :
                          EGUI_ControlStatus_Default;
    GUI_DrawCheck(shape, *value, on_txt, off_txt, status, colors, font);
}
// < END CHECK

#endif
