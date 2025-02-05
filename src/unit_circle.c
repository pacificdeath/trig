#include "main.h"

#define QUADRANT_I_OFFSET 0
#define QUADRANT_II_OFFSET (PI/2)
#define QUADRANT_III_OFFSET PI
#define QUADRANT_IV_OFFSET (PI*1.5)

void unit_circle_update_towards(UnitCircle *uc, Vector2 position) {
    const float adj = position.x - uc->center.x;
    const float opp = uc->center.y - position.y;

    const float hyp = sqrtf(powf(adj,2) + powf(opp,2));
    const float hyp_normalize_factor = 1 / hyp;

    uc->cos = adj * hyp_normalize_factor;
    uc->sin = opp * hyp_normalize_factor;
    uc->tan = uc->sin / uc->cos;

    uc->point = (Vector2) {
        uc->center.x + (uc->cos * uc->radius),
        uc->center.y - (uc->sin * uc->radius),
    };

    const bool right = position.x > uc->center.x;
    const bool top = position.y < uc->center.y;

    if (top) {
        if (right) {
            uc->rad = QUADRANT_I_OFFSET + asinf(opp/hyp);
        } else {
            uc->rad = QUADRANT_II_OFFSET + acosf(opp/hyp);
        }
    } else if (!right) {
        uc->rad = QUADRANT_III_OFFSET + asinf(-(opp/hyp));
    } else {
        uc->rad = QUADRANT_IV_OFFSET + acosf(-(opp/hyp));
    }

    uc->deg = uc->rad * RAD2DEG;
}

void unit_circle_update_radians(UnitCircle *uc, float rad) {
    uc->rad = rad;
    uc->cos = cosf(rad);
    uc->sin = sinf(rad);
    uc->tan = tanf(rad);

    uc->point = (Vector2) {
        uc->center.x + (uc->cos * uc->radius),
        uc->center.y - (uc->sin * uc->radius),
    };

    uc->deg = uc->rad * RAD2DEG;
}

void unit_circle_draw_base(UnitCircle *uc) {
    DrawCircleLinesV(uc->center, uc->radius, MAIN_COL);
    DrawCircleSector(uc->center, uc->radius, 0, -uc->deg, uc->rad / 10, FILL_COL);
    DrawCircleSectorLines(uc->center, uc->radius * 0.15 * 1.4, 0, -uc->deg, uc->rad / 10, MAIN_COL);

    Vector2 vertical_line_start = { uc->position.x, uc->center.y };
    Vector2 vertical_line_end = { uc->position.x + (uc->radius*2), uc->center.y };
    DrawLineV(vertical_line_start, vertical_line_end, MAIN_COL);

    Vector2 horizontal_line_start = { uc->center.x, uc->position.y };
    Vector2 horizontal_line_end = { uc->center.x, uc->position.y + (uc->radius*2) };
    DrawLineV(horizontal_line_start, horizontal_line_end, MAIN_COL);
}

void unit_circle_draw_triangle(UnitCircle *uc, Font *font) {
    Vector2 sin_corner = {uc->center.x, uc->point.y};
    Vector2 cos_corner = {uc->point.x, uc->center.y};

    DrawLineEx(sin_corner, uc->point, LINE_SMALL, SIN_COL);
    DrawLineEx(cos_corner, uc->point, LINE_SMALL, COS_COL);

    DrawLineEx(uc->center, uc->point, LINE_BIG, MAIN_COL);
    DrawCircleV(uc->point, POINT_RADIUS, MAIN_COL);

    DrawLineEx(uc->center, sin_corner, LINE_BIG, SIN_COL);
    DrawCircleV(sin_corner, POINT_RADIUS, SIN_COL);

    DrawLineEx(uc->center, cos_corner, LINE_BIG, COS_COL);
    DrawCircleV(cos_corner, POINT_RADIUS, COS_COL);

    const float trig_func_text_offset = 0.05f;
    Vector2 sin_text_position = {
        (uc->cos < 0) ? uc->center.x + (WINSIDE*trig_func_text_offset) : uc->center.x - (WINSIDE*trig_func_text_offset),
        uc->center.y - (uc->sin * uc->radius),
    };
    draw_text_centered(font, TEXT_FLAG_BACKING_RECTANGLE, sin_text_position, 0, TextFormat("%.2f", uc->sin), SIN_COL);
    Vector2 cos_text_position = {
        uc->center.x + (uc->cos * uc->radius),
        (uc->sin < 0) ? uc->center.y - (WINSIDE*trig_func_text_offset) : uc->center.y + (WINSIDE*trig_func_text_offset),
    };
    draw_text_centered(font, TEXT_FLAG_BACKING_RECTANGLE, cos_text_position, 0, TextFormat("%.2f", uc->cos), COS_COL);
}

void unit_circle_draw_angles_on_circumference(UnitCircle *uc, Font *font, float *angles, int angle_count) {
    for (int i = 0; i < angle_count; i++) {
        float deg = angles[i];
        Vector2 dir = get_angle_direction(deg);
        DrawLineV(
            vec2_in_direction(uc->center, dir, 0.95f * uc->radius),
            vec2_in_direction(uc->center, dir, 1.05f * uc->radius),
            MAIN_COL
        );
        draw_text_centered(
            font,
            TEXT_FLAG_NONE,
            vec2_in_direction(uc->center, dir, 1.2f * uc->radius),
            (deg <= 90 || deg > 270) ? -deg : 180-deg,
            TextFormat("%.0f", deg),
            MAIN_COL
        );
    }
}

void unit_circle_draw_quadrants(UnitCircle *uc, Font *font) {
    float offset = (WINSIDE * 0.05);
    float x = uc->position.x - offset;
    float y = uc->position.y - offset;
    float o = (uc->radius * 2) + (offset * 2);
    draw_text_centered(font, TEXT_FLAG_LARGE, (Vector2){x+o, y   }, 0, "I", MAIN_COL);
    draw_text_centered(font, TEXT_FLAG_LARGE, (Vector2){x  , y   }, 0, "II", MAIN_COL);
    draw_text_centered(font, TEXT_FLAG_LARGE, (Vector2){x  , y+o }, 0, "III", MAIN_COL);
    draw_text_centered(font, TEXT_FLAG_LARGE, (Vector2){x+o, y+o }, 0, "IV", MAIN_COL);
}

void unit_circle_draw_right_angle(UnitCircle *uc) {
    const float offset = uc->radius * 0.15;
    const float low_offset = 20;
    const float high_offset = 65;
    Vector2 right_angle_offset;
    if (uc->deg > low_offset && uc->deg < high_offset) {
        Vector2 v = { uc->point.x - offset, uc->center.y - offset };
        DrawLine(v.x, v.y, uc->point.x, v.y, MAIN_COL);
        DrawLine(v.x, v.y, v.x, uc->center.y, MAIN_COL);
    } else if (uc->deg > (low_offset+90) && uc->deg < (high_offset+90)) {
        Vector2 v = { uc->center.x - offset, uc->point.y + offset };
        DrawLine(v.x, v.y, uc->center.x, v.y, MAIN_COL);
        DrawLine(v.x, v.y, v.x, uc->point.y, MAIN_COL);
    } else if (uc->deg > (180+low_offset) && uc->deg < (180+high_offset)) {
        Vector2 v = { uc->point.x + offset, uc->center.y + offset };
        DrawLine(v.x, v.y, uc->point.x, v.y, MAIN_COL);
        DrawLine(v.x, v.y, v.x, uc->center.y, MAIN_COL);
    } else if (uc->deg > (270+low_offset) && uc->deg < (270+high_offset)) {
        Vector2 v = { uc->center.x + offset, uc->point.y - offset };
        DrawLine(v.x, v.y, uc->center.x, v.y, MAIN_COL);
        DrawLine(v.x, v.y, v.x, uc->point.y, MAIN_COL);
    }
}

void unit_circle_draw_tan(UnitCircle *uc, Font *font) {
    Vector2 tan_outer_pos = {
        uc->point.x + (uc->tan * uc->radius * uc->sin),
        uc->center.y
    };
    DrawLineEx(uc->point, tan_outer_pos, LINE_BIG, TAN_COL);
    DrawCircleV(tan_outer_pos, POINT_RADIUS, TAN_COL);
    Vector2 text_pos;
    const float inner_padding = WINSIDE * 0.1;
    const float outer_padding = WINSIDE * 0.05;
    if (tan_outer_pos.x < outer_padding) {
        text_pos.x = outer_padding;
    } else if (
        tan_outer_pos.x > (uc->center.x - uc->radius - inner_padding) &&
        tan_outer_pos.x < uc->center.x
    ) {
        text_pos.x = (uc->center.x - uc->radius - inner_padding);
    } else if (
        tan_outer_pos.x > uc->center.x &&
        tan_outer_pos.x < (uc->center.x + uc->radius + inner_padding)
    ) {
        text_pos.x = (uc->center.x + uc->radius + inner_padding);
    } else if (tan_outer_pos.x > (WINSIDE - outer_padding)) {
        text_pos.x = (WINSIDE - outer_padding);
    } else {
        text_pos.x = tan_outer_pos.x;
    }
    const float vertical_padding = WINSIDE * 0.05;
    if (uc->deg <= 180) {
        text_pos.y = tan_outer_pos.y + vertical_padding;
    } else {
        text_pos.y = tan_outer_pos.y - vertical_padding;
    }
    draw_text_centered(
        font,
        TEXT_FLAG_NONE,
        text_pos,
        0,
        (uc->tan > -100) && (uc->tan < 100) ? TextFormat("%.2f", uc->tan) : "??",
        TAN_COL
    );
}