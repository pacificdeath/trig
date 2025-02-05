#include "main.h"

#define TRIGONOMETRIC_FUNCTION_RESOLUTION 32

void trigonometric_function_draw(TrigonometricFunction *tf, Font *font, float radians) {
    DrawLine(
        tf->position.x, tf->position.y + (tf->size.y/2),
        tf->position.x + tf->size.x, tf->position.y + (tf->size.y/2),
        MAIN_COL
    );
    const int vertical_line_count = 5;
    for (int j = 0; j < vertical_line_count; j++) {
        const float fract = (tf->size.x/(vertical_line_count-1));
        float x = tf->position.x + (fract * j);
        DrawLine(
            x, tf->position.y,
            x, tf->position.y + (tf->size.y),
            MAIN_COL
        );
        Vector2 text_position = {x, tf->position.y - WINSIDE * 0.05};
        float text_rotation = 315;
        switch (j) {
        case 0: draw_text_centered(font, TEXT_FLAG_NONE, text_position, text_rotation, "0", MAIN_COL); break;
        case 1: draw_text_centered(font, TEXT_FLAG_NONE, text_position, text_rotation, "pi/2", MAIN_COL); break;
        case 2: draw_text_centered(font, TEXT_FLAG_NONE, text_position, text_rotation, "pi", MAIN_COL); break;
        case 3: draw_text_centered(font, TEXT_FLAG_NONE, text_position, text_rotation, "3pi/2", MAIN_COL); break;
        case 4: draw_text_centered(font, TEXT_FLAG_NONE, text_position, text_rotation, "2pi", MAIN_COL); break;
        }
    }
    float x_fract = tf->size.x / TRIGONOMETRIC_FUNCTION_RESOLUTION;
    float y_fract = PI*2/TRIGONOMETRIC_FUNCTION_RESOLUTION;
    float half_height = (tf->size.y/2);
    Vector2 prev = {
        tf->position.x,
        tf->position.y - (tf->function(0) / (tf->range.max - tf->range.min) * tf->size.y) + half_height,
    };
    float func_min = tf->position.y;
    float func_max = (tf->position.y + tf->size.y);
    for (int j = 0; j <= TRIGONOMETRIC_FUNCTION_RESOLUTION; j++) {
        Vector2 next = {
            tf->position.x + x_fract*j,
            tf->position.y - (tf->function(y_fract*j) / (tf->range.max - tf->range.min) * tf->size.y) + half_height,
        };
        if (next.y < func_min) {
            next.y = func_min;
        } else if (next.y > func_max) {
            next.y = func_max;
        }
        if ((prev.y != func_min && prev.y != func_max) ||
            (next.y != func_min && next.y != func_max)
        ) {
            DrawLineEx(prev, next, LINE_BIG, tf->color);
        }
        prev = next;
    }

    float current_rad_result = tf->function(radians);
    bool inside_bounds = (current_rad_result <= tf->range.max) && (current_rad_result >= tf->range.min);
    Vector2 func_pos;
    func_pos.x = tf->position.x + (tf->size.x) / PI / 2 * radians;
    if (inside_bounds) {
        func_pos.y = tf->position.y - (current_rad_result / (tf->range.max - tf->range.min) * tf->size.y) + half_height;
        DrawCircleV(func_pos, POINT_RADIUS, MAIN_COL);
    } else {
        if (current_rad_result > 0) {
            func_pos.y = func_min;
        } else {
            func_pos.y = func_max;
        }
    }
    DrawLine(
        tf->position.x, func_pos.y,
        func_pos.x, func_pos.y,
        MAIN_COL
    );
    draw_text_centered(
        font,
        TEXT_FLAG_NONE,
        (Vector2){tf->position.x - (WINSIDE * 0.05), func_pos.y},
        0,
        inside_bounds ? TextFormat("%.2f", current_rad_result) : "??",
        tf->color
    );
    draw_text_centered(
        font,
        TEXT_FLAG_LARGE,
        (Vector2){tf->position.x + (tf->size.x/2), tf->position.y + (tf->size.y) + WINSIDE * 0.05},
        0,
        tf->name,
        tf->color
    );
}
