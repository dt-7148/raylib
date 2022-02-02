/*******************************************************************************************
*
*   raylib [core] example - Basic 3d example
*
*   Welcome to raylib!
*
*   To compile example, just press F5.
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2020 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include <math.h>
#include <stdbool.h>

void initialize();
void update();
void getVelocities(int index);
void draw();

typedef struct Player {
    Rectangle rect;
    Vector2 velocity;
    Color color;
} Player;

typedef struct Bullet {
    Rectangle rect;
    Color color;
    Vector2 velocity;
    bool isActive;
} Bullet;

#define MAX_BULLETS 50
#define BULLET_VELOCITY 25

Player player;
Bullet bullets[MAX_BULLETS];

int frameCounter = 0;
int curBullet = 0;

const int screenWidth = 800;
const int screenHeight = 450;

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

    // Initialize player
    player.rect.width = 40;
    player.rect.height = 40;
    player.rect.x = screenWidth / 2 - (player.rect.width / 2);
    player.rect.y = screenHeight / 2 - (player.rect.height / 2);
    player.velocity = (Vector2) {5, 5};
    player.color = BLACK;

    // Initialize bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].rect.width = 10;
        bullets[i].rect.height = 10;
        bullets[i].rect.x = player.rect.x + (player.rect.width / 2);
        bullets[i].rect.y = player.rect.y + (player.rect.height / 2);

        bullets[i].color = YELLOW;

        bullets[i].velocity = (Vector2) {0, 0};
        bullets[i].isActive = false;
    }
}

void update() {
    
    // Player movement
    if (IsKeyDown(KEY_W)) { player.rect.y -= player.velocity.y; }
    if (IsKeyDown(KEY_S)) { player.rect.y += player.velocity.y; }
    if (IsKeyDown(KEY_A)) { player.rect.x -= player.velocity.x; }
    if (IsKeyDown(KEY_D)) { player.rect.x += player.velocity.x; }

    // Wall behaviour
    if (player.rect.x <= 0) { player.rect.x = 0; }
    if (player.rect.x + player.rect.width >= screenWidth) { player.rect.x = screenWidth - player.rect.width; }
    if (player.rect.y <= 0) { player.rect.y = 0; }
    if (player.rect.y + player.rect.height >= screenHeight) { player.rect.y = screenHeight - player.rect.height; }

    if (IsMouseButtonDown(0)) {
        /*
            basically what you are doing here and with teh projectile array is working around not being able to create new objects
            the original thought was that every time you shot, then you would create a new projectile, then when it reached the end of the screen, you would destroy it
            but that would need a linked list and as you know, linked lists are actual pain to get working
            so instead, we just have an array that will hold all of the projectiles we will ever need - we dont need to dynamically create a structure that adds and removes bullets to be drawn
                then when we want to shoot, we "create" a new bullet by making it active and giving it a velocity
                and when it reaches the end of the screen, we "destroy" it by making it inactive

                this is why the order that we get it doesnt matter - the array is just a store of the bullets we would need
                activiating bullets sequentially is just done because its more organized than randomly choosing an index
        */

        frameCounter++;

        if (frameCounter > 60) { frameCounter = 1; }

        if (frameCounter % 4 == 0) {
            getVelocities(curBullet);
            bullets[curBullet].isActive = true;

            curBullet++;

            if (curBullet == MAX_BULLETS) { curBullet = 0; }
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].isActive) {
            bullets[i].rect.x += bullets[i].velocity.x;
            bullets[i].rect.y += bullets[i].velocity.y;
        }

        if (!bullets[i].isActive) {
            bullets[i].rect.x = player.rect.x + (player.rect.width / 2);
            bullets[i].rect.y = player.rect.y + (player.rect.height / 2);
        }

        if (bullets[i].rect.x <= 0 || bullets[i].rect.x + bullets[i].rect.width >= screenWidth) {
            bullets[i].rect.x = player.rect.x + (player.rect.width / 2);
            bullets[i].rect.y = player.rect.y + (player.rect.height / 2);
            bullets[i].velocity = (Vector2) {0, 0};
            bullets[i].isActive = false;
        }
        if (bullets[i].rect.y <= 0 || bullets[i].rect.y + bullets[i].rect.height >= screenHeight) {
            bullets[i].rect.x = player.rect.x + (player.rect.width / 2);
            bullets[i].rect.y = player.rect.y + (player.rect.height / 2);
            bullets[i].velocity = (Vector2) {0, 0};
            bullets[i].isActive = false;
        }
    } 

}

void getVelocities(int index) {
    double deltaX = GetMouseX() - (player.rect.x + (player.rect.width / 2.0));
    double deltaY = GetMouseY() - (player.rect.y + (player.rect.height / 2.0));

    double ratio = BULLET_VELOCITY / sqrt(pow(deltaX, 2) + pow(deltaY, 2));

    bullets[index].velocity.x = deltaX * ratio;
    bullets[index].velocity.y = deltaY * ratio;
}

void draw() {
    BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].isActive) {
                DrawRectangleRec(bullets[i].rect, bullets[i].color);
            }
        }

        DrawRectangleRec(player.rect, player.color);

    EndDrawing();
}
