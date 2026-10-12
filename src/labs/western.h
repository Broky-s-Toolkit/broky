#pragma once

#ifndef NON_EDITOR_BUILD
 #define NON_EDITOR_BUILD 0
 #include "../common.h"
#endif

#define WESTERN_ASSET_ROOT BROKY_ART_ROOT "/spr/western"
#define WESTERN_MAX_ENEMIES 24
#define WESTERN_MAX_BULLETS 96
#define WESTERN_AMMO 6
#define WESTERN_PLAYER_RADIUS 5.0f
#define WESTERN_ENEMY_RADIUS 7.0f
#define WESTERN_PLAYER_BULLET_RADIUS 1.5f
#define WESTERN_ENEMY_BULLET_RADIUS 2.0f

typedef struct {
    bool active;
    bool hostile;
    Vector2 position;
    Vector2 velocity;
    float life;
} WesternBullet;

typedef struct {
    bool active;
    Vector2 position;
    float fire_timer;
    float speed;
    int variant;
} WesternEnemy;

typedef struct {
    Texture2D player;
    Texture2D enemy_gunman;
    Texture2D enemy_bandito;
    Texture2D revolver;
    Vector2 player_position;
    Vector2 aim;
    WesternEnemy enemies[WESTERN_MAX_ENEMIES];
    WesternBullet bullets[WESTERN_MAX_BULLETS];
    float spawn_timer;
    float shot_cooldown;
    float reload_timer;
    float muzzle_flash;
    bool trigger_was_down;
    bool dead;
    int ammo;
    int score;
} WesternLab;

static float WesternRandom(float min, float max)
{
    return min + (max - min) * ((float)GetRandomValue(0, 10000) / 10000.0f);
}

static void WesternReset(WesternLab *lab)
{
    lab->player_position = (Vector2){ 160.0f, 120.0f };
    lab->aim = (Vector2){ 1.0f, 0.0f };
    lab->spawn_timer = 0.2f;
    lab->shot_cooldown = 0.0f;
    lab->reload_timer = 0.0f;
    lab->muzzle_flash = 0.0f;
    lab->trigger_was_down = false;
    lab->dead = false;
    lab->ammo = WESTERN_AMMO;
    lab->score = 0;
    memset(lab->enemies, 0, sizeof(lab->enemies));
    memset(lab->bullets, 0, sizeof(lab->bullets));
}

static WesternLab WesternLoad(void)
{
    WesternLab lab = {
        .player        = LoadTexture(WESTERN_ASSET_ROOT "/player-portrait.png"),
        .enemy_gunman  = LoadTexture(WESTERN_ASSET_ROOT "/enemy-gunman.png"),
        .enemy_bandito = LoadTexture(WESTERN_ASSET_ROOT "/enemy-bandito.png"),
        .revolver      = LoadTexture(WESTERN_ASSET_ROOT "/revolver-ui.png")
    };
    SetTextureFilter(lab.player, TEXTURE_FILTER_POINT);
    SetTextureFilter(lab.enemy_gunman, TEXTURE_FILTER_POINT);
    SetTextureFilter(lab.enemy_bandito, TEXTURE_FILTER_POINT);
    SetTextureFilter(lab.revolver, TEXTURE_FILTER_POINT);
    WesternReset(&lab);
    return lab;
}

static void WesternUnload(WesternLab *lab)
{
    UnloadTexture(lab->player);
    UnloadTexture(lab->enemy_gunman);
    UnloadTexture(lab->enemy_bandito);
    UnloadTexture(lab->revolver);
    *lab = (WesternLab){ 0 };
}

static void WesternAddBullet(WesternLab *lab, Vector2 position, Vector2 direction,
                             float speed, bool hostile)
{
    for (int i = 0; i < WESTERN_MAX_BULLETS; ++i) {
        WesternBullet *bullet = &lab->bullets[i];
        if (bullet->active) continue;
        *bullet = (WesternBullet){
            .active = true,
            .hostile = hostile,
            .position = position,
            .velocity = Vector2Scale(direction, speed),
            .life = 2.5f
        };
        return;
    }
}

static void WesternSpawnEnemy(WesternLab *lab)
{
    for (int i = 0; i < WESTERN_MAX_ENEMIES; ++i) {
        if (lab->enemies[i].active) continue;

        int edge = GetRandomValue(0, 3);
        Vector2 position = { 0 };
        if (edge == 0) position = (Vector2){ WesternRandom(14, 306), 12 };
        if (edge == 1) position = (Vector2){ 308, WesternRandom(18, 222) };
        if (edge == 2) position = (Vector2){ WesternRandom(14, 306), 228 };
        if (edge == 3) position = (Vector2){ 12, WesternRandom(18, 222) };

        lab->enemies[i] = (WesternEnemy){
            .active = true,
            .position = position,
            .fire_timer = WesternRandom(0.65f, 1.6f),
            .speed = WesternRandom(19.0f, 29.0f),
            .variant = GetRandomValue(0, 1)
        };
        return;
    }
}

static Vector2 WesternMouseInCanvas(void)
{
    Vector2 mouse = GetMousePosition();
    float width = (float)GetScreenWidth();
    float height = (float)GetScreenHeight();
    if (width <= 0.0f || height <= 0.0f) return (Vector2){ 160, 120 };
    return (Vector2){ mouse.x * 320.0f / width, mouse.y * 240.0f / height };
}

static void WesternUpdate(WesternLab *lab, float dt)
{
    dt = FloatMin(dt, 0.05f);
    bool gamepad = IsGamepadAvailable(0);

    if (lab->dead) {
        bool restart = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
        restart |= gamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
        if (restart) WesternReset(lab);
        return;
    }

    Vector2 move = {
        (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) - (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)),
        (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) - (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    };
    if (gamepad) {
        Vector2 stick = {
            GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X),
            GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y)
        };
        if (Vector2Length(stick) > 0.18f) move = stick;
    }
    if (Vector2Length(move) > 1.0f) move = Vector2Normalize(move);
    lab->player_position = Vector2Add(lab->player_position, Vector2Scale(move, 72.0f * dt));
    lab->player_position.x = Clamp(lab->player_position.x, 10.0f, 310.0f);
    lab->player_position.y = Clamp(lab->player_position.y, 18.0f, 226.0f);

    Vector2 mouse_aim = Vector2Subtract(WesternMouseInCanvas(), lab->player_position);
    if (Vector2Length(mouse_aim) > 2.0f) lab->aim = Vector2Normalize(mouse_aim);

    float trigger = -1.0f;
    if (gamepad) {
        Vector2 stick_aim = {
            GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X),
            GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y)
        };
        if (Vector2Length(stick_aim) > 0.25f) lab->aim = Vector2Normalize(stick_aim);
        trigger = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER);
    }

    bool trigger_down = gamepad && trigger > 0.15f;
    bool shoot = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    shoot |= gamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2);
    shoot |= trigger_down && !lab->trigger_was_down;
    lab->trigger_was_down = trigger_down;

    lab->shot_cooldown = FloatMax(0.0f, lab->shot_cooldown - dt);
    lab->muzzle_flash = FloatMax(0.0f, lab->muzzle_flash - dt);
    if (lab->reload_timer > 0.0f) {
        lab->reload_timer -= dt;
        if (lab->reload_timer <= 0.0f) lab->ammo = WESTERN_AMMO;
    }

    bool reload = IsKeyPressed(KEY_R);
    reload |= gamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
    if (reload && lab->ammo < WESTERN_AMMO && lab->reload_timer <= 0.0f) lab->reload_timer = 0.82f;

    if (shoot && lab->reload_timer <= 0.0f && lab->shot_cooldown <= 0.0f) {
        if (lab->ammo > 0) {
            Vector2 muzzle = Vector2Add(lab->player_position, Vector2Scale(lab->aim, 10.0f));
            WesternAddBullet(lab, muzzle, lab->aim, 330.0f, false);
            lab->ammo--;
            lab->shot_cooldown = 0.16f;
            lab->muzzle_flash = 0.07f;
        } else {
            lab->reload_timer = 0.82f;
        }
    }

    lab->spawn_timer -= dt;
    if (lab->spawn_timer <= 0.0f) {
        WesternSpawnEnemy(lab);
        lab->spawn_timer = FloatMax(0.42f, 1.3f - lab->score * 0.018f);
    }

    for (int i = 0; i < WESTERN_MAX_ENEMIES; ++i) {
        WesternEnemy *enemy = &lab->enemies[i];
        if (!enemy->active) continue;
        Vector2 delta = Vector2Subtract(lab->player_position, enemy->position);
        float distance = Vector2Length(delta);
        Vector2 direction = distance > 0.01f ? Vector2Scale(delta, 1.0f / distance) : Vector2Zero();

        if (distance > 58.0f) {
            enemy->position = Vector2Add(enemy->position, Vector2Scale(direction, enemy->speed * dt));
        }
        if (distance < WESTERN_PLAYER_RADIUS + WESTERN_ENEMY_RADIUS) lab->dead = true;

        enemy->fire_timer -= dt;
        if (enemy->fire_timer <= 0.0f && distance < 175.0f) {
            WesternAddBullet(lab, Vector2Add(enemy->position, Vector2Scale(direction, 6.0f)),
                             direction, 74.0f, true);
            enemy->fire_timer = WesternRandom(1.15f, 2.05f);
        }
    }

    for (int b = 0; b < WESTERN_MAX_BULLETS; ++b) {
        WesternBullet *bullet = &lab->bullets[b];
        if (!bullet->active) continue;
        bullet->position = Vector2Add(bullet->position, Vector2Scale(bullet->velocity, dt));
        bullet->life -= dt;
        if (bullet->life <= 0.0f || bullet->position.x < -4 || bullet->position.x > 324 ||
            bullet->position.y < -4 || bullet->position.y > 244) {
            bullet->active = false;
            continue;
        }

        if (bullet->hostile) {
            if (Vector2Distance(bullet->position, lab->player_position) <
                WESTERN_PLAYER_RADIUS + WESTERN_ENEMY_BULLET_RADIUS) {
                bullet->active = false;
                lab->dead = true;
            }
            continue;
        }

        for (int e = 0; e < WESTERN_MAX_ENEMIES; ++e) {
            WesternEnemy *enemy = &lab->enemies[e];
            if (enemy->active && Vector2Distance(bullet->position, enemy->position) <
                WESTERN_ENEMY_RADIUS + WESTERN_PLAYER_BULLET_RADIUS) {
                bullet->active = false;
                enemy->active = false;
                lab->score++;
                break;
            }
        }
    }
}

static void WesternDrawSprite(Texture2D texture, Vector2 position, float width, float height,
                              bool flip, Color tint)
{
    Rectangle source = { 0, 0, flip ? -(float)texture.width : (float)texture.width, (float)texture.height };
    Rectangle destination = { position.x, position.y, width, height };
    DrawTexturePro(texture, source, destination, (Vector2){ width * 0.5f, height * 0.82f }, 0, tint);
}

static void WesternDraw(const WesternLab *lab, int width, int height)
{
    (void)width;
    (void)height;
    ClearBackground(RAYWHITE);

    for (int i = 0; i < WESTERN_MAX_ENEMIES; ++i) {
        const WesternEnemy *enemy = &lab->enemies[i];
        if (!enemy->active) continue;
        bool flip = lab->player_position.x < enemy->position.x;
        DrawEllipse((int)enemy->position.x, (int)enemy->position.y + 2, 7, 3, Fade(BLACK, 0.13f));
        Texture2D sprite = enemy->variant ? lab->enemy_bandito : lab->enemy_gunman;
        float sprite_height = enemy->variant ? 25.0f : 23.0f;
        WesternDrawSprite(sprite, enemy->position, 18.0f, sprite_height, flip, WHITE);

        DrawCircleLinesV(enemy->position, WESTERN_ENEMY_RADIUS, (Color){ 220, 55, 45, 230 });
        DrawCircleV(enemy->position, 1.0f, (Color){ 220, 55, 45, 230 });

        Vector2 aim = Vector2Normalize(Vector2Subtract(lab->player_position, enemy->position));
        DrawLineEx(enemy->position, Vector2Add(enemy->position, Vector2Scale(aim, 7.0f)), 1.5f,
                   (Color){ 63, 40, 35, 255 });
    }

    for (int i = 0; i < WESTERN_MAX_BULLETS; ++i) {
        const WesternBullet *bullet = &lab->bullets[i];
        if (!bullet->active) continue;
        Color color = bullet->hostile ? (Color){ 169, 41, 35, 255 } : (Color){ 56, 43, 37, 255 };
        float bullet_radius = bullet->hostile ? WESTERN_ENEMY_BULLET_RADIUS
                                              : WESTERN_PLAYER_BULLET_RADIUS;
        DrawCircleV(bullet->position, bullet_radius, color);
        DrawCircleLinesV(bullet->position, bullet_radius + 0.75f,
                         bullet->hostile ? (Color){ 255, 45, 35, 245 }
                                         : (Color){ 35, 105, 225, 245 });
        Vector2 trail = Vector2Subtract(bullet->position, Vector2Scale(Vector2Normalize(bullet->velocity), 4.0f));
        DrawLineV(trail, bullet->position, Fade(color, 0.55f));
    }

    DrawEllipse((int)lab->player_position.x, (int)lab->player_position.y + 3, 8, 3, Fade(BLACK, 0.15f));
    WesternDrawSprite(lab->player, lab->player_position, 18.0f, 38.0f, lab->aim.x < 0, WHITE);
    DrawCircleLinesV(lab->player_position, WESTERN_PLAYER_RADIUS, (Color){ 35, 105, 225, 235 });
    DrawCircleV(lab->player_position, 1.0f, (Color){ 35, 105, 225, 235 });
    Vector2 muzzle = Vector2Add(lab->player_position, Vector2Scale(lab->aim, 11.0f));
    DrawLineEx(lab->player_position, muzzle, 2.0f, (Color){ 48, 39, 36, 255 });
    if (lab->muzzle_flash > 0.0f) {
        DrawPoly(muzzle, 5, 4.0f, atan2f(lab->aim.y, lab->aim.x) * RAD2DEG, GOLD);
    }

    Vector2 reticle = Vector2Add(lab->player_position, Vector2Scale(lab->aim, 35.0f));
    DrawCircleLines((int)reticle.x, (int)reticle.y, 3, (Color){ 105, 50, 42, 190 });
    DrawLine((int)reticle.x - 5, (int)reticle.y, (int)reticle.x + 5, (int)reticle.y, (Color){ 105, 50, 42, 160 });
    DrawLine((int)reticle.x, (int)reticle.y - 5, (int)reticle.x, (int)reticle.y + 5, (Color){ 105, 50, 42, 160 });

    DrawText("WESTERN", 8, 7, 10, (Color){ 60, 39, 34, 255 });
    DrawText(TextFormat("BAJAS %03i", lab->score), 8, 19, 8, (Color){ 95, 66, 55, 255 });
    DrawTexturePro(lab->revolver, (Rectangle){ 0, 0, lab->revolver.width, lab->revolver.height },
                   (Rectangle){ 281, 5, 17, 17 }, Vector2Zero(), 0, (Color){ 65, 47, 40, 255 });
    for (int i = 0; i < WESTERN_AMMO; ++i) {
        Color color = i < lab->ammo ? (Color){ 124, 75, 35, 255 } : (Color){ 210, 203, 195, 255 };
        DrawRectangle(276 + i * 6, 25, 3, 9, color);
        DrawRectangle(276 + i * 6, 24, 3, 2, Fade(color, 0.7f));
    }
    if (lab->reload_timer > 0.0f) DrawText("RECARGANDO", 244, 38, 7, (Color){ 105, 50, 42, 255 });

    DrawText("WASD + RATON  |  STICKS + RT  |  R/X RECARGA", 45, 229, 6, Fade(DARKGRAY, 0.8f));

    if (lab->dead) {
        DrawRectangle(0, 0, 320, 240, Fade((Color){ 91, 22, 18, 255 }, 0.24f));
        const char *title = "HAS CAIDO";
        const char *prompt = "ENTER / A PARA VOLVER";
        DrawText(title, 160 - MeasureText(title, 18) / 2, 96, 18, (Color){ 80, 25, 22, 255 });
        DrawText(prompt, 160 - MeasureText(prompt, 8) / 2, 121, 8, (Color){ 75, 52, 46, 255 });
    }
}
