#include "./raylib/include/raylib.h"
#include "math.h"

#define WINSIDE 1000
#define CENTERX (WINX / 2)
#define CENTERY (WINY / 2)
#define CENTER ((Vector2){CENTERX,CENTERY})

#define LINE_BIG 5
#define LINE_SMALL 1

#define POINT_RADIUS 10

#define PRIMARY_COL ((Color){255,255,255,255})
#define SECONDARY_COL ((Color){255,255,255,128})
#define CIRCLE_FILL_COL ((Color){255,255,255,32})
#define SIN_COL ((Color){0,128,255,255})
#define COS_COL ((Color){200,0,255,255})
#define TAN_COL ((Color){255,128,0,255})

#define QUADRANT_I_OFFSET 0
#define QUADRANT_II_OFFSET (PI/2)
#define QUADRANT_III_OFFSET PI
#define QUADRANT_IV_OFFSET (PI*1.5)

#define VISUAL_RESOLUTION 20

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

typedef struct FunctionVisual {
    char name[4];
    float (*function)(float);
    Vector2 position;
    Vector2 size;
    int resolution;
    Color color;
} FunctionVisual;

void update_unit_circle_towards_position(UnitCircle *uc, Vector2 position) {
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

void update_unit_circle_radians(UnitCircle *uc, float rad) {
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

typedef enum TextFlags {
    TEXT_FLAG_NONE = 0,
    TEXT_FLAG_LARGE = 1 << 0,
    TEXT_FLAG_BACKING_RECTANGLE = 1 << 1,
} TextFlags;

static inline bool has_flag(int flags, int flag) {
    return (flags & flag) == flag;
}

void draw_text_centered(Font *font, TextFlags flags, Vector2 position, float rotation, char *text, Color color) {
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

bool is_point_inside_area(Vector2 area_pos, Vector2 area_size, Vector2 point_pos) {
    return (
        point_pos.x > area_pos.x &&
        point_pos.x < (area_pos.x + area_size.x) &&
        point_pos.y > area_pos.y &&
        point_pos.y < (area_pos.y + area_size.y)
    );
}

Vector2 get_angle_direction(float deg) {
    float rad = deg * DEG2RAD;
    return (Vector2) {
        cosf(rad),
        -sinf(rad)
    };
}

Vector2 vec2_in_direction(Vector2 position, Vector2 direction, float distance) {
    return (Vector2) {
        position.x + (direction.x * distance),
        position.y + (direction.y * distance),
    };
}

void draw_angles_on_unit_circle(Font *font, UnitCircle *uc, float *angles, int angle_count) {
    for (int i = 0; i < angle_count; i++) {
        float deg = angles[i];
        Vector2 dir = get_angle_direction(deg);
        DrawLineV(
            vec2_in_direction(uc->center, dir, 0.95f * uc->radius),
            vec2_in_direction(uc->center, dir, 1.05f * uc->radius),
            PRIMARY_COL
        );
        draw_text_centered(
            font,
            TEXT_FLAG_NONE,
            vec2_in_direction(uc->center, dir, 1.2f * uc->radius),
            (deg <= 90 || deg > 270) ? -deg : 180-deg,
            TextFormat("%.0f", deg),
            PRIMARY_COL
        );
    }
}

void draw_right_angle(UnitCircle *uc) {
    const float offset = uc->radius * 0.15;
    const float low_offset = 20;
    const float high_offset = 65;
    Vector2 right_angle_offset;
    if (uc->deg > low_offset && uc->deg < high_offset) {
        Vector2 v = { uc->point.x - offset, uc->center.y - offset };
        DrawLine(v.x, v.y, uc->point.x, v.y, PRIMARY_COL);
        DrawLine(v.x, v.y, v.x, uc->center.y, PRIMARY_COL);
    } else if (uc->deg > (low_offset+90) && uc->deg < (high_offset+90)) {
        Vector2 v = { uc->center.x - offset, uc->point.y + offset };
        DrawLine(v.x, v.y, uc->center.x, v.y, PRIMARY_COL);
        DrawLine(v.x, v.y, v.x, uc->point.y, PRIMARY_COL);
    } else if (uc->deg > (180+low_offset) && uc->deg < (180+high_offset)) {
        Vector2 v = { uc->point.x + offset, uc->center.y + offset };
        DrawLine(v.x, v.y, uc->point.x, v.y, PRIMARY_COL);
        DrawLine(v.x, v.y, v.x, uc->center.y, PRIMARY_COL);
    } else if (uc->deg > (270+low_offset) && uc->deg < (270+high_offset)) {
        Vector2 v = { uc->center.x + offset, uc->point.y - offset };
        DrawLine(v.x, v.y, uc->center.x, v.y, PRIMARY_COL);
        DrawLine(v.x, v.y, v.x, uc->point.y, PRIMARY_COL);
    }
}

int main(void) {
    InitWindow(WINSIDE, WINSIDE, "Trig");
    SetTargetFPS(60);

    Font font = LoadFont("arial.ttf");

    const int visual_angles_count = 16;
    float visual_angles[visual_angles_count];
    {
        for (int i = 0; i < 4; i++) {
            int idx = 4*i;
            float deg = 90*i;
            visual_angles[idx+0] = deg+30;
            visual_angles[idx+1] = deg+45;
            visual_angles[idx+2] = deg+60;
            visual_angles[idx+3] = deg+90;
        }
    }

    UnitCircle uc;
    uc.position = (Vector2){WINSIDE*0.3,WINSIDE*0.2};
    uc.radius = WINSIDE*0.2;
    uc.center = (Vector2){
        uc.position.x + uc.radius,
        uc.position.y + uc.radius
    };

    const int func_visuals_count = 3;
    FunctionVisual func_visuals[func_visuals_count];
    {
        float x = WINSIDE*0.1;
        float y = WINSIDE*0.8;
        float width = WINSIDE*0.6/3;
        float height = WINSIDE*0.1;
        int resolution = 32;

        func_visuals[0] = (FunctionVisual) {
            .name = "sin",
            .function = sinf,
            .position = (Vector2){x,y},
            .size = (Vector2){width,height},
            .resolution = resolution,
            .color = SIN_COL,
        };
        func_visuals[1] = (FunctionVisual) {
            .name = "cos",
            .function = cosf,
            .position = (Vector2){x+width+x,y},
            .size = (Vector2){width,height},
            .resolution = resolution,
            .color = COS_COL,
        };
        func_visuals[2] = (FunctionVisual) {
            .name = "tan",
            .function = tanf,
            .position = (Vector2){x+width+x+width+x,y},
            .size = (Vector2){width,height},
            .resolution = resolution,
            .color = TAN_COL,
        };
    }

    {
        Vector2 angle45 = {
            uc.center.x + 1,
            uc.center.y - 1,
        };
        update_unit_circle_towards_position(&uc, angle45);
    }

    while (!WindowShouldClose()) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetMousePosition();

            if (is_point_inside_area(uc.position, (Vector2){uc.radius*2,uc.radius*2}, mouse)) {
                update_unit_circle_towards_position(&uc, mouse);
            } else {
                for (int i = 0; i < func_visuals_count; i++) {
                    FunctionVisual *fv = &(func_visuals[i]);
                    if (is_point_inside_area(fv->position, fv->size, mouse)) {
                        float relative_x = (mouse.x - fv->position.x) / fv->size.x;
                        float rad = relative_x * 2 * PI;
                        update_unit_circle_radians(&uc, rad);
                        break;
                    }
                }
            }
        }

        BeginDrawing();

        ClearBackground(BLACK);

        DrawCircleLinesV(uc.center, uc.radius, PRIMARY_COL);
        DrawCircleSector(uc.center, uc.radius, 0, -uc.deg, uc.rad / 10, CIRCLE_FILL_COL);
        DrawCircleSectorLines(uc.center, uc.radius * 0.15 * 1.4, 0, -uc.deg, uc.rad / 10, PRIMARY_COL);

        { // TAN
            Vector2 tan_outer_pos = {
                uc.point.x + (uc.tan * uc.radius * uc.sin),
                uc.center.y
            };
            DrawLineEx(uc.point, tan_outer_pos, LINE_BIG, TAN_COL);
            DrawCircleV(tan_outer_pos, POINT_RADIUS, TAN_COL);
            Vector2 text_pos;
            const float inner_padding = WINSIDE * 0.1;
            const float outer_padding = WINSIDE * 0.05;
            if (tan_outer_pos.x < outer_padding) {
                text_pos.x = outer_padding;
            } else if (
                tan_outer_pos.x > (uc.center.x - uc.radius - inner_padding) &&
                tan_outer_pos.x < uc.center.x
            ) {
                text_pos.x = (uc.center.x - uc.radius - inner_padding);
            } else if (
                tan_outer_pos.x > uc.center.x &&
                tan_outer_pos.x < (uc.center.x + uc.radius + inner_padding)
            ) {
                text_pos.x = (uc.center.x + uc.radius + inner_padding);
            } else if (tan_outer_pos.x > (WINSIDE - outer_padding)) {
                text_pos.x = (WINSIDE - outer_padding);
            } else {
                text_pos.x = tan_outer_pos.x;
            }
            const float vertical_padding = WINSIDE * 0.05;
            if (uc.deg <= 180) {
                text_pos.y = tan_outer_pos.y + vertical_padding;
            } else {
                text_pos.y = tan_outer_pos.y - vertical_padding;
            }
            draw_text_centered(
                &font,
                TEXT_FLAG_NONE,
                text_pos,
                0,
                (uc.tan > -100) && (uc.tan < 100) ? TextFormat("%.2f", uc.tan) : "??",
                TAN_COL
            );
        }

        draw_angles_on_unit_circle(&font, &uc, visual_angles, visual_angles_count);
        draw_right_angle(&uc);

        DrawLine(
            uc.position.x,
            uc.center.y,
            uc.position.x + (uc.radius*2),
            uc.center.y,
            PRIMARY_COL
        );
        DrawLine(
            uc.center.x,
            uc.position.y,
            uc.center.x,
            uc.position.y + (uc.radius*2),
            PRIMARY_COL
        );

        Vector2 sin_corner = {uc.center.x, uc.point.y};
        Vector2 cos_corner = {uc.point.x, uc.center.y};

        DrawLineEx(sin_corner, uc.point, LINE_SMALL, SIN_COL);
        DrawLineEx(cos_corner, uc.point, LINE_SMALL, COS_COL);

        DrawLineEx(uc.center, uc.point, LINE_BIG, PRIMARY_COL);
        DrawCircleV(uc.point, POINT_RADIUS, PRIMARY_COL);

        DrawLineEx(uc.center, sin_corner, LINE_BIG, SIN_COL);
        DrawCircleV(sin_corner, POINT_RADIUS, SIN_COL);

        DrawLineEx(uc.center, cos_corner, LINE_BIG, COS_COL);
        DrawCircleV(cos_corner, POINT_RADIUS, COS_COL);

        {
            float offset = (WINSIDE * 0.05);
            float x = uc.position.x - offset;
            float y = uc.position.y - offset;
            float o = (uc.radius * 2) + (offset * 2);
            draw_text_centered(&font, TEXT_FLAG_LARGE, (Vector2){x+o, y   }, 0, "I", PRIMARY_COL);
            draw_text_centered(&font, TEXT_FLAG_LARGE, (Vector2){x  , y   }, 0, "II", PRIMARY_COL);
            draw_text_centered(&font, TEXT_FLAG_LARGE, (Vector2){x  , y+o }, 0, "III", PRIMARY_COL);
            draw_text_centered(&font, TEXT_FLAG_LARGE, (Vector2){x+o, y+o }, 0, "IV", PRIMARY_COL);
        }

        for (int i = 0; i < func_visuals_count; i++) {
            FunctionVisual *fv = &(func_visuals[i]);
            DrawLine(
                fv->position.x, fv->position.y + (fv->size.y/2),
                fv->position.x + fv->size.x, fv->position.y + (fv->size.y/2),
                WHITE
            );
            const int vertical_line_count = 5;
            for (int j = 0; j < vertical_line_count; j++) {
                const float fract = (fv->size.x/(vertical_line_count-1));
                float x = fv->position.x + (fract * j);
                DrawLine(
                    x, fv->position.y,
                    x, fv->position.y + (fv->size.y),
                    WHITE
                );
                Vector2 text_position = {x, fv->position.y - WINSIDE * 0.05};
                float text_rotation = 315;
                switch (j) {
                case 0: draw_text_centered(&font, TEXT_FLAG_NONE, text_position, text_rotation, "0", PRIMARY_COL); break;
                case 1: draw_text_centered(&font, TEXT_FLAG_NONE, text_position, text_rotation, "pi/2", PRIMARY_COL); break;
                case 2: draw_text_centered(&font, TEXT_FLAG_NONE, text_position, text_rotation, "pi", PRIMARY_COL); break;
                case 3: draw_text_centered(&font, TEXT_FLAG_NONE, text_position, text_rotation, "3pi/2", PRIMARY_COL); break;
                case 4: draw_text_centered(&font, TEXT_FLAG_NONE, text_position, text_rotation, "2pi", PRIMARY_COL); break;
                }
            }
            float x_fract = fv->size.x / fv->resolution;
            float y_fract = PI*2/fv->resolution;
            float half_height = (fv->size.y/2);
            Vector2 prev = {
                fv->position.x,
                fv->position.y - (fv->function(0) * half_height) + half_height,
            };
            float func_min = fv->position.y;
            float func_max = (fv->position.y + fv->size.y);
            for (int j = 0; j <= fv->resolution; j++) {
                Vector2 next = {
                    fv->position.x + x_fract*j,
                    fv->position.y - (fv->function(y_fract*j) * half_height) + half_height,
                };
                if (prev.y >= func_min &&
                    prev.y <= func_max &&
                    next.y >= func_min &&
                    next.y <= func_max
                ) {
                    DrawLineEx(prev, next, LINE_BIG, fv->color);
                }
                prev = next;
            }

            float current_rad_result = fv->function(uc.rad);
            bool inside_bounds = (current_rad_result <= 1) && (current_rad_result >= -1);
            Vector2 func_pos;
            func_pos.x = fv->position.x + (fv->size.x) / PI / 2 * uc.rad;
            if (inside_bounds) {
                func_pos.y = fv->position.y - (current_rad_result * half_height) + half_height;
                DrawCircleV(func_pos, POINT_RADIUS, PRIMARY_COL);
            } else {
                if (current_rad_result > 1) {
                    func_pos.y = fv->position.y;
                } else {
                    func_pos.y = fv->position.y + fv->size.y;
                }
            }
            DrawLine(
                fv->position.x, func_pos.y,
                func_pos.x, func_pos.y,
                PRIMARY_COL
            );
            bool gigantic = !inside_bounds && current_rad_result <= -100 || current_rad_result >= 100;
            draw_text_centered(
                &font,
                TEXT_FLAG_NONE,
                (Vector2){fv->position.x - (WINSIDE * 0.05), func_pos.y},
                0,
                !gigantic ? TextFormat("%.2f", current_rad_result) : "??",
                fv->color
            );
            draw_text_centered(
                &font,
                TEXT_FLAG_LARGE,
                (Vector2){fv->position.x + (fv->size.x/2), fv->position.y + (fv->size.y) + WINSIDE * 0.05},
                0,
                fv->name,
                fv->color
            );
        }

        draw_text_centered(&font, TEXT_FLAG_NONE, (Vector2){(WINSIDE/2)-(WINSIDE*0.25),WINSIDE*0.05}, 0, TextFormat("deg: %.2f", uc.deg), PRIMARY_COL);
        draw_text_centered(&font, TEXT_FLAG_NONE, (Vector2){(WINSIDE/2)+(WINSIDE*0.25),WINSIDE*0.05}, 0, TextFormat("rad: %.2f", uc.rad), PRIMARY_COL);

        const float trig_func_text_offset = 0.05f;

        Vector2 sin_text_position = {
            (uc.cos < 0) ? uc.center.x + (WINSIDE*trig_func_text_offset) : uc.center.x - (WINSIDE*trig_func_text_offset),
            uc.center.y - (uc.sin * uc.radius),
        };
        draw_text_centered(&font, TEXT_FLAG_BACKING_RECTANGLE, sin_text_position, 0, TextFormat("%.2f", uc.sin), SIN_COL);
        Vector2 cos_text_position = {
            uc.center.x + (uc.cos * uc.radius),
            (uc.sin < 0) ? uc.center.y - (WINSIDE*trig_func_text_offset) : uc.center.y + (WINSIDE*trig_func_text_offset),
        };
        draw_text_centered(&font, TEXT_FLAG_BACKING_RECTANGLE, cos_text_position, 0, TextFormat("%.2f", uc.cos), COS_COL);

        EndDrawing();
    }
}
