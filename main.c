#include "./raylib/include/raylib.h"
#include "math.h"

#define WINX 1000
#define WINY 1000
#define CENTERX (WINX / 2)
#define CENTERY (WINY / 2)
#define CENTER ((Vector2){CENTERX,CENTERY})

#define LINE_BIG 5
#define LINE_SMALL 1

#define POINT_RADIUS 10
#define ANGLE_FILL_RADIUS 50

#define PRIMARY_COL ((Color){255,255,255,255})
#define SECONDARY_COL ((Color){255,255,255,128})
#define CIRCLE_FILL_COL ((Color){255,255,255,32})
#define ANGLE_FILL_COL ((Color){255,255,255,64})
#define SIN_COL RED
#define COS_COL YELLOW

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
    Vector2 point;
} UnitCircle;

typedef struct FunctionVisual {
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
    uc->cos = cos(rad);
    uc->sin = sin(rad);

    uc->point = (Vector2) {
        uc->center.x + (uc->cos * uc->radius),
        uc->center.y - (uc->sin * uc->radius),
    };

    uc->deg = uc->rad * RAD2DEG;
}

typedef enum TextFlags {
    TEXT_FLAG_NONE = 0,
    TEXT_FLAG_BACKING_RECTANGLE = 1 << 0,
} TextFlags;

static inline bool has_flag(int flags, int flag) {
    return (flags & flag) == flag;
}

void draw_text_centered(Font *font, TextFlags flags, Vector2 position, float rotation, char *text, Color color) {
    int size = 30;
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

int main(void) {
    InitWindow(WINX, WINY, "Trig");

    Font font = LoadFont("arial.ttf");

    UnitCircle uc;
    uc.position = (Vector2){WINX*0.25,WINY*0.1};
    uc.radius = WINX*0.25;
    uc.center = (Vector2){
        uc.position.x + uc.radius,
        uc.position.y + uc.radius
    };

    const int func_visuals_count = 2;
    FunctionVisual func_visuals[func_visuals_count];
    {
        func_visuals[0] = (FunctionVisual) {
            .function = sinf,
            .position = (Vector2){WINX*0.1,WINY*0.75},
            .size = (Vector2){WINX*0.3,WINY*0.1},
            .resolution = 40,
            .color = SIN_COL,
        };
        func_visuals[1] = (FunctionVisual) {
            .function = cosf,
            .position = (Vector2){(WINX/2)+func_visuals[0].position.x,func_visuals[0].position.y},
            .size = func_visuals[0].size,
            .resolution = func_visuals[0].resolution,
            .color = COS_COL,
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
        DrawCircleSectorLines(uc.center, ANGLE_FILL_RADIUS, 0, -uc.deg, uc.rad / 10, ANGLE_FILL_COL);

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
        DrawLineEx(uc.center, sin_corner, LINE_BIG, SIN_COL);
        DrawLineEx(sin_corner, uc.point, LINE_SMALL, SIN_COL);
        DrawCircleV(sin_corner, POINT_RADIUS, SIN_COL);

        Vector2 cos_corner = {uc.point.x, uc.center.y};
        DrawLineEx(uc.center, cos_corner, LINE_BIG, COS_COL);
        DrawLineEx(cos_corner, uc.point, LINE_SMALL, COS_COL);
        DrawCircleV(cos_corner, POINT_RADIUS, COS_COL);

        DrawLineEx(uc.center, uc.point, LINE_BIG, PRIMARY_COL);
        DrawCircleV(uc.point, POINT_RADIUS, PRIMARY_COL);

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
                Vector2 text_position = {x, fv->position.y - WINY * 0.05};
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
            for (int j = 0; j <= fv->resolution; j++) {
                Vector2 next = {
                    fv->position.x + x_fract*j,
                    fv->position.y - (fv->function(y_fract*j) * half_height) + half_height,
                };
                DrawLineEx(prev, next, LINE_BIG, fv->color);
                prev = next;
            }

            Vector2 func_y_pos = {
                fv->position.x + (fv->size.x) / PI / 2 * uc.rad,
                fv->position.y - (fv->function(uc.rad) * half_height) + half_height,
            };
            draw_text_centered(
                &font,
                TEXT_FLAG_NONE,
                (Vector2){fv->position.x - (WINX * 0.05), func_y_pos.y},
                0,
                TextFormat("%.2f", uc.sin),
                fv->color
            );
            DrawLine(
                fv->position.x, func_y_pos.y,
                func_y_pos.x, func_y_pos.y,
                PRIMARY_COL
            );
            DrawCircleV(func_y_pos, POINT_RADIUS, PRIMARY_COL);
        }

        draw_text_centered(&font, TEXT_FLAG_NONE, (Vector2){(WINX/2)-(WINX*0.25),WINY*0.05}, 0, TextFormat("deg: %.2f", uc.deg), PRIMARY_COL);
        draw_text_centered(&font, TEXT_FLAG_NONE, (Vector2){(WINX/2)+(WINX*0.25),WINY*0.05}, 0, TextFormat("rad: %.2f", uc.rad), PRIMARY_COL);

        const float trig_func_text_offset = 0.05f;

        Vector2 sin_text_position = {
            (uc.cos < 0) ? uc.center.x + (WINX*trig_func_text_offset) : uc.center.x - (WINX*trig_func_text_offset),
            uc.center.y - (uc.sin * uc.radius),
        };
        draw_text_centered(&font, TEXT_FLAG_NONE, sin_text_position, 0, TextFormat("%.2f", uc.sin), SIN_COL);
        Vector2 cos_text_position = {
            uc.center.x + (uc.cos * uc.radius),
            (uc.sin < 0) ? uc.center.y - (WINY*trig_func_text_offset) : uc.center.y + (WINY*trig_func_text_offset),
        };
        draw_text_centered(&font, TEXT_FLAG_NONE, cos_text_position, 0, TextFormat("%.2f", uc.cos), COS_COL);

        EndDrawing();
    }
}
