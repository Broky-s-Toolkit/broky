#pragma once
#include "global.h"

#define Assert(cond) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "Assert failed: %s, file %s, line %d\n", #cond, __FILE__, __LINE__); \
            exit(1); \
        } \
    } while (0)

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define StaticAssert(cond, msg) _Static_assert((cond), #msg)
#else
    #define STATIC_ASSERT_JOIN_(a, b) a##b
    #define STATIC_ASSERT_JOIN(a, b) STATIC_ASSERT_JOIN_(a, b)
    #define StaticAssert(cond, msg) \
        enum { STATIC_ASSERT_JOIN(static_assert_failed_##msg##_line_, __LINE__) = 1 / ((cond) ? 1 : 0) }
#endif

const char* BuildTimeFormatted();
Vector2 WaitForWindowSizeStable(int stable_frames, int max_frames);
void InitArtWorkingDirectory(void);
bool ParseFloatStrict(const char *s, float *out);
Color MakeColor(unsigned char r, unsigned char g, unsigned char b);
Color MakeColorAlpha(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
Rectangle LimitRect(Rectangle shape, Rectangle limits);
Vector2 LimitVector2Rect(Vector2 point, Rectangle limits);
void DrawDebugRect(Rectangle rect, Color color);
Rectangle RelativeToRect(Rectangle rectangle, Rectangle relativeTo);
Rectangle RectIntersection(Rectangle a, Rectangle b);
bool IsKeyEnterPressed();
bool CheckCollisionPointRecWithMargin(Vector2 point, Rectangle rect, float margin);
Rectangle GetSourceRec(Texture2D texture);
Rectangle FlipYRec(Rectangle rectangle);
Rectangle MoveAndExtendXY(Rectangle rectangle, float dx, float dy);
Rectangle MoveRect(Rectangle rect, Vector2 offset);
Rectangle AddRect(Rectangle rectangle, float dx, float dy, float dw, float dh);
Vector2 AddVector2(Vector2 vector, float dx, float dy);
Rectangle RectFromVector2(Vector2 position, float w, float h);
Vector2 RectPosition(Rectangle rect);
float SnapFloat(float x);
Vector2 SnapVector2(Vector2 v);
float ClampFloat(float v, float min_v, float max_v);
Color HSVToRGB(float h, float s, float v);
void BeginScissorModeRect(Rectangle rect);
int IntMax(int a, int b);
int IntMin(int a, int b);
float FloatMax(float a, float b);
float FloatMin(float a, float b);
int IntAbs(int value);
float FloatAbs(float value);
int FloatCeil(float value);
int FloatFloor(float value);
void DrawTextureFullScreenKeep(Texture2D tex, Color tint);
void DrawTextureFullScreen(Texture2D tex, Color tint);
int StringSize(const char *str);
Vector2 RectCenter(Rectangle shape);
void TrySetTargetMonitor(int targetMonitor, int retries);
Rectangle GetScreenRect();

#include "rayext.c"
