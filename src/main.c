#include "main.h"
#include "unit_circle.c"
#include "trigonometric_function.c"

int main(void) {
    InitWindow(WINSIDE, WINSIDE, "Trig");
    SetTargetFPS(60);

    Font font = LoadFont("arial.ttf");

    UnitCircle unit_circle;
    unit_circle.position = (Vector2){WINSIDE*0.3,WINSIDE*0.2};
    unit_circle.radius = WINSIDE*0.2;
    unit_circle.center = (Vector2){
        unit_circle.position.x + unit_circle.radius,
        unit_circle.position.y + unit_circle.radius
    };

    const int trigonometric_functions_count = 3;
    TrigonometricFunction trigonometric_functions[trigonometric_functions_count];
    {
        float x = WINSIDE*0.1;
        float y = WINSIDE*0.8;
        float width = WINSIDE*0.6/3;
        float height = WINSIDE*0.1;

        trigonometric_functions[0] = (TrigonometricFunction) {
            .name = "sin",
            .function = sinf,
            .range = (Range) {-1,1},
            .position = (Vector2){x,y},
            .size = (Vector2){width,height},
            .color = SIN_COL,
        };
        trigonometric_functions[1] = (TrigonometricFunction) {
            .name = "cos",
            .function = cosf,
            .range = (Range) {-1,1},
            .position = (Vector2){x+width+x,y},
            .size = (Vector2){width,height},
            .color = COS_COL,
        };
        trigonometric_functions[2] = (TrigonometricFunction) {
            .name = "tan",
            .function = tanf,
            .range = (Range) {-5,5},
            .position = (Vector2){x+width+x+width+x,y},
            .size = (Vector2){width,height},
            .color = TAN_COL,
        };
    }

    const int significant_angles_count = 16;
    float significant_angles[significant_angles_count];
    {
        for (int i = 0; i < 4; i++) {
            int idx = 4*i;
            float deg = 90*i;
            significant_angles[idx+0] = deg+30;
            significant_angles[idx+1] = deg+45;
            significant_angles[idx+2] = deg+60;
            significant_angles[idx+3] = deg+90;
        }
    }

    { // Unit circle initial angle
        Vector2 angle45 = {
            unit_circle.center.x + 1,
            unit_circle.center.y - 1,
        };
        unit_circle_update_towards(&unit_circle, angle45);
    }

    while (!WindowShouldClose()) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetMousePosition();

            if (is_point_inside_area(unit_circle.position, (Vector2){unit_circle.radius*2,unit_circle.radius*2}, mouse)) {
                unit_circle_update_towards(&unit_circle, mouse);
            } else {
                for (int i = 0; i < trigonometric_functions_count; i++) {
                    TrigonometricFunction *fv = &(trigonometric_functions[i]);
                    if (is_point_inside_area(fv->position, fv->size, mouse)) {
                        float relative_x = (mouse.x - fv->position.x) / fv->size.x;
                        float rad = relative_x * 2 * PI;
                        unit_circle_update_radians(&unit_circle, rad);
                        break;
                    }
                }
            }
        }

        BeginDrawing();

        ClearBackground(BLACK);

        unit_circle_draw_tan(&unit_circle, &font); // drawn early to not block texts outside of unit circle
        unit_circle_draw_base(&unit_circle);
        unit_circle_draw_quadrants(&unit_circle, &font);
        unit_circle_draw_angles_on_circumference(&unit_circle, &font, significant_angles, significant_angles_count);
        unit_circle_draw_right_angle(&unit_circle);
        unit_circle_draw_triangle(&unit_circle, &font);

        for (int i = 0; i < trigonometric_functions_count; i++) {
            trigonometric_function_draw(&(trigonometric_functions[i]), &font, unit_circle.rad);
        }

        draw_text_centered(&font, TEXT_FLAG_NONE, (Vector2){(WINSIDE/2)-(WINSIDE*0.25),WINSIDE*0.05}, 0, TextFormat("deg: %.2f", unit_circle.deg), MAIN_COL);
        draw_text_centered(&font, TEXT_FLAG_NONE, (Vector2){(WINSIDE/2)+(WINSIDE*0.25),WINSIDE*0.05}, 0, TextFormat("rad: %.2f", unit_circle.rad), MAIN_COL);

        EndDrawing();
    }
}
