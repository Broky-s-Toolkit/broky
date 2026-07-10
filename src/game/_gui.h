#pragma once

#ifndef IDE_SYNTAX_HL
 #define IMPLEMENT_ALL   1
 #include "__core.h"
#endif

//
// INDEX
//
void GUI_ProgramMenu();
void GUI_GameMenu();
void GUI_FontSettingsMenu();
void GUI_FontFilterMenu();
void GUI_TopBar();
void WIN_Window(GUI_Window* window);
void WIN_Layouts(GUI_Window* window);
void WIN_CharacterDebug(GUI_Window* window);
void WIN_Settings(GUI_Window* window);
void WIN_FontSettings(GUI_Window* window);
void WIN_Logo(GUI_Window* window);
void WIN_Winman(GUI_Window* window);

//
// FUNCTIONS
//
#ifdef IMPLEMENT_ALL

void GUI_ProgramMenu()
{
    int x = -1;
    GUI_Icons *icons        = GUI_GetIcons();
    GUI_Params opt          = (GUI_Params) { .colors = GUI_OverlayColors() };
    GUI_BeginOverlay(true, true);
        GUI_Button(GUI_MakeBoxEx(GUI_GridAt(x,1), opt), "New", &icons->New);
        GUI_Button(GUI_MakeBoxEx(GUI_GridAt(x,2), opt), "Open", &icons->Open);
        if (GUI_Button(GUI_MakeBoxEx(GUI_GridAt(x,3), opt), "Quit", &icons->New)) {
            exit(0);
        }
    GUI_EndOverlay(GUI_GridBetween(x, 1, x, 3));
}

void GUI_GameMenu()
{
    GUI_Icons *icons        = GUI_GetIcons();
    GAME_Actions *actions   = &GAME_CTX.temp->player_actions;
    GUI_Params opt          = (GUI_Params) { .colors = GUI_OverlayColors() };
    GUI_BeginOverlay(true, true);
        GUI_Button(GUI_MakeBoxEx(GUI_GridAt(-1, 0), opt), "Game [X]", &icons->Dog);
        actions->reset_characters    = GUI_Button(GUI_MakeBoxEx(GUI_GridAt(-1,1), opt), "Reset", &icons->New);
        actions->add_character       = GUI_Button(GUI_MakeBoxEx(GUI_GridAt(-1,2), opt), "Add", &icons->Open);
        actions->toggle_character    = GUI_Button(GUI_MakeBoxEx(GUI_GridAt(-1,3), opt), "Change", &icons->Error);
    GUI_EndOverlay(GUI_GridBetween(-1, 1, -1, 3));
}

void GUI_FontSettingsMenu()
{
    GAME_WindowState *win_state = GAME_CTX.win_state;
    int selected_font = win_state->editor_font;
    Rectangle final_shape = { 0 };
    GUI_MenuItems items = GUI_GetFontMenuItems(&selected_font);

    GUI_BeginOverlay(true, true);
        GUI_SetFont(EGUI_Font_Default);
        if (GUI_ButtonMenuContents(-2, -1, 1, EGUI_ThemeColor_Green, &items, &final_shape)) {
            win_state->editor_font = (EGUI_Font)selected_font;
        }
    GUI_EndOverlay(final_shape);
}

void GUI_FontFilterMenu()
{
    GAME_WindowState *win_state     = GAME_CTX.win_state;
    GUI_Setup *setup                = GUI_GetSetup();
    EGUI_Font font_target           = win_state->editor_font;
    GUI_FontSetup *font_setup       = &setup->fonts[font_target];
    int selected_filter             = font_setup->texture_filter;
    Rectangle final_shape           = { 0 };
    GUI_MenuItems items             = GUI_GetTextureFilterMenuItems(&selected_filter);

    GUI_BeginOverlay(true, true);
        GUI_SetFont(EGUI_Font_Default);
        if (GUI_ButtonMenuContents(-2, -1, 1, EGUI_ThemeColor_Green, &items, &final_shape)) {
            font_setup->texture_filter = selected_filter;
            GUI_ApplyFontTextureFilter(font_target);
        }
    GUI_EndOverlay(final_shape);
}

void GUI_TopBar()
{
    GUI_Icons *icons    = GUI_GetIcons();
    Vector2 start       = RectPosition(GUI_GridAt(0,0));
    const int BUTTONS   = 4;
    static const int project_menu_owner = 0;
    static const int game_menu_owner = 0;
    static const int other_menu_owner = 0;
    GUI_SetThemeColors(EGUI_ThemeColor_Red);
    GUI_SetFont(EGUI_Font_GUI);
    GUI_GridForCols(BUTTONS, GUI_CalcDefaultHeightScaled(GUI_GetFont()));
    GUI_ButtonMenu(GUI_MakeBoxColor(GUI_GridNextX(), EGUI_ThemeColor_Red), &project_menu_owner, "Project", &icons->None, GUI_ProgramMenu);
    GUI_ButtonMenu(GUI_MakeBoxColor(GUI_GridNextX(), EGUI_ThemeColor_Abstractica), &game_menu_owner, "Game", &icons->Dog, GUI_GameMenu);
    GUI_ButtonMenu(GUI_MakeBoxColor(GUI_GridNextX(), EGUI_ThemeColor_Gray), &other_menu_owner, "Other", &icons->Dog, GUI_GameMenu);

    GUI_Face(GUI_MakePin(start, GUI_GridHeightOrDefault()));
    GUI_DrawOverlay();
}

//
// SAMPLE WINDOW
//

const char *GAME_GetFontLabel(EGUI_Font font)
{
    switch (font) {
    case EGUI_Font_GUI:
        return "GUI";
    case EGUI_Font_ShareTech:
        return "ShareTech";
    case EGUI_Font_Default:
    default:
        return "Default";
    }
}

// Define your draw window
void WIN_Window(GUI_Window* window)
{
    // Prepare your data
    GAME_WindowState *win_state = GAME_CTX.win_state;

    // Responsive height (if you require it)
    // GUI_WindowUpdateShapeForContent(window);

    GUI_Icons *icons            = GUI_GetIcons();
    static const int game_menu_2_owner = 0;
    // Keep or modify colors
    EGUI_ThemeColor colors      = window->colors;
    // Set your font
    EGUI_Font font     = win_state->demo_font;

    GUI_SetFont(font);
    // A default layout with 3 columns
    GUI_GridForCols(3, GUI_CalcDefaultHeightScaled(GUI_GetFont()));

    GUI_GridNextX();
    GUI_ButtonMenu(GUI_MakeBoxColor(GUI_GridNextX(), EGUI_ThemeColor_Abstractica), &game_menu_2_owner, "Game 2", &icons->Dog, GUI_GameMenu);

    GUI_GridForDuplicate();
    // 1st input (textbox)
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Text");
    GUI_Input(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), win_state->input_contents, win_state->input_contents, (int)sizeof(win_state->input_contents), EGUI_Input_Text);

    // 2nd input for integer
    // TODO@dc: add min, max and parsing
    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Int");
    GUI_Input(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), win_state->input_int_contents, win_state->input_int_contents, (int)sizeof(win_state->input_int_contents), EGUI_Input_Int);

    // 3rd input for float
    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Float");
    GUI_Input(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), win_state->input_float_contents, win_state->input_float_contents, (int)sizeof(win_state->input_float_contents), EGUI_Input_Float);
}

void WIN_Layouts(GUI_Window* window)
{
    EGUI_Font font = EGUI_Font_Default;
    float default_height = GUI_CalcDefaultHeightScaled(font);

    Rectangle window_workspace = window->workspace;
    GUI_SetFont(EGUI_Font_Default);

    // First block
    GUI_GridForXY(window_workspace.width, default_height, 0);
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextY(), EGUI_ThemeColor_Gray), "Some sample layouts for imKairos");

    float color_alpha = 0.9f;
    GUI_BeginControlScissor();
        DrawDebugRect(GUI_GridNextY(), ColorAlpha(BROWN, color_alpha));
        DrawDebugRect(GUI_GridNextY(), ColorAlpha(BEIGE, color_alpha));

        // 1/3rd and 2/3rds blocks
        GUI_GridForXY(window_workspace.width / 3, default_height, 0);
        DrawDebugRect(GUI_GridNextX(), ColorAlpha(YELLOW, color_alpha));
        DrawDebugRect(GUI_GridNextXn(2), ColorAlpha(GREEN, color_alpha));

        // Second block
        // 3 horizontals of 1/3 of the available space
        GUI_GridForXY(window_workspace.width / 3, default_height, 0);
        DrawDebugRect(GUI_GridNextX(), ColorAlpha(DARKGRAY, color_alpha));
        DrawDebugRect(GUI_GridNextX(), ColorAlpha(GRAY, color_alpha));
        DrawDebugRect(GUI_GridNextX(), ColorAlpha(LIGHTGRAY, color_alpha));

        // Prepare for a new block with 5 elements per row
        // You can send negative values to use AVAILABLE - YOUR_VALUE
        // Ex:
        // -200 means take all space but at least 200
        GUI_GridForXY(window_workspace.width / 5, -200, default_height);
        DrawDebugRect(GUI_GridNextX(), BLACK);
        DrawDebugRect(GUI_GridNextX(), BLACK);
        DrawDebugRect(GUI_GridNextX(), BLACK);
        DrawDebugRect(GUI_GridNextX(), BLACK);
        DrawDebugRect(GUI_GridNextX(), BLACK);

        // Final row
        GUI_GridForXY(window_workspace.width / 2, default_height, 0);
        DrawDebugRect(GUI_GridNextX(), ColorAlpha(RED, color_alpha));
        DrawDebugRect(GUI_GridNextX(), ColorAlpha(BLUE, color_alpha));
    EndScissorMode();

}


void WIN_CharacterDebug(GUI_Window* window)
{
    GAME_State *game_state      = GAME_CTX.state;
    GAME_Character *ch          = &game_state->characters[game_state->current_character];
    EGUI_ThemeColor colors      = window->colors;

    GUI_GridForCols(2, GUI_CalcDefaultHeightScaled(GUI_GetFont()));

    // Shape
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "shape.x");
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), TextFormat("%.2f", ch->shape.x));

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "shape.y");
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), TextFormat("%.2f", ch->shape.y));

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "shape.w");
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), TextFormat("%.2f", ch->shape.width));

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "shape.h");
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), TextFormat("%.2f", ch->shape.height));

    // Color (RGB)
    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "color");
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), TextFormat("r:%d g:%d b:%d a:%d",
        ch->color.r, ch->color.g, ch->color.b, ch->color.a));

    // Movement
    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "movement.x");
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), TextFormat("%.3f", ch->movement.x));

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "movement.y");
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), TextFormat("%.3f", ch->movement.y));

    // Animation time
    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "anim_time");
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), TextFormat("%.3f", ch->anim_time));
}

// Define your draw window
void WIN_Settings(GUI_Window* window)
{
    GAME_WindowState *win_state = GAME_CTX.win_state;
    GUI_State *state            = GUI_GetState();
    EGUI_ThemeColor colors      = window->colors;
    EGUI_Font font             = win_state->demo_font;

    GUI_SetFont(font);
    GUI_GridForCols(3, GUI_CalcDefaultHeightScaled(GUI_GetFont()));
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Scale");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), &state->scale, 0.5f, 6.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Scale 2");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), &state->scale, 0.5f, 6.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Wallpaper");
    GUI_Check(GUI_MakeBoxColor(GUI_GridNextXn(2), EGUI_ThemeColor_Red), &win_state->checkbox_value, "ON", "OFF");

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Font");
    if (GUI_Button(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), GAME_GetFontLabel(win_state->demo_font), NULL)) {
        win_state->demo_font = (EGUI_Font)((win_state->demo_font + 1) % EGUI_Font_Count);
    }
}

void WIN_FontSettings(GUI_Window* window)
{
    GAME_WindowState *win_state = GAME_CTX.win_state;
    GUI_Setup *setup            = GUI_GetSetup();
    EGUI_ThemeColor colors      = window->colors;
    EGUI_Font font_target       = win_state->editor_font;
    GUI_MenuItems font_items    = GUI_GetFontMenuItems((int *)&font_target);
    const GUI_MenuItem *item    = GUI_MenuItemGetSelected(&font_items);
    GUI_FontSetup *font_setup   = &setup->fonts[font_target];

    GUI_SetFont(EGUI_Font_Default);
    GUI_GridForCols(3, GUI_CalcDefaultHeightScaled(GUI_GetFont()));

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Editing");
    GUI_ButtonMenu(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), &win_state->editor_font, item->label, GUI_GetIconTexture(item->icon), GUI_FontSettingsMenu);
    font_target = win_state->editor_font;
    font_setup = &setup->fonts[font_target];

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "default_h");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), &font_setup->default_height, 8.0f, 128.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "border");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), &font_setup->border, 0.0f, 16.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "scale");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), &font_setup->scale, 0.1f, 8.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "spacing");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), &font_setup->spacing, -8.0f, 32.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "delta.x");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextX(), colors), &font_setup->delta.x, -64.0f, 64.0f);
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextX(), colors), &font_setup->delta.y, -64.0f, 64.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "blink size");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextX(), colors), &font_setup->blink_size.x, 0.0f, 32.0f);
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextX(), colors), &font_setup->blink_size.y, 0.0f, 128.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "blink delta");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextX(), colors), &font_setup->blink_delta.x, -64.0f, 64.0f);
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextX(), colors), &font_setup->blink_delta.y, -64.0f, 64.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "blink alpha");
    GUI_Float(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), &font_setup->blink_alpha, 0.0f, 1.0f);

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "filter");
    {
        int selected_filter = font_setup->texture_filter;
        GUI_MenuItems filter_items = GUI_GetTextureFilterMenuItems(&selected_filter);
        const GUI_MenuItem *filter_item = GUI_MenuItemGetSelected(&filter_items);
        GUI_ButtonMenu(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), &font_setup->texture_filter, filter_item->label, GUI_GetIconTexture(filter_item->icon), GUI_FontFilterMenu);
    }
    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextXn(3), colors), "combine filtering with non-integer scaling");

    bool use_atlas = font_setup->atlas_reload_size > 0
        || font_setup->atlas.ready
        || font_setup->atlas.texture.id != 0;

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "mode");
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), use_atlas ? "atlas" : "plain");
    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "reload");
    if (use_atlas) {
        float atlas_reload_size = (float)font_setup->atlas_reload_size;
        GUI_Float(GUI_MakeBoxColor(GUI_GridNextX(), colors), &atlas_reload_size, 8.0f, 128.0f);
        font_setup->atlas_reload_size = (int)roundf(atlas_reload_size);
    } else {
        GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "-");
    }
    if (GUI_Button(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Reload", NULL)) {
        GUI_ReloadFontSetupAsset(font_target);
    }

    if (use_atlas) {
        GUI_GridForDuplicate();
        GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "atlas");
        GUI_Text(GUI_MakeBoxColor(GUI_GridNextXn(2), colors),
            TextFormat("%dx%d px:%d ready:%d",
                font_setup->atlas.atlas_width,
                font_setup->atlas.atlas_height,
                font_setup->atlas.pixel_size,
                font_setup->atlas.ready));
    }

    GUI_GridForDuplicate();
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextX(), colors), "Preview");
    GUI_SetFont(font_target);
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextXn(2), colors), "Scale 123 Font");
}

void WIN_Logo(GUI_Window* window)
{
    static Texture2D logo = {0};
    if (logo.id == 0) logo = LoadTexture(BROKY_AI_ROOT "/logo.png");

    EGUI_Font font          = GUI_GetFont();
    float default_height    = GUI_CalcDefaultHeightScaled(font);
    Rectangle workspace     = window->workspace;

    // Label
    GUI_GridForXY(workspace.width, default_height, 0);
    GUI_Text(GUI_MakeBoxColor(GUI_GridNextY(), window->colors), "Broky logo");
    // Image
    Rectangle next = GUI_GridNextY();
    GUI_Image(logo, (Rectangle){ next.x, next.y, next.width, 320 });
}

void WIN_Winman(GUI_Window* window)
{
    GUI_State *state = GUI_GetState();
    GUI_Icons *icons = GUI_GetIcons();
    EGUI_Font font = EGUI_Font_Default;
    float default_height = GUI_CalcDefaultHeightScaled(font);

    Rectangle window_workspace = window->workspace;

    GUI_GridForXY(window_workspace.width, default_height, 0);

    static GUI_Window* win_window = NULL;
    if (GUI_Button(GUI_MakeBox(GUI_GridNextY()), "Sample window", NULL)) {
        int win_id = 2;
        if (win_window == NULL || win_window->id == 0) {
            win_window = GUI_OpenWindow(win_id, "Sample window", EGUI_ThemeColor_Abstractica, &icons->Dog, false, WIN_Window);
        }
        GUI_ForceZindex(win_id);
    }
    static GUI_Window* win_layouts = NULL;
    if (GUI_Button(GUI_MakeBox(GUI_GridNextY()), "Layouts window", NULL)) {
        int win_id = 3;
        if (win_layouts == NULL || win_layouts->id == 0) {
            win_layouts = GUI_OpenWindow(win_id, "Layouts window", EGUI_ThemeColor_Gray, &icons->Layouts, false, WIN_Layouts);
        }
        GUI_ForceZindex(win_id);
    }
    static GUI_Window* win_character_debug = NULL;
    if (GUI_Button(GUI_MakeBox(GUI_GridNextY()), "Character debug", NULL)) {
        int win_id = 4;
        if (win_character_debug == NULL || win_character_debug->id == 0) {
            win_character_debug = GUI_OpenWindow(win_id, "Character debug", EGUI_ThemeColor_Gray, &icons->Dog, false, WIN_CharacterDebug);
        }
        GUI_ForceZindex(win_id);
    }
    static GUI_Window* win_settings = NULL;
    if (GUI_Button(GUI_MakeBox(GUI_GridNextY()), "Settings", &icons->Setup)) {
        int win_id = 5;
        if (win_settings == NULL || win_settings->id == 0) {
            win_settings = GUI_OpenWindow(win_id, "Settings", EGUI_ThemeColor_Gray, &icons->Setup, true, WIN_Settings);
        }
        GUI_ForceZindex(win_id);
    }
    static GUI_Window* win_font_settings = NULL;
    if (GUI_Button(GUI_MakeBox(GUI_GridNextY()), "Font settings", &icons->Setup)) {
        int win_id = 7;
        if (win_font_settings == NULL || win_font_settings->id == 0) {
            win_font_settings = GUI_OpenWindow(win_id, "Font settings", EGUI_ThemeColor_Gray, &icons->Setup, true, WIN_FontSettings);
            win_font_settings->shape.width = 520.0f;
            win_font_settings->shape.height = 420.0f;
        }
        GUI_ForceZindex(win_id);
    }
    static GUI_Window* win_logo = NULL;
    if (GUI_Button(GUI_MakeBox(GUI_GridNextY()), "Logo window", &icons->Dog)) {
        int win_id = 6;
        if (win_logo == NULL || win_logo->id == 0) {
            win_logo = GUI_OpenWindow(win_id, "Logo", EGUI_ThemeColor_Abstractica, &icons->Dog, false, WIN_Logo);
            win_logo->shape.width = 520.0f;
            win_logo->shape.height = 420.0f;
        }
        GUI_ForceZindex(win_id);
    }


    GUI_Text(GUI_MakeBox(GUI_GridNextY()), "--- Opened windows ---");
    GUI_GridForDuplicate();
    for (int i = 0; i < GUI_MAX_OPEN_WINS; ++i) {
        GUI_Window* win = &state->window_s[i];
        if (win->id == 0 || window->id == win->id) continue;
        if (GUI_Button(GUI_MakeBox(GUI_GridNextY()), TextFormat("%d - %s", win->id, win->title), NULL)) {
            GUI_ForceZindex(win->id);
        }
    }

    GUI_Text(GUI_MakeBox(GUI_GridNextY()), "--- Global values ---");

    // NOTE:
    // GUI_IsPointerOverGui() is not safe to be called by a Window. It requires ALL windows to be processed beforehand.
    // GUI_Text(GUI_NextVertical(), TextFormat("PointerOverGUI: %d", GUI_IsPointerOverGui()),  window->colors);

    GUI_Window *active = GUI_GetWindowByZindex(0);
    if (active != NULL) {
        Rectangle t_shape = GUI_GetWindowTitle(active->shape);
        Rectangle w_shape = active->shape;

        GUI_Text(GUI_MakeBox(GUI_GridNextY()), "--- Focused window ---");
        GUI_Text(GUI_MakeBox(GUI_GridNextY()), TextFormat("ID=%d scroll=%.2f content_height=%.2f", active->id, active->scroll_offset, active->content_height));

        GUI_Text(GUI_MakeBox(GUI_GridNextY()), "title_shape");
        GUI_Text(GUI_MakeBox(GUI_GridNextY()), TextFormat("x=%.2f  y=%.2f w=%.2f  h=%.2f", t_shape.y,  t_shape.x, t_shape.width, t_shape.height));

        GUI_Text(GUI_MakeBox(GUI_GridNextY()), "window_shape");
        GUI_Text(GUI_MakeBox(GUI_GridNextY()), TextFormat("x=%.2f  y=%.2f w=%.2f  h=%.2f", w_shape.x, w_shape.y, w_shape.width, w_shape.height));
        GUI_Text(GUI_MakeBox(GUI_GridNextY()), "--- End focused window ---");
    }

    Rectangle next = GUI_GridNextY();
    float icon_w = GUI_GetIconWidth();
    GUI_Face(GUI_MakePin((Vector2){ next.x, next.y }, (float) icon_w / 2));
    GUI_Face(GUI_MakePin((Vector2){ next.x + (float) icon_w / 2, next.y }, icon_w));
    GUI_Face(GUI_MakePin((Vector2){ next.x + (float) icon_w / 2 + icon_w, next.y }, icon_w * 2));
    GUI_Icon(&icons->Dog, GUI_MakePin((Vector2){ next.x + (float) icon_w / 2 + icon_w * 3, next.y }, icon_w * 2), WHITE);
    GUI_GridNextY(); // Jump line
    GUI_GridNextY(); // Jump line

    static Texture2D image;
    if (image.id == 0) image = LoadTexture(BROKY_AI_ROOT "/abstractica.png");
    next = GUI_GridNextY();
    GUI_Image(image, (Rectangle){ next.x, next.y, next.width, 320 });
}

#endif
