#include "raylib.h"
#include <math.h>
#include <stdbool.h>

void initialize();
void update();
void draw();
void getVelocities(int index);

typedef struct Bullet {
    double centreX;
    double centreY;
    Vector2 velocity;
    bool isActive;
} Bullet;

#define MAX_BULLETS 500
#define BULLET_VELOCITY 5
#define RADIUS 50

const int screenWidth = 800;
const int screenHeight = 450;

double x;
double y;
double angle;
double radianAngle;
double angleChange = 10;
int frameCounter;
int curBullet;
int shootRate = 4;
int numBullets = 0;

Bullet bullets[MAX_BULLETS];

int main() 
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

void initialize() {
    InitWindow(screenWidth, screenHeight, "raylib");
    SetTargetFPS(60);
    
    angle = 1;

    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].centreX = screenWidth / 2;
        bullets[i].centreY = screenHeight / 2;
        bullets[i].isActive = false;
        bullets[i].velocity = (Vector2) {0, 0};
    }

    frameCounter = 0;
    curBullet = 0;
}

void update() {
    if (IsKeyPressed(KEY_A)) { angleChange++; }
    if (IsKeyPressed(KEY_D)) { angleChange--; }
    if (angleChange < 1) { angleChange = 1; }
    if (IsKeyPressed(KEY_R)) { angleChange = 1; }

    if (IsKeyPressed(KEY_UP)) { shootRate++; }
    if (IsKeyPressed(KEY_DOWN)) { shootRate--; }
    if (shootRate < 1) { shootRate = 1; }

    // Getting point on orbit
    radianAngle = angle * (PI / 180.0);

    x = RADIUS * cos(radianAngle);
    y = RADIUS * sin(radianAngle);

    x += screenWidth / 2;
    y += screenHeight / 2;

    angle += angleChange;
    if (angle > 360) { angle = angle - 360; }

    angleChange += 0.025;

    /*
        minor changes:
        first, the double shot was because the angle went from 360 to 0 degrees, which is the same angle, so the two shots at that border would be in the same direction, hence double shot
        next, originally if the angle went above 360, then i just set it back to 1, but that made a boring pattern where the arms would fold in at 0 and you would get a straight line forever
            this was fixed by setting the angle to : angle - 360, which is basically whatever is the remainder after 360, so if the angle wass 357 and our angle change is +14, then the angle would update to 
            357 + 14 -> 371, then we would subtract 360 to get 11 degrees, and this is where the angle would be after passing 360
            then we can get the cool patterns - with the infinitely accelerating orbiter, then we can get the border of wave and particle pattern
    */

    // shoot bullet
    frameCounter++;
    if (frameCounter > 60) { frameCounter = 1; }

    // note: if you want to shoot multiple times per frame, use a for loop [do the things inside this if statement, incrament angle and do it again]
    if (frameCounter % shootRate == 0) {
        getVelocities(curBullet);
        bullets[curBullet].isActive = true;

        curBullet++;
        if (curBullet > MAX_BULLETS) { curBullet = 0; }
        numBullets++;
    }

    // update position
    for (int i = 0; i < MAX_BULLETS; i++) {
        // If bullet active, make it move
        if (bullets[i].isActive) {
            bullets[i].centreX += bullets[i].velocity.x;
            bullets[i].centreY += bullets[i].velocity.y;
        }

        // If bullet at edge, make it stop moving and move it back to the emitter
        if (bullets[i].centreX >= screenWidth || bullets[i].centreX < 0 ||
            bullets[i].centreY >= screenHeight || bullets[i].centreY < 0) {
            
                bullets[i].centreX = screenWidth / 2;
                bullets[i].centreY = screenHeight / 2;
                bullets[i].velocity = (Vector2) {0, 0};
                bullets[i].isActive = false;
                numBullets--;
        }
    }
    
}

// Your basic similar triangle velocity method
void getVelocities(int index) {
    double deltaX = x - (screenWidth / 2);
    double deltaY = y - (screenHeight / 2);

    double ratio = BULLET_VELOCITY / sqrt((deltaX * deltaX) + (deltaY * deltaY));

    bullets[index].velocity.x = deltaX * ratio;
    bullets[index].velocity.y = deltaY * ratio;
}

void draw() {
    BeginDrawing();

        ClearBackground(RAYWHITE);

        // The emitter
        DrawCircle(screenWidth / 2, screenHeight / 2, 5, BLACK);

        // The orbiter
        DrawCircle(x, y, 5, YELLOW);

        // Draw all active bullets
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].isActive) {
                DrawCircle(bullets[i].centreX, bullets[i].centreY, 5, BLACK);
            }
        }

        DrawText(TextFormat("%d", numBullets), 50, 50, 20, BLACK);

    EndDrawing();
}
