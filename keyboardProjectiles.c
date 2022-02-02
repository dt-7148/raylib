#include "raylib.h"

#include <stdbool.h>

void initialize();
void update();
void draw();

typedef struct Player {   
    Rectangle rect;
    double yVelocity;
    Color color;
} Player;

typedef struct Bullet {
    Rectangle rect;
    double xVelocity;
    Color color;
    bool isActive;
} Bullet;

#define MAX_BULLETS 50

Player player;
static Bullet bullets[MAX_BULLETS] = { 0 };

int curBullet;
int frameCounter;

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

    curBullet = 0;
    frameCounter = 0;

    player.rect.height = 20;
    player.rect.width = 40;
    player.rect.x = 5;
    player.rect.y = screenHeight / 2 - (player.rect.height / 2);
    player.yVelocity = 10;
    player.color = BLACK;

    // Initialize all bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].rect.height = 10;
        bullets[i].rect.width = 10;
        bullets[i].rect.x = player.rect.x;
        bullets[i].rect.y = player.rect.y;
        bullets[i].color = GRAY;
        bullets[i].xVelocity = 0;
        bullets[i].isActive = false;
    }
}

void update() {

    // update player position
    if (IsKeyDown(KEY_W)) { player.rect.y -= player.yVelocity; }
    if (IsKeyDown(KEY_S)) { player.rect.y += player.yVelocity; }

    // wall behaviour
    if (player.rect.y <= 0) { player.rect.y = 0; }
    if (player.rect.y + player.rect.height >= screenHeight) { player.rect.y = screenHeight - player.rect.height; }

    if (IsKeyDown(KEY_SPACE)) {
        /*
            so how does this work
            so we have a frame counter that keeps track of frames - it starts at 1 because we add 1 at the start
                it also resets when it reaches 60, or 1 second
            so every frame, the counter increases by 1
            since we want to shoot a bullet every 4 frames, then we check if the frame counter mod 4 == 0, so every 4th frame
            
            we also have a curBullet variable - it goes from 0 to maxbullet-1 then back to 0
            basically what happens is that every time it is the 4th frame, you set the velocity of the curBullet bullet to active so that it will move
            
            curBullet works because you dont really need to pick a specific bullet to give a velocity
                this is because once you shoot it (make it move), its position will now change because of its velocity
                therefore, it will always appear closer to the end of the screen because its position is later
                the index does not change anything, its just based off of positions, and if a bullet velocity is set earlier, it will be farther than whatever comes after
                curBullets going up is just to make sure that all elements of the array actually get used and you dont get jumping around the array

                tldr: 
                    index of bullet to be shot doesnt matter because once given velocity, its position will always be farther than the bullet shot after it
                    draw draws the bullets based on position, so the index does not matter
                    curBullets is not mandator, but makes sure that all indexes are used and you dont need to check if a bullet is active before setting it

                eg.
                    if we wanted to shoot twice

                    with random index model
                        first shot index: 12
                        second shot index: 3

                        1. first shot is given velocity
                        2. first shot moves [velocity] units forwards
                        3. second shot is given velocity
                        4. first shot moves [velocity] units 
                        5. second shot moves [velocity] units forwards

                        therefore, when the second draw is called for both bullets, the second shot, even though its at an earlier index, has an earlier position, so it will be drawn 
                            behind the first shot

                    this princinple is the same for curBullet, except that curBullet is a bit more orderly and makes it so that you dont need to check if an index is nto aactive before setting it
                    
        */

        frameCounter++;

        if (frameCounter > 60) {
            frameCounter = 1;
        }

        if (frameCounter % 4 == 0) {
            bullets[curBullet].isActive = true;
            bullets[curBullet].xVelocity = 20;

            curBullet++;

            if (curBullet == MAX_BULLETS) { curBullet = 0; }
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        // If bullet is not moving, make it track the player
        if (bullets[i].xVelocity == 0) {
            bullets[i].rect.x = player.rect.x;
            bullets[i].rect.y = player.rect.y;
        }

        // If bullet is active, make it move forwards
        if (bullets[i].isActive) {
            bullets[i].rect.x += bullets[i].xVelocity;
        }

        // If bullet reaches the end of the screen, reset it
        if (bullets[i].rect.x >= screenWidth) {
            bullets[i].xVelocity = 0;
            bullets[i].rect.x = player.rect.x;
            bullets[i].rect.y = player.rect.y;
            bullets[i].isActive = false;
        }
    }
}

void draw() {
    BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawRectangleRec(player.rect, player.color);

        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].isActive) {
                DrawRectangleRec(bullets[i].rect, bullets[i].color);
            }
        }

    EndDrawing();
}
