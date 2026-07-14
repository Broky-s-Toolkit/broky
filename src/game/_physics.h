#pragma once
#ifndef IDE_SYNTAX_HL
 #include "__core.h"
#endif

#define GAME_PHYSICS_STEP_SECONDS     (1.0f / 60.0f)
#define GAME_PHYSICS_SUBSTEPS         4

void GAME_InitPhysics(GAME_State *state);
void GAME_DestroyPhysics(GAME_State *state);
void GAME_StepPhysics(GAME_State *state, float dt);

#ifdef IMPLEMENT_ALL
static float GAME_PhysicsPixelsToMeters(float pixels)
{
    return pixels / GAME_PHYSICS_PIXELS_PER_METER;
}

static Vector2 GAME_PhysicsMetersToPixelsVec2(b2Vec2 value)
{
    return (Vector2) {
        value.x * GAME_PHYSICS_PIXELS_PER_METER,
        value.y * GAME_PHYSICS_PIXELS_PER_METER
    };
}

static b2Pos GAME_PhysicsRectCenterToMeters(Rectangle shape)
{
    Vector2 center = RectCenter(shape);
    return (b2Pos) {
        GAME_PhysicsPixelsToMeters(center.x),
        GAME_PhysicsPixelsToMeters(center.y)
    };
}

static void GAME_SyncCharacterFromBody(GAME_Character *character)
{
    Assert(character != NULL);
    if (B2_IS_NULL(character->body_id)) {
        return;
    }

    Vector2 size = { character->shape.width, character->shape.height };
    Vector2 center = GAME_PhysicsMetersToPixelsVec2(b2Body_GetPosition(character->body_id));
    character->shape.x = center.x - size.x * 0.5f;
    character->shape.y = center.y - size.y * 0.5f;
}

void GAME_InitPhysics(GAME_State *state)
{
    Assert(state != NULL);
    Assert(B2_IS_NULL(state->physics_world_id));

    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = (b2Vec2){ 0.0f, 0.0f };
    world_def.enableSleep = false;
    world_def.workerCount = 1;
    state->physics_world_id = b2CreateWorld(&world_def);
    state->physics_accumulator = 0.0f;

    for (int i = 0; i < CHARACTERS; ++i) {
        GAME_Character *character = &state->characters[i];

        b2BodyDef body_def = b2DefaultBodyDef();
        body_def.type = b2_dynamicBody;
        body_def.position = GAME_PhysicsRectCenterToMeters(character->shape);
        body_def.linearDamping = 6.0f;
        body_def.angularDamping = 10.0f;
        body_def.enableSleep = false;
        body_def.motionLocks.angularZ = true;
        body_def.userData = character;
        character->body_id = b2CreateBody(state->physics_world_id, &body_def);

        b2ShapeDef shape_def = b2DefaultShapeDef();
        shape_def.density = 1.0f;
        shape_def.material.friction = 0.15f;
        shape_def.material.restitution = 0.0f;
        shape_def.enableContactEvents = true;
        shape_def.userData = character;

        b2Polygon box = b2MakeBox(
            GAME_PhysicsPixelsToMeters(character->shape.width * 0.5f),
            GAME_PhysicsPixelsToMeters(character->shape.height * 0.5f)
        );
        b2CreatePolygonShape(character->body_id, &shape_def, &box);

        if (i >= state->alive_characters) {
            b2Body_Disable(character->body_id);
        }
    }
}

void GAME_DestroyPhysics(GAME_State *state)
{
    Assert(state != NULL);
    if (B2_IS_NON_NULL(state->physics_world_id)) {
        b2DestroyWorld(state->physics_world_id);
        state->physics_world_id = b2_nullWorldId;
    }

    state->physics_accumulator = 0.0f;
    for (int i = 0; i < CHARACTERS; ++i) {
        state->characters[i].body_id = b2_nullBodyId;
        state->characters[i].colliding = false;
    }
}

void GAME_StepPhysics(GAME_State *state, float dt)
{
    Assert(state != NULL);
    if (B2_IS_NULL(state->physics_world_id)) {
        return;
    }

    for (int i = 0; i < CHARACTERS; ++i) {
        state->characters[i].colliding = false;
    }

    for (int i = 0; i < state->alive_characters; ++i) {
        GAME_Character *character = &state->characters[i];
        Vector2 movement = character->movement;
        if (Vector2LengthSqr(movement) > 1.0f) {
            movement = Vector2Normalize(movement);
        }

        b2Vec2 velocity = {
            GAME_PhysicsPixelsToMeters(movement.x * CHARACTER_MAX_SPEED * 2.0f),
            GAME_PhysicsPixelsToMeters(movement.y * CHARACTER_MAX_SPEED * 2.0f)
        };
        b2Body_SetLinearVelocity(character->body_id, velocity);
    }

    dt = Clamp(dt, 0.0f, 0.25f);
    state->physics_accumulator += dt;
    while (state->physics_accumulator >= GAME_PHYSICS_STEP_SECONDS) {
        b2World_Step(state->physics_world_id, GAME_PHYSICS_STEP_SECONDS, GAME_PHYSICS_SUBSTEPS);
        state->physics_accumulator -= GAME_PHYSICS_STEP_SECONDS;
    }

    for (int i = 0; i < state->alive_characters; ++i) {
        GAME_Character *character = &state->characters[i];
        GAME_SyncCharacterFromBody(character);

        int contact_capacity = b2Body_GetContactCapacity(character->body_id);
        if (contact_capacity <= 0) {
            continue;
        }

        b2ContactData contact_data[8];
        Assert(contact_capacity <= (int)(sizeof(contact_data) / sizeof(contact_data[0])));
        int contact_count = b2Body_GetContactData(character->body_id, contact_data, contact_capacity);
        character->colliding = contact_count > 0;
    }

    for (int i = state->alive_characters; i < CHARACTERS; ++i) {
        state->characters[i].colliding = false;
    }
}
#endif
