/*
    Bezier test 2 - make a single control point bezier

    Works ok, some things you need to fix

        1. reset all of the values when going out of moving mode so you don't get weird behaviour if you dont change the points - done
        2. figure out how to trace the path of the moving point - maybe check out the example where you can draw things, see how they did that
            - done, just draw to a texture so that it gets preserved then draw out the texture to get your curve
        3. sometimes the moving point goes past the end point - how to make sure that it stops at the end point?
			something you could do is that when the moving point is within some small distance from endPt (less than deltaX/Y), force it to endPt, then stop motion
*/

#include <math.h>
#include <stdbool.h>

#include "raylib.h"

#define MOVE_STEPS 120
#define START_PT 0
#define END_PT 2

void initialize(void);
void update(void);
void draw(void);

const int screenWidth = 800;
const int screenHeight = 450;

// SE = start end, C = control
typedef enum State {
    PLACING_SE,
    PLACING_C,
    MOVING,
    RESULT
} State;

typedef struct CtrlPoint {
    Vector2 position;
    Vector2 target;
    Vector2 delta;
} CtrlPoint;

Vector2 movingPt = (Vector2) {0, 0};

// 0 = startPt, 1 = control point, 2 = end point
Vector2 targetPts[3];

// Moving control points, 1 is from start to control, 2 is from control to end
CtrlPoint movingControlPts[2];

RenderTexture2D target;

State gameState = PLACING_SE;

int main(void) 
{
    initialize();

    while (!WindowShouldClose())
    {
        update();

        draw();
    }

    CloseWindow();

    return 0;
}

void initialize(void) {
    InitWindow(screenWidth, screenHeight, "raylib");
    SetTargetFPS(60);

    for (int i = 0; i < 2; i++) {
        movingControlPts[i].position = (Vector2) {0, 0};

        // 0-> 1, 1-> 2, so first moving control point target is control point, and second moving control point target is end point
        movingControlPts[i].target = targetPts[i + 1];
    }

    target = LoadRenderTexture(screenWidth, screenHeight);

    BeginTextureMode(target);
    ClearBackground(RAYWHITE);
    EndTextureMode();
}

void update(void) {

    switch (gameState) {
        case PLACING_SE:

            movingPt = targetPts[START_PT];
            movingControlPts[0].position = targetPts[START_PT];


            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                targetPts[START_PT] = GetMousePosition();
            }
            else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                targetPts[END_PT] = GetMousePosition();
                movingControlPts[1].target = targetPts[END_PT];
            }

            if (IsKeyPressed(KEY_TAB)) {
                gameState = PLACING_C;
            }

            break;

        case PLACING_C:

            movingControlPts[1].position = targetPts[1];

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                targetPts[1] = GetMousePosition();
                movingControlPts[0].target = targetPts[1];
            }

            if (IsKeyPressed(KEY_TAB)) {
                gameState = PLACING_SE;
            }
            else if (IsKeyPressed(KEY_ENTER)) {
                gameState = MOVING;

                for (int i = 0; i < 2; i++) {
                    movingControlPts[i].delta.x = movingControlPts[i].target.x - movingControlPts[i].position.x;
                    movingControlPts[i].delta.y = movingControlPts[i].target.y - movingControlPts[i].position.y;
                }
            }

            break;

        case MOVING:
        
            bool ctrlMoving = true;

            for (int i = 0; i < 2; i++) {

                if (abs(movingControlPts[i].target.x - movingControlPts[i].position.x) > 0) {
                    movingControlPts[i].position.x += movingControlPts[i].delta.x / MOVE_STEPS;
                    movingControlPts[i].position.y += movingControlPts[i].delta.y / MOVE_STEPS;
                }
                else {
                    if (i == 1) {
                        ctrlMoving = false;
                    }
                }
            }

            if (ctrlMoving) {
                // Not sure why it works when you divide the deltas by 60, or MOVE_STEPS / 2
                float deltaX = (movingControlPts[1].position.x - movingControlPts[0].position.x) / 60;
                float deltaY = (movingControlPts[1].position.y - movingControlPts[0].position.y) / 60;

                movingPt.x +=  deltaX;
                movingPt.y += deltaY;

                BeginTextureMode(target);

                    // DrawCircleV(movingPt, 10, GRAY);
                    if ((movingPt.x != targetPts[START_PT].x) && (movingPt.y != targetPts[START_PT].y)) {
                        DrawLine(movingPt.x - deltaX, movingPt.y - deltaY, movingPt.x, movingPt.y, BLACK);
                    }
                    

                EndTextureMode();
            }

            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_TAB)) {
                gameState = PLACING_SE;
            }
            else if (IsKeyPressed(KEY_R)) {
                gameState = RESULT;
            }

            break;

        case RESULT:
            if (IsKeyPressed(KEY_TAB)) {
                gameState = PLACING_SE;

                BeginTextureMode(target);

                    ClearBackground(RAYWHITE);

                EndTextureMode();
            }
    }

}

void draw(void) {
    BeginDrawing();

        ClearBackground(RAYWHITE);

        switch (gameState) {
            case PLACING_SE:
                DrawText("SE", 0, 0, 20, BLACK);
                break;
            case PLACING_C:
                DrawText("C", 0, 0, 20, BLACK);
                break;
            case MOVING:
                DrawText("M", 0, 0, 20, BLACK);
                break;
            case RESULT:
                DrawText("R", 0, 0, 20, BLACK);
                break;
        }

        if (gameState != RESULT) {
            for (int i = 0; i < 3; i++) {
                switch (i) {
                    case START_PT:
                        DrawCircleV(targetPts[START_PT], 10, GREEN);
                        break;
                    case END_PT:
                        DrawCircleV(targetPts[END_PT], 10, RED);
                        break;
                    default:
                        DrawCircleV(targetPts[i], 10, BLUE);
                }
            }

            if (gameState == MOVING) {
                for (int i = 0; i < 2; i++) {
                    DrawCircleV(movingControlPts[i].position, 5, PURPLE);
                }
                
                DrawCircleV(movingPt, 10, BLACK);
            }
        }
        else {
            DrawTextureRec(target.texture, (Rectangle){0, 0, target.texture.width, -target.texture.height }, (Vector2) {0, 0}, WHITE);
        }

    EndDrawing();
}