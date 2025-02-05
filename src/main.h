#ifndef MAIN_H
#define MAIN_H

#include "../raylib/include/raylib.h"
#include <math.h>

#define WINSIDE 1000

#define LINE_BIG 5
#define LINE_SMALL 1
#define POINT_RADIUS 10

#define MAIN_COL ((Color){255,255,255,255})
#define FILL_COL ((Color){255,255,255,32})
#define SIN_COL ((Color){0,128,255,255})
#define COS_COL ((Color){200,0,255,255})
#define TAN_COL ((Color){255,128,0,255})

typedef struct UnitCircle {
    Vector2 position;
    Vector2 center;
    float radius;
    float rad;
    float deg;
    float sin;
    float cos;
    float tan;
    Vector2 point;
} UnitCircle;

typedef struct Range {
    float min;
    float max;
} Range;

typedef struct TrigonometricFunction {
    char name[4];
    float (*function)(float);
    Range range;
    Vector2 position;
    Vector2 size;
    Color color;
} TrigonometricFunction;

typedef enum TextFlags {
    TEXT_FLAG_NONE = 0,
    TEXT_FLAG_LARGE = 1 << 0,
    TEXT_FLAG_BACKING_RECTANGLE = 1 << 1,
} TextFlags;

static inline bool has_flag(int flags, int flag) {
    return (flags & flag) == flag;
}

static inline bool is_point_inside_area(Vector2 area_pos, Vector2 area_size, Vector2 point_pos) {
    return (
        point_pos.x > area_pos.x &&
        point_pos.x < (area_pos.x + area_size.x) &&
        point_pos.y > area_pos.y &&
        point_pos.y < (area_pos.y + area_size.y)
    );
}

static inline Vector2 get_angle_direction(float deg) {
    float rad = deg * DEG2RAD;
    return (Vector2) {
        cosf(rad),
        -sinf(rad)
    };
}

static inline Vector2 vec2_in_direction(Vector2 position, Vector2 direction, float distance) {
    return (Vector2) {
        position.x + (direction.x * distance),
        position.y + (direction.y * distance),
    };
}

static inline void draw_text_centered(Font *font, TextFlags flags, Vector2 position, float rotation, const char *text, Color color) {
    int size = has_flag(flags, TEXT_FLAG_LARGE) ? 40 : 30;
    int spacing = 2;
    Vector2 text_dimensions = MeasureTextEx(*font, text, size, spacing);
    Vector2 text_origin = {
        text_dimensions.x/2,
        text_dimensions.y/2
    };
    const float padding = 5;
    if (has_flag(flags, TEXT_FLAG_BACKING_RECTANGLE)) {
        DrawRectangle(
            position.x - (text_dimensions.x/2) - padding,
            position.y - (text_dimensions.y/2) - padding,
            text_dimensions.x + (padding*2),
            text_dimensions.y + (padding*2),
            (Color) {0,0,0,128}
        );
    }
    DrawTextPro(*font, text, position, text_origin, rotation, size, spacing, color);
}

#endif