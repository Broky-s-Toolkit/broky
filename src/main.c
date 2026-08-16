#define IDE_SYNTAX_HL 0
#define IMPLEMENT_ALL 1
#define STB_TRUETYPE_IMPLEMENTATION
#include "3rd/stb_truetype.h"
#include "common.h"
#include "gui/gui.h"
#include "game/main.h"
#include "labs/lab.h"
#include "labs/sprites-paralax.h"

#define GAME_RES_W          320
#define GAME_RES_H          240
#define GAME_RES_HALF_W     160
#define GAME_RES_HALF_H     120


int main(void) {
    InitArtWorkingDirectory();

    if (DEV_FULLSCREEN == false)
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(DEV_WINDOW_W, DEV_WINDOW_H, TextFormat("%s - %s - %s", BuildTimeFormatted(), __DATE__, GetWorkingDirectory()));
    SetTargetFPS(60);

    TrySetTargetMonitor(DEV_TARGET_MONITOR, 256);

    // TODO@dc: review
    Vector2 screen_max = (Vector2) { GetMonitorWidth(DEV_TARGET_MONITOR),  GetMonitorHeight(DEV_TARGET_MONITOR) };
    SetWindowMaxSize(screen_max.x, screen_max.y);

    if (DEV_FULLSCREEN)
        ToggleFullscreen();

    if (DEV_MAXIMIZE)
        MaximizeWindow();

    if (DEV_HIDE_CURSOR)
        HideCursor();

    // PREPARE GUI
    static GUI_State gui_state;
    static GUI_Setup gui_setup;
    static GUI_Temp  gui_temp;
    static GUI_Icons icons;
    static Texture2D wp_voronoi;

    gui_state   = GUI_MakeStateDefault(screen_max);
    gui_setup   = GUI_LoadSetupDefault();
    gui_temp    = GUI_MakeTempDefault();
    icons       = gui_setup.icons.icons;
    wp_voronoi  = GenerateVoronoiTexture((int)screen_max.x, (int)screen_max.y);
    GUI_SetContext(&gui_state, &gui_setup, &gui_temp);

    // PREPARE GAME
    static GAME_State game_state;
    static GAME_WindowState win_state;
    static GAME_Temp game_temp;

    game_state  = GAME_MakeState();
    win_state   = GAME_MakeWindowState();
    game_temp   = GAME_MakeTemp();
    GAME_InitPhysics(&game_state);
    GAME_SetContext(&game_state, &win_state, &game_temp);

    SpritesParallaxLab sprites_parallax = SpritesParallaxLoad();

    // Game canvas
    RenderTexture2D game_canvas = LoadRenderTexture(GAME_RES_W, GAME_RES_H);
    SetTextureFilter(game_canvas.texture, TEXTURE_FILTER_POINT);

    SetTargetFPS(60);
    WaitForWindowSizeStable(2, 256);
    while (!WindowShouldClose()) {
        // RESET TEMP VALUES ONLY 1-FRAME
        GAME_CTX.temp->player_actions   = GAME_MakeActions();

        //
        // UPDATE
        //

        //
        // UI
        static EGUI_Cursor pointer_style   = EGUI_Cursor_Default;
        if (IsKeyPressed(KEY_F10)) {
            pointer_style = pointer_style == EGUI_Cursor_Default ? EGUI_Cursor_AGS : EGUI_Cursor_Default;
        }

        float topbar_height = GUI_CalcDefaultHeightScaled(EGUI_Font_GUI);
        Rectangle window_limits = (Rectangle){
            0,
            topbar_height,
            (float) GetScreenWidth(),
            (float) GetScreenHeight() - topbar_height
        };

        // Draw UI buffer

        BeginTextureMode(gui_state.buffer);
        GUI_BeginDraw(pointer_style);
            ClearBackground(BLANK);
            // Win-manager
            {
                static GUI_Window* win_man = NULL;
                if (win_man == NULL) {
                    win_man = GUI_OpenWindow(1, "WinMan", EGUI_ThemeColor_Gray, &icons.Setup, true, WIN_Winman);
                } else {
                    float win_forth          = window_limits.width / 4.0f;
                    win_man->shape.x         = win_forth * 3;
                    win_man->shape.y         = window_limits.y;
                    win_man->shape.width     = win_forth;
                    win_man->shape.height    = window_limits.height;
                }
            }
            GUI_UpdateAndDrawWindows(window_limits);

            GUI_ResetStyleDefaults();
            GUI_TopBar((Rectangle){ 0, 0, (float) GetScreenWidth(), topbar_height });
        GUI_EndDraw();
        EndTextureMode();

        // GUI Actions
        GAME_Actions *player_actions = &GAME_CTX.temp->player_actions;
        player_actions->toggle_character     |= IsKeyPressed(KEY_TAB);
        player_actions->move_down             = IsKeyDown(KEY_DOWN);
        player_actions->move_up               = IsKeyDown(KEY_UP);
        player_actions->move_left             = IsKeyDown(KEY_LEFT);
        player_actions->move_right            = IsKeyDown(KEY_RIGHT);

        if (player_actions->reset_characters) {
            GAME_DestroyPhysics(&game_state);
            game_state = GAME_MakeState();
            GAME_InitPhysics(&game_state);
        }
        if (player_actions->add_character)      GAME_AddCharacter();
        if (player_actions->toggle_character)   GAME_UpdateNextCharacter();
        if (IsKeyPressed(KEY_F8))               gui_state.scale = FloatMin(6.0f, gui_state.scale + 0.25f);
        if (IsKeyPressed(KEY_F7))               gui_state.scale = FloatMax(0.5f, gui_state.scale - 0.25f);

        //
        // CHARACTERS
        GAME_Character *player = GAME_GetCurrentCharacter();
        Camera2D *camera = &game_state.camera2D;
        camera->offset = (Vector2){ GAME_RES_HALF_W,  GAME_RES_HALF_H };

        if (GUI_IsCursorOverGui() == false) {
            // Update camera
            camera->zoom += GetMouseWheelMove() * 0.1f;
            if (camera->zoom > 3.0f) camera->zoom = 3.0f;
            else if (camera->zoom < 0.1f) camera->zoom = 0.1f;
        }

        for (int i = 0; i < game_state.alive_characters; ++i) {
            game_state.characters[i].movement = Vector2Zero();
        }

        Vector2 move = { 0.0f, 0.0f };
        if (player_actions->move_down)  move.y += 1;
        if (player_actions->move_up)    move.y -= 1;
        if (player_actions->move_left)  move.x -= 1;
        if (player_actions->move_right) move.x += 1;

        float dt = GetFrameTime();
        SpritesParallaxUpdate(&sprites_parallax, dt);
        player->movement    = move;
        GAME_StepPhysics(&game_state, dt);
        camera->target = RectCenter(player->shape);

        // Animate
        float speed = Vector2Length(player->movement);
        if (speed > 0.01f)
            player->anim_time   += dt * speed * 2;
        else
            player->anim_time   = 0;


        //
        // RENDER
        //
        BeginTextureMode(game_canvas);
            ClearBackground(BLANK);

            // Game world
            BeginMode2D(*camera);
                SpritesParallaxDraw(&sprites_parallax, camera);

                bool collisions[CHARACTERS];
                float radius = 30.0f;
                GAME_UpdateCollisions(collisions, radius);

                for (int i = 0; i < game_state.alive_characters; ++i) {
                    GAME_Character *character   = &game_state.characters[i];
                    Vector2 center              = RectCenter(character->shape);
                    Color ring_color            = collisions[i] ? ColorAlpha(WHITE, 0.2)
                                                                : ColorAlpha(WHITE, 0);
                    Color box_color             = collisions[i] ? RED : BLACK;
                    float anim_phase            = character->anim_time * (character->movement.x < 0 ? 1.0f : -1.0f);
                    DrawRing(center, radius - 3, radius, 0, 360, 32, ring_color);
                    DrawCharacter(character->shape, character->movement, anim_phase, character->color);
                    DrawRectangleLinesEx(character->shape, 1.0f, box_color);
                }
            EndMode2D();
        EndTextureMode();


        // FXs
        static RenderTexture2D rain_buffer  = { 0 };
        if (rain_buffer.id == 0) {
            rain_buffer = LoadRenderTexture((int)(GetScreenWidth() / 5), (int)(GetScreenHeight() / 5 + 100));
        }
        BeginTextureMode(rain_buffer);
            ClearBackground(BLANK);
            DrawRain(rain_buffer.texture.width, rain_buffer.texture.height, 2.5f, ColorAlpha(BLUE, 0.4f));
        EndTextureMode();

        // Draw
        BeginDrawing();
            ClearBackground(WHITE);

            // Wallpaper
            if (win_state.checkbox_value == 1) {
                DrawTextureRec(wp_voronoi, GetSourceRec(wp_voronoi), (Vector2){ 0, 0 }, gui_setup.theme.colors[EGUI_ThemeColor_Gray].bg_color_3);
                /*DrawTexturePro(rain_buffer.texture, GetSourceRec(rain_buffer.texture), MoveAndExtendXY(window_limits, 0, 100), (Vector2){0,0}, 0.0, WHITE);*/
            }

            DrawTexturePro(game_canvas.texture,
                (Rectangle){ 0, 0, GAME_RES_W, -GAME_RES_H },
                (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );

            // Show UI Buffer
            DrawTextureRec(gui_state.buffer.texture, FlipYRec(GetSourceRec(gui_state.buffer.texture)), (Vector2){ 0, 0 }, WHITE);

            // Finally, draw the pointer so it appears in the top of anything else
            GUI_DrawCursorTrail();
            GUI_DrawCursor();
            #if DEV_DEBUG_GUI_STATE
            if (GUI_IsCursorOverGui()) DrawText("Cursor over GUI", 0, topbar_height, 32, GREEN);
            if (GUI_IsCursorOverOverlay()) DrawText("Cursor over OVERLAY", 0, topbar_height, 32, RED);
            #endif
        EndDrawing();
    }

    GAME_DestroyPhysics(&game_state);
    SpritesParallaxUnload(&sprites_parallax);
    CloseWindow();
    return 0;
}
