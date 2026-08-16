#pragma once

#ifndef NON_EDITOR_BUILD
 #define NON_EDITOR_BUILD 0
 #include "../common.h"
#endif

#define SPRITES_PARALLAX_ASSET_ROOT BROKY_ART_ROOT "/spr/forest-sample"

typedef struct {
    Texture2D mountain;
    Texture2D mountains;
    Texture2D tree_trunk;
    Texture2D tree_leaves[4];
    Texture2D grass;
    Texture2D grass_line;
    Texture2D small_grass;
    Texture2D extra_small_grass;
    Texture2D vegetation;
    float time;
} SpritesParallaxLab;

static void SpritesParallaxSetPointFilter(Texture2D texture)
{
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
}

static unsigned char SpritesParallaxColorChannel(unsigned char shadow, unsigned char highlight, float amount)
{
    return (unsigned char)(shadow + (highlight - shadow) * amount);
}

static Texture2D SpritesParallaxLoadPaletted(const char *path, Color shadow, Color highlight,
                                             float source_low, float source_high)
{
    Image source = LoadImage(path);
    Color *pixels = LoadImageColors(source);
    int pixel_count = source.width * source.height;

    for (int i = 0; i < pixel_count; ++i) {
        float luminance = (pixels[i].r + pixels[i].g + pixels[i].b) / 3.0f;
        float amount = Clamp((luminance - source_low) / (source_high - source_low), 0.0f, 1.0f);
        pixels[i].r = SpritesParallaxColorChannel(shadow.r, highlight.r, amount);
        pixels[i].g = SpritesParallaxColorChannel(shadow.g, highlight.g, amount);
        pixels[i].b = SpritesParallaxColorChannel(shadow.b, highlight.b, amount);
    }

    Image paletted = {
        .data = pixels,
        .width = source.width,
        .height = source.height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    Texture2D texture = LoadTextureFromImage(paletted);
    UnloadImageColors(pixels);
    UnloadImage(source);
    return texture;
}

static SpritesParallaxLab SpritesParallaxLoad(void)
{
    // Leaves and grass use Vegetation_1's darkest and lightest dominant colors,
    // averaging close to #2e466d. The trunk keeps a warm brown contrast.
    const Color leaf_shadow      = {  37,  41, 86, 255 };
    const Color leaf_highlight   = {  50,  91, 130, 255 };
    const Color trunk_shadow     = {  71,  54, 52, 255 };
    const Color trunk_highlight  = { 148, 117, 125, 255 };
    const Color grass_shadow     = {  37,  41, 86, 255 };
    const Color grass_highlight  = {  50,  91, 130, 255 };

    SpritesParallaxLab lab = {
        .mountain          = LoadTexture(SPRITES_PARALLAX_ASSET_ROOT "/Mountain_1.png"),
        .mountains         = LoadTexture(SPRITES_PARALLAX_ASSET_ROOT "/Mountains.png"),
        .tree_trunk        = SpritesParallaxLoadPaletted(
            SPRITES_PARALLAX_ASSET_ROOT "/T_Tree_1_Trunk.png",
            trunk_shadow, trunk_highlight, 88.0f, 128.0f
        ),
        .tree_leaves = {
            SpritesParallaxLoadPaletted(SPRITES_PARALLAX_ASSET_ROOT "/T_Tree_1_Leaves_0.png",
                                        leaf_shadow, leaf_highlight, 88.0f, 140.0f),
            SpritesParallaxLoadPaletted(SPRITES_PARALLAX_ASSET_ROOT "/T_Tree_1_Leaves_1.png",
                                        leaf_shadow, leaf_highlight, 88.0f, 140.0f),
            SpritesParallaxLoadPaletted(SPRITES_PARALLAX_ASSET_ROOT "/T_Tree_1_Leaves_2.png",
                                        leaf_shadow, leaf_highlight, 88.0f, 140.0f),
            SpritesParallaxLoadPaletted(SPRITES_PARALLAX_ASSET_ROOT "/T_Tree_1_Leaves_3.png",
                                        leaf_shadow, leaf_highlight, 88.0f, 140.0f)
        },
        .grass             = SpritesParallaxLoadPaletted(
            SPRITES_PARALLAX_ASSET_ROOT "/T_Vegetation_Grass.png",
            grass_shadow, grass_highlight, 125.0f, 205.0f
        ),
        .grass_line        = SpritesParallaxLoadPaletted(
            SPRITES_PARALLAX_ASSET_ROOT "/T_Vegetation_GrassLine.png",
            grass_shadow, grass_highlight, 125.0f, 205.0f
        ),
        .small_grass       = SpritesParallaxLoadPaletted(
            SPRITES_PARALLAX_ASSET_ROOT "/T_Vegetation_SmGrass.png",
            grass_shadow, grass_highlight, 125.0f, 205.0f
        ),
        .extra_small_grass = SpritesParallaxLoadPaletted(
            SPRITES_PARALLAX_ASSET_ROOT "/T_Vegetation_XsGrass.png",
            grass_shadow, grass_highlight, 125.0f, 205.0f
        ),
        .vegetation        = LoadTexture(SPRITES_PARALLAX_ASSET_ROOT "/Vegetation_1.png"),
        .time              = 0.0f
    };

    SpritesParallaxSetPointFilter(lab.mountain);
    SpritesParallaxSetPointFilter(lab.mountains);
    SpritesParallaxSetPointFilter(lab.tree_trunk);
    for (int i = 0; i < 4; ++i) SpritesParallaxSetPointFilter(lab.tree_leaves[i]);
    SpritesParallaxSetPointFilter(lab.grass);
    SpritesParallaxSetPointFilter(lab.grass_line);
    SpritesParallaxSetPointFilter(lab.small_grass);
    SpritesParallaxSetPointFilter(lab.extra_small_grass);
    SpritesParallaxSetPointFilter(lab.vegetation);

    return lab;
}

static void SpritesParallaxUnload(SpritesParallaxLab *lab)
{
    UnloadTexture(lab->mountain);
    UnloadTexture(lab->mountains);
    UnloadTexture(lab->tree_trunk);
    for (int i = 0; i < 4; ++i) UnloadTexture(lab->tree_leaves[i]);
    UnloadTexture(lab->grass);
    UnloadTexture(lab->grass_line);
    UnloadTexture(lab->small_grass);
    UnloadTexture(lab->extra_small_grass);
    UnloadTexture(lab->vegetation);
    *lab = (SpritesParallaxLab){ 0 };
}

static void SpritesParallaxUpdate(SpritesParallaxLab *lab, float dt)
{
    lab->time += dt;
}

static Vector2 SpritesParallaxPosition(Vector2 position, const Camera2D *camera, float factor)
{
    return Vector2Add(position, Vector2Scale(camera->target, 1.0f - factor));
}

static void SpritesParallaxDrawTexture(Texture2D texture, Vector2 position, float scale, Color tint)
{
    Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle destination = {
        position.x,
        position.y,
        texture.width * scale,
        texture.height * scale
    };
    DrawTexturePro(texture, source, destination, Vector2Zero(), 0.0f, tint);
}

static void SpritesParallaxDrawPivoted(Texture2D texture, Vector2 top_left, float scale,
                                       Vector2 pivot, float rotation, Color tint)
{
    Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Vector2 scaled_pivot = Vector2Scale(pivot, scale);
    Rectangle destination = {
        top_left.x + scaled_pivot.x,
        top_left.y + scaled_pivot.y,
        texture.width * scale,
        texture.height * scale
    };
    DrawTexturePro(texture, source, destination, scaled_pivot, rotation, tint);
}

static void SpritesParallaxDrawTree(const SpritesParallaxLab *lab, const Camera2D *camera,
                                    Vector2 position, float scale, float factor, float phase)
{
    Vector2 top_left = SpritesParallaxPosition(position, camera, factor);
    SpritesParallaxDrawTexture(lab->tree_trunk, top_left, scale, WHITE);

    // All leaf sprites share the trunk's 69x128 canvas. Leaves_0 is the canopy base;
    // Leaves_1..3 are registered detail layers and sway independently around the crown.
    Vector2 crown_pivot = { 34.5f, 76.0f };
    float wind = sinf(lab->time * 1.35f + phase);
    SpritesParallaxDrawPivoted(lab->tree_leaves[0], top_left, scale, crown_pivot,
                               wind * 0.70f, WHITE);
    for (int i = 1; i < 4; ++i) {
        float detail_wind = sinf(lab->time * (1.55f + i * 0.12f) + phase + i * 0.85f);
        SpritesParallaxDrawPivoted(lab->tree_leaves[i], top_left, scale, crown_pivot,
                                   detail_wind * (0.8f + i * 0.22f), WHITE);
    }
}

static void SpritesParallaxDraw(const SpritesParallaxLab *lab, const Camera2D *camera)
{
    Vector2 sky = SpritesParallaxPosition((Vector2){ -900.0f, -600.0f }, camera, 0.0f);
    DrawRectangleV(sky, (Vector2){ 1800.0f, 1200.0f }, (Color){ 19, 30, 50, 255 });

    // Far silhouettes: low factors make them drift slowly against the camera.
    for (int i = -5; i <= 5; ++i) {
        Vector2 p = SpritesParallaxPosition((Vector2){ i * 82.0f - 30.0f, 5.0f }, camera, 0.12f);
        SpritesParallaxDrawTexture(lab->mountains, p, 1.0f, (Color){ 116, 137, 171, 210 });
    }
    for (int i = -4; i <= 4; ++i) {
        Vector2 p = SpritesParallaxPosition((Vector2){ i * 112.0f - 55.0f, 29.0f }, camera, 0.28f);
        SpritesParallaxDrawTexture(lab->mountain, p, 1.15f, (Color){ 170, 181, 204, 235 });
    }

    // A quiet middle plane keeps the tree silhouettes readable.
    Vector2 middle_ground = SpritesParallaxPosition((Vector2){ -700.0f, 73.0f }, camera, 0.50f);
    DrawRectangleV(middle_ground, (Vector2){ 1400.0f, 220.0f }, (Color){ 30, 54, 65, 255 });

    SpritesParallaxDrawTree(lab, camera, (Vector2){ -104.0f, -24.0f }, 0.78f, 0.58f, 1.7f);
    SpritesParallaxDrawTree(lab, camera, (Vector2){  104.0f, -37.0f }, 0.90f, 0.67f, 3.1f);
    SpritesParallaxDrawTree(lab, camera, (Vector2){   28.0f, -54.0f }, 1.05f, 0.78f, 0.0f);

    // Grass is the vegetation base; line, small tufts and the vegetation cluster add detail.
    for (int i = -5; i <= 5; ++i) {
        Vector2 p = SpritesParallaxPosition(
            (Vector2){ i * 124.0f - 32.0f, 68.0f - lab->grass.height + 16.0f }, camera, 0.90f
        );
        p.x += sinf(lab->time * 0.85f + i) * 0.7f;
        SpritesParallaxDrawTexture(lab->grass, p, 1.0f, WHITE);
    }
    for (int i = -3; i <= 3; ++i) {
        Vector2 p = SpritesParallaxPosition((Vector2){ i * 252.0f - 86.0f, 64.0f }, camera, 0.96f);
        SpritesParallaxDrawTexture(lab->grass_line, p, 1.0f, WHITE);
    }

    for (int i = -5; i <= 6; ++i) {
        Texture2D tuft = (i & 1) ? lab->small_grass : lab->extra_small_grass;
        float scale = (i & 1) ? 1.0f : 1.35f;
        Vector2 p = SpritesParallaxPosition((Vector2){ i * 43.0f - 12.0f, 58.0f + (i & 1) * 7.0f }, camera, 1.04f);
        Vector2 pivot = { tuft.width * 0.5f, (float)tuft.height };
        float sway = sinf(lab->time * 2.1f + i * 0.72f) * 1.8f;
        SpritesParallaxDrawPivoted(tuft, p, scale, pivot, sway, WHITE);
    }

    Vector2 bush = SpritesParallaxPosition((Vector2){ -55.0f, 48.0f }, camera, 1.08f);
    SpritesParallaxDrawPivoted(lab->vegetation, bush, 0.72f,
                               (Vector2){ 39.5f, 73.0f }, sinf(lab->time * 1.4f) * 0.8f, WHITE);
}
