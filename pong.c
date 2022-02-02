/*
    Pong implementation
    Made - Dec 20-21, 2021

    A simple pong implementation. It lacks some features like a menu and game over screen, but the game is done.
    
    You may notice that in sosme places, name collisions, it uses a range, like ball position x <= 0 instead of == 0
    This is because the ball speeds up every time a paddle hits it, but if it speeds up, it begins to skip pixels.
    For example, if you have two balls starting at the left end of the screen, one with 1 velocity, the other with 2
        the one with 1 velocity would go to every pixel, like 1, 2, 3, 4, 5, 6
        the one with 2 velocity would begin skipping pixels and go 2, 4, 6, 8, 10

    This happens because you just add a velocity to the position, so unless you have a billion calls for update, the position will just skip pixels if speed != 0
    Therefore, to handle this, you can use a range to check if the ball would be at the paddle or inside, then rebound - this way the ball wont pass through
        after that, you need to make sure that you set a max speed, because if the paddle with is constant but the speed goes up infinitely, it will eventually reach a speed that is greater than the width of teh paddle and pass through
    
*/

#include "raylib.h"

void initialize();
void update();
void draw();

typedef struct Paddle {
    int x;
    int y;
} Paddle;

const int PADDLE_VELOCITY = 5;

const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 100;

typedef struct Ball {
    int x;
    int y;
    int xVelocity;
    int yVelocity;
} Ball;

const int BALL_LENGTH = 10;
const int INIT_BALL_VELOCITY = 5;
const int BALL_VELOCITY_MAX = 9;

int screenWidth = 850;
int screenHeight = 400;

int playerScore = 0;
int computerScore = 0;

int main(void) {

    Paddle player;
    Paddle computer;
    Ball ball;

    initialize(&player, &computer, &ball);

    while (!WindowShouldClose()) {
        // you could have a general update method - update = hub method, then have a bunch of sub updates there
        // ie. in update, you could have player input update, then automatic update
        update(&player, &computer, &ball);

        // maybe you could have a game elements array, then go through each one and draw them
        draw(player, computer, ball);
    }

    CloseWindow();

    return 0;

}

void initialize(Paddle *player, Paddle *computer, Ball *ball) {
    InitWindow(screenWidth, screenHeight, "Pong");

    player->x = 0 + 30;
    player->y = (screenHeight / 2) - (PADDLE_HEIGHT / 2);

    computer->x = screenWidth - 30;
    computer->y = (screenHeight / 2) - (PADDLE_HEIGHT / 2);

    ball->x = (screenWidth / 2) - (BALL_LENGTH / 2);
    ball->y = (screenHeight / 2) - (BALL_LENGTH / 2);
    ball->xVelocity = INIT_BALL_VELOCITY;
    ball->yVelocity = INIT_BALL_VELOCITY;

    SetTargetFPS(60);
}

void update(Paddle *player, Paddle *computer, Ball *ball) {

    // player input section
    if (IsKeyDown(KEY_W)) {
        if (player->y == 0) {
            player->y = 0;
        }
        else {
            player->y -= PADDLE_VELOCITY;
        }
    }
    else if (IsKeyDown(KEY_S)) {
        if ((player->y + PADDLE_HEIGHT ) == screenHeight) {
            player->y = screenHeight - PADDLE_HEIGHT;
        }
        else {
            player->y += PADDLE_VELOCITY;
        }
    }

    // update computer position
    if (ball->y > computer->y + (PADDLE_HEIGHT / 2)) {
        // ball is below computer paddle midpoint

        if (computer->y + PADDLE_HEIGHT == screenHeight) {
            computer->y = screenHeight - PADDLE_HEIGHT;
        }
        else 
        {
            computer->y += PADDLE_VELOCITY;
        }
    }
    else if (ball->y < computer->y + (PADDLE_HEIGHT / 2)) {
        // ball is above computer paddle
        if (computer->y == 0) {
            computer->y = 0;
        }
        else {
            computer->y -= PADDLE_VELOCITY;
        }
    }

    if (ball->x + BALL_LENGTH >= screenWidth) {
        // computer lost

        // reset speeds and position of ball and players
        ball->xVelocity = INIT_BALL_VELOCITY;
        ball->x = (screenWidth / 2) - (BALL_LENGTH / 2);
        ball->y = (screenHeight / 2) - (BALL_LENGTH / 2);
        ball->yVelocity = INIT_BALL_VELOCITY;

        player->x = 0 + 30;
        player->y = (screenHeight / 2) - (PADDLE_HEIGHT / 2);

        computer->x = screenWidth - 30;
        computer->y = (screenHeight / 2) - (PADDLE_HEIGHT / 2);

        playerScore++;
    }
    else if (ball->x <= 0) {
        // player lost
        ball->xVelocity = INIT_BALL_VELOCITY;
        ball->x = (screenWidth / 2) - (BALL_LENGTH / 2);
        ball->y = (screenHeight / 2) - (BALL_LENGTH / 2);
        ball->yVelocity = INIT_BALL_VELOCITY;

        player->x = 0 + 30;
        player->y = (screenHeight / 2) - (PADDLE_HEIGHT / 2);

        computer->x = screenWidth - 30;
        computer->y = (screenHeight / 2) - (PADDLE_HEIGHT / 2);
        
        computerScore++;
    }

    // Bounce ball if it hits the top of the screen
    if ((ball->y + BALL_LENGTH >= screenHeight) || (ball->y <= 0)) {
        ball->yVelocity = -(ball->yVelocity);
    }

    // collisions
    // reflect the ball if it comes in contact with the player or computer paddles
    // even though they do the same thing, the reason they are split is so that you do not have a massive if statement condition
    if (((ball->x <= player->x + PADDLE_WIDTH) && (ball->x >= player->x)) && ((ball->y >= player->y) && (ball->y <= player->y + PADDLE_HEIGHT))) {
        ball->xVelocity = -ball->xVelocity;

        // This speeds up the ball up to 9 pixels/second, +1 each time it hits a paddle
        // BALL_VELOCITY_MAX is here because if the speed is too high, the ball will skip the pixels of the paddle and go through it
        // 9 is max so that the most pixels per second it can move is 9, which is 1 pixel less than the paddle width (so it cannot pass through it)
        if (ball->xVelocity > 0 && ball->xVelocity < BALL_VELOCITY_MAX) {
            ball->xVelocity += 1;
        }
        else if (ball->xVelocity < 0 && ball->xVelocity > -BALL_VELOCITY_MAX){
            ball->xVelocity -= 1;
        }

        if (ball->yVelocity > 0 && ball->yVelocity < BALL_VELOCITY_MAX) {
            ball->yVelocity += 1;
        }
        else if (ball->yVelocity < 0 && ball->yVelocity > -BALL_VELOCITY_MAX){
            ball->yVelocity -= 1;
        }
    }
    else if (((ball->x <= computer->x + PADDLE_WIDTH) && (ball->x >= computer->x)) && ((ball->y >= computer->y) && (ball->y <= computer->y + PADDLE_HEIGHT))) {
        ball->xVelocity = -ball->xVelocity;

        if (ball->xVelocity > 0 && ball->xVelocity < BALL_VELOCITY_MAX) {
            ball->xVelocity += 1;
        }
        else if (ball->xVelocity < 0 && ball->xVelocity > -BALL_VELOCITY_MAX){
            ball->xVelocity -= 1;
        }

        if (ball->yVelocity > 0 && ball->yVelocity < BALL_VELOCITY_MAX) {
            ball->yVelocity += 1;
        }
        else if (ball->yVelocity < 0 && ball->yVelocity > -BALL_VELOCITY_MAX){
            ball->yVelocity -= 1;
        }
    }

    // the actual movement of the ball
    ball->y -= ball->yVelocity;
    ball->x += ball->xVelocity;
}

void draw(Paddle player, Paddle computer, Ball ball) {
    BeginDrawing();

        ClearBackground(BLACK);
    
        DrawRectangle(player.x, player.y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
        DrawRectangle(computer.x, computer.y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
        DrawRectangle(ball.x, ball.y, BALL_LENGTH, BALL_LENGTH, WHITE);

        DrawText(TextFormat("%d", playerScore), screenWidth / 2 - 20, 20, 20, WHITE);
        DrawText(TextFormat("%d", computerScore), screenWidth / 2 + 20, 20, 20, WHITE);


    EndDrawing();
}
