#pragma once

#ifndef NON_EDITOR_BUILD
 #define NON_EDITOR_BUILD 0
 #include "../common.h"
#endif

#include "lab_wallpaper.h"
#include "sprites-paralax.h"
#include "western.h"

typedef enum {
    LAB_WALLPAPER,
    LAB_PARALLAX,
    LAB_WESTERN,
    LAB_COUNT
} ELabType;

typedef struct {
    Camera2D *camera;
    int       width;
    int       height;
} LabDrawContext;

typedef void (*LabUpdateFn)(void *state, float dt);
typedef void (*LabDrawFn)(void *state, const LabDrawContext *context);
typedef void (*LabUnloadFn)(void *state);

typedef struct {
    bool          active;
    void         *state;
    LabUpdateFn   update;
    LabDrawFn     draw;
    LabUnloadFn   unload;
} Lab;

typedef struct {
    ELabType  type;
    Lab       runtime;

    union {
        WallpaperLab       wallpaper;
        SpritesParallaxLab parallax;
        WesternLab         western;
    } state;
} LabEntry;

typedef struct {
    LabEntry entries[LAB_COUNT];
} Labs;

void Labs_Init(Labs *labs);
void Labs_SetActive(Labs *labs, ELabType type, bool active);
bool Labs_IsActive(const Labs *labs, ELabType type);
void Labs_Update(Labs *labs, float dt);
void Labs_Draw(Labs *labs, const LabDrawContext *context);
void Labs_Unload(Labs *labs);

static void LabWallpaperUpdate(void *state, float dt)
{
    WallpaperLabUpdate((WallpaperLab *)state, dt);
}

static void LabWallpaperDraw(void *state, const LabDrawContext *context)
{
    WallpaperLabDraw((const WallpaperLab *)state, context->width, context->height);
}

static void LabWallpaperUnload(void *state)
{
    WallpaperLabUnload((WallpaperLab *)state);
}

static void LabParallaxUpdate(void *state, float dt)
{
    SpritesParallaxUpdate((SpritesParallaxLab *)state, dt);
}

static void LabParallaxDraw(void *state, const LabDrawContext *context)
{
    if (context->camera == NULL) return;

    BeginMode2D(*context->camera);
        SpritesParallaxDraw((const SpritesParallaxLab *)state, context->camera);
    EndMode2D();
}

static void LabParallaxUnload(void *state)
{
    SpritesParallaxUnload((SpritesParallaxLab *)state);
}

static void LabWesternUpdate(void *state, float dt)
{
    WesternUpdate((WesternLab *)state, dt);
}

static void LabWesternDraw(void *state, const LabDrawContext *context)
{
    WesternDraw((const WesternLab *)state, context->width, context->height);
}

static void LabWesternUnload(void *state)
{
    WesternUnload((WesternLab *)state);
}

void Labs_Init(Labs *labs)
{
    *labs = (Labs){ 0 };

    LabEntry *wallpaper = &labs->entries[LAB_WALLPAPER];
    wallpaper->type            = LAB_WALLPAPER;
    wallpaper->state.wallpaper = WallpaperLabLoad(0, 0xC0FFEEu, false);
    wallpaper->runtime         = (Lab){
        .active = false,
        .state  = &wallpaper->state.wallpaper,
        .update = LabWallpaperUpdate,
        .draw   = LabWallpaperDraw,
        .unload = LabWallpaperUnload
    };

    LabEntry *parallax = &labs->entries[LAB_PARALLAX];
    parallax->type           = LAB_PARALLAX;
    parallax->state.parallax = SpritesParallaxLoad();
    parallax->runtime        = (Lab){
        .active = false,
        .state  = &parallax->state.parallax,
        .update = LabParallaxUpdate,
        .draw   = LabParallaxDraw,
        .unload = LabParallaxUnload
    };

    LabEntry *western = &labs->entries[LAB_WESTERN];
    western->type          = LAB_WESTERN;
    western->state.western = WesternLoad();
    western->runtime       = (Lab){
        .active = true,
        .state  = &western->state.western,
        .update = LabWesternUpdate,
        .draw   = LabWesternDraw,
        .unload = LabWesternUnload
    };
}

void Labs_SetActive(Labs *labs, ELabType type, bool active)
{
    if (type < 0 || type >= LAB_COUNT) return;
    labs->entries[type].runtime.active = active;
}

bool Labs_IsActive(const Labs *labs, ELabType type)
{
    if (type < 0 || type >= LAB_COUNT) return false;
    return labs->entries[type].runtime.active;
}

void Labs_Update(Labs *labs, float dt)
{
    for (int i = 0; i < LAB_COUNT; ++i) {
        Lab *lab = &labs->entries[i].runtime;
        if (lab->active && lab->update != NULL) lab->update(lab->state, dt);
    }
}

void Labs_Draw(Labs *labs, const LabDrawContext *context)
{
    for (int i = 0; i < LAB_COUNT; ++i) {
        Lab *lab = &labs->entries[i].runtime;
        if (lab->active && lab->draw != NULL) lab->draw(lab->state, context);
    }
}

void Labs_Unload(Labs *labs)
{
    for (int i = 0; i < LAB_COUNT; ++i) {
        Lab *lab = &labs->entries[i].runtime;
        if (lab->unload != NULL) lab->unload(lab->state);
    }

    *labs = (Labs){ 0 };
}
