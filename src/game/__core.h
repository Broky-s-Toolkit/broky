#pragma once
#ifndef NON_EDITOR_BUILD
 #define NON_EDITOR_BUILD 0
 #include "gui/gui.h"
 #include "common.h"
 #include "box2d/box2d.h"
#endif

//
// CORE
//

typedef struct {
    Texture2D texture;
    Vector2 position;
    float parallax;      // Parallax factor (1.0f = camera locked)
    float scale;
    Color tint;
} Game_Element;

Game_Element Game_MakeElement(const char *path, Vector2 position, float parallax, float scale, Color tint)
{
    Game_Element element = {
        .texture    = LoadTexture(path),
        .position   = position,
        .parallax   = parallax,
        .scale      = scale,
        .tint       = tint
    };
    return element;
}

//
// GAME ITSELF
//

#define CHARACTERS              4
#define CHARACTER_MAX_SPEED     32
#define GAME_PHYSICS_PIXELS_PER_METER 32.0f

typedef struct {
    bool reset_characters;
    bool add_character;
    bool toggle_character;
    bool move_up;
    bool move_down;
    bool move_left;
    bool move_right;
} GAME_Actions;

GAME_Actions GAME_MakeActions()
{
    GAME_Actions actions = { 0 };
    return actions;
}

typedef struct  {
    Rectangle shape;
    Color color;
    Vector2 movement;
    float anim_time;
    b2BodyId body_id;
    bool colliding;
} GAME_Character;

typedef struct {
    int             current_character;
    int             alive_characters;
    Camera2D        camera2D;
    b2WorldId       physics_world_id;
    float           physics_accumulator;
    GAME_Character  characters[CHARACTERS];
} GAME_State;

GAME_State GAME_MakeState(void)
{
    GAME_State state = {
        .current_character = 0,
        .alive_characters  = 2,
        .camera2D = {
            .offset   = (Vector2){ 0, 0 },
            .target   = (Vector2){ 0, 0 },
            .rotation = 0.0f,
            .zoom     = 1.0f
        },
        .physics_world_id = b2_nullWorldId,
        .physics_accumulator = 0.0f,
        .characters = {
            (GAME_Character){ .shape = {  0,  0, 10, 20 }, .color = RED,    .movement = { 0 }, .anim_time = 0.0f, .body_id = b2_nullBodyId, .colliding = false },
            (GAME_Character){ .shape = { 10, 30, 10, 20 }, .color = BLUE,   .movement = { 0 }, .anim_time = 0.0f, .body_id = b2_nullBodyId, .colliding = false },
            (GAME_Character){ .shape = { 50, 60, 10, 20 }, .color = GREEN,  .movement = { 0 }, .anim_time = 0.0f, .body_id = b2_nullBodyId, .colliding = false },
            (GAME_Character){ .shape = { 80, 60, 10, 20 }, .color = ORANGE, .movement = { 0 }, .anim_time = 0.0f, .body_id = b2_nullBodyId, .colliding = false },
        }
    };

    return state;
}

// > GAME GUI State

typedef struct {
    bool checkbox_value;
    EGUI_Font demo_font;
    EGUI_Font editor_font;
    char input_contents[256];
    char input_int_contents[256];
    char input_float_contents[256];
} GAME_WindowState;

GAME_WindowState GAME_MakeWindowState()
{
    GAME_WindowState state      = {
        .checkbox_value         = true,
        .demo_font              = EGUI_Font_Default,
        .editor_font            = EGUI_Font_ShareTech,
        .input_contents         = {'\0'},
        .input_int_contents     = {'\0'},
        .input_float_contents   = {'\0'}
    };
    return state;
}

typedef struct {
    GAME_Actions  player_actions;
} GAME_Temp;

GAME_Temp GAME_MakeTemp()
{
    GAME_Temp temp = {
        .player_actions     = {0}
    };
    return temp;
}

// > CONTEXT

static struct {
    GAME_State          *state;
    GAME_WindowState    *win_state;
    GAME_Temp           *temp;
} GAME_CTX = { 0 };

void GAME_SetContext(GAME_State *state, GAME_WindowState *win_state, GAME_Temp *temp)
{
    GAME_CTX.state      = state;
    GAME_CTX.win_state  = win_state;
    GAME_CTX.temp       = temp;
}

// > FUNCTIONS
//   INDEX

void                GAME_UpdateNextCharacter();
void                GAME_AddCharacter();
GAME_Character*     GAME_GetCurrentCharacter();
bool                GAME_CheckRingCollision(GAME_Character* character1, GAME_Character* character2, float radius);
void                GAME_UpdateCollisions(bool collisions[], float radius);


// > IMPLEMENTATION
#ifdef IMPLEMENT_ALL
void GAME_UpdateNextCharacter()
{
    GAME_State* state   = GAME_CTX.state;
    state->current_character = (state->current_character + 1) % state->alive_characters;
}

void GAME_AddCharacter()
{
    GAME_State* state   = GAME_CTX.state;
    if (state->alive_characters >= CHARACTERS) {
        return;
    }

    int next_index = state->alive_characters;
    GAME_Character *character = &state->characters[next_index];
    if (B2_IS_NON_NULL(character->body_id)) {
        Vector2 center = RectCenter(character->shape);
        b2Body_Enable(character->body_id);
        b2Body_SetTransform(character->body_id, (b2Pos){
            center.x / GAME_PHYSICS_PIXELS_PER_METER,
            center.y / GAME_PHYSICS_PIXELS_PER_METER
        }, b2MakeRot(0.0f));
        b2Body_SetLinearVelocity(character->body_id, (b2Vec2){ 0.0f, 0.0f });
    }

    state->alive_characters++;
}

GAME_Character* GAME_GetCurrentCharacter()
{
    GAME_State* state   = GAME_CTX.state;
    return &state->characters[state->current_character];
}

bool GAME_CheckRingCollision(GAME_Character* character1, GAME_Character* character2, float radius)
{
    if (character1->colliding && character2->colliding) {
        return true;
    }

    Vector2 center1     = RectCenter(character1->shape);
    Vector2 center2     = RectCenter(character2->shape);
    float distance      = Vector2Distance(center1, center2);
    return distance < (radius * 2);
}

void GAME_UpdateCollisions(bool collisions[], float radius)
{
    GAME_State* state   = GAME_CTX.state;
    (void)radius;
    for (int i = 0; i < CHARACTERS; i++) {
        collisions[i] = state->characters[i].colliding;
    }
}
#endif
