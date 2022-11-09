/*
An updated version of "projectilePattern.c"
Now can have multiple target points and rotation is based on linear algebra.
*/

#include <math.h>
#include <stdbool.h>
#include "raylib.h"

void init(void);
void update(void);
void draw(void);
void setTargets(void);
void getVelocities(int bulIndex, int targIndex);
void resetBullet(int index);

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define NUM_TARGETS 5
#define MAX_BULLETS 1200
#define MAX_VELOCITY 5

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool isActive;
} Bullet;

Vector2 targets[NUM_TARGETS];
Bullet bullets[MAX_BULLETS];

// curBullet is the latest bullet to be shot. 
// You can guarantee* that the next NUM_TARGET bullets after curBullet are free.
int curBullet;

float rotationAngle = 0;
float deltaRAngle = 0.225f;

int main() {

    init();

    // Main game loop
    while (!WindowShouldClose())
    {
        update();
        draw();
    }

    CloseWindow();

    return 0;
}

void init(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib");
    SetTargetFPS(60);

    setTargets();

    for (int i = 0; i < MAX_BULLETS; i++) {
        resetBullet(i);
    }
}

void update(void) {

    // Update bullets ====================================================
    for (int i = 0; i < MAX_BULLETS; i++) {
        // Updating position, if the bullet is active
        if (bullets[i].isActive) {
            bullets[i].position.x += bullets[i].velocity.x;
            bullets[i].position.y += bullets[i].velocity.y;
        }

        // Test for out-of-bounds bullets
        if (bullets[i].position.x > SCREEN_WIDTH / 2 || bullets[i].position.x < -SCREEN_WIDTH / 2) {
            resetBullet(i);
        }
        else if (bullets[i].position.y > SCREEN_HEIGHT / 2 || bullets[i].position.y < -SCREEN_HEIGHT / 2) {
            resetBullet(i);
        }
    }

    // Shoot the bullet(s) (lol)
    for (int i = 0; i < NUM_TARGETS; i++) {

        bullets[curBullet].isActive = true;
        
        // This could probably be rewritten where getVelocities returns a Vector2, but whatever 
        getVelocities(curBullet, i);
        
        bullets[curBullet].position = targets[i];

        curBullet++;

        // Absolutely nooooooooo seg faults here
        // ... > MAX_BULLETS, wow
        if (curBullet >= MAX_BULLETS) {
            curBullet = 0;
        }
    }

    // Update targets ====================================================
    for (int i = 0; i < NUM_TARGETS; i++) {
        // This is just an expanded rotation matrix multiplication
        float xComponent = (targets[i].x * cos(rotationAngle * DEG2RAD)) - 
                           (targets[i].y * sin(rotationAngle * DEG2RAD));
        float yComponent = (targets[i].x * sin(rotationAngle * DEG2RAD)) +
                           (targets[i].y * cos(rotationAngle * DEG2RAD));
        targets[i] = (Vector2) {xComponent, yComponent};
    }

    rotationAngle += deltaRAngle;
}

void draw(void) {
    BeginDrawing();

        ClearBackground(BLACK);

        /*
        Previously, after some time the bullets would all move to the top left corner for some reason.
        I did not want to figure out what the problem was, so I just shifted the bullets at the output stage.
        */

        for (int i = 0; i < NUM_TARGETS; i++) {
            DrawCircle(targets[i].x + SCREEN_WIDTH / 2, targets[i].y + SCREEN_HEIGHT / 2, 5, YELLOW);
        }

        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].isActive) {
                Vector2 tempPos = (Vector2) {bullets[i].position.x + SCREEN_WIDTH / 2, bullets[i].position.y + SCREEN_HEIGHT / 2};
                DrawCircleV(tempPos, 5, PURPLE);
            }
        }

    EndDrawing();
}

// Just your classic "n circular points made from rotating [1; 0] with a rotation matrix"
void setTargets(void) {
    float deltaAngle = 360 / NUM_TARGETS;

    for (int i = 0; i < NUM_TARGETS; i++) {
        float angle = (deltaAngle * i) * DEG2RAD;
        
        targets[i].x = cos(angle);
        targets[i].y = sin(angle);
    }
}

// Just your classic similar triangles problem
void getVelocities(int bulIndex, int targIndex) {
    float deltaX = targets[targIndex].x;
    float deltaY = targets[targIndex].y;

    float hyp = sqrt( pow(deltaX, 2) + pow(deltaY, 2));

    bullets[bulIndex].velocity.x = MAX_VELOCITY * (deltaX / hyp);
    bullets[bulIndex].velocity.y = MAX_VELOCITY * (deltaY / hyp);
}

void resetBullet(int index) {
    bullets[index].position = (Vector2) {0, 0};
    bullets[index].velocity = (Vector2) {0, 0};
    bullets[index].isActive = false;
}
