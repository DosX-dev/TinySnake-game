// Coded by DosX
// GitHub: https://github.com/DosX-dev

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "colors.h"

#define SIZE 25

#define WIDTH SIZE
#define HEIGHT SIZE

#define SPEED_DELAY 100

#define true 0b1          //
#define false 0b0         // I know there are predefined values ​​like TRUE, FALSE, NULL etc
#define short 32768       // But I prefer using these values ​​in small case.
#define null (void *)0b0  //

int gameover = false,
    score = 0,
    snakeX[short], snakeY[short],
    snake_length = 1,
    foodX = 0, foodY = 0,
    dirX = 0, dirY = 0;

HANDLE hConsole = null;
CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

void setup() {
    snakeX[0] = WIDTH / 2;
    snakeY[0] = HEIGHT / 2;  // Set snake head at center of game zone

    srand(time(0x00));  // Initialize the random number generator with the current time
    foodX = 1 + rand() % (WIDTH - 2);
    foodY = 1 + rand() % (HEIGHT - 2);

    dirX = 0;
    dirY = 1;  // Move down
}

void refillRenderZone() {
    SetConsoleCursorPosition(hConsole, (COORD){0, 2});  // Skip 2 lines
}

void draw(char *text, int color) {
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, color);
    printf(text);
    SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
}

// Function to render the game screen
void render() {
    refillRenderZone();

    for (int i = 0; i < HEIGHT; i++)  // Loop through the game board rows
    {
        for (int j = 0; j < WIDTH; j++)  // Loop through the game board columns
        {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)               // Check if it's a border cell
                draw("  ", !gameover ? BACKGROUND_CYAN : BACKGROUND_RED);            // Draw a cyan-colored border
            else if (i == snakeY[0] && j == snakeX[0])                               // Check if it's the snake's head cell
                draw(!gameover ? "^^" : "oO", BACKGROUND_WHITE | FOREGROUND_BLACK);  // Draw the snake's head with eyes
            else if (i == foodY && j == foodX)                                       // Check if it's the food cell
                draw("  ", BACKGROUND_LIGHT_RED);                                    // Draw an apple (food)
            else {
                int isBody = false;
                for (int k = 1; k < snake_length; k++)  // Loop through the snake's body cells
                {
                    if (snakeX[k] == j && snakeY[k] == i)  // Check if it's a cell in the snake's body
                    {
                        draw("##", BACKGROUND_GRAY | FOREGROUND_GRAY);  // Draw the snake's
                                                                        // body segment
                        isBody = true;
                        break;
                    }
                }
                if (!isBody) {
                    printf("  ");  // Draw two empty spaces for an empty cell
                }
            }
        }
        printf("\n");  // Move to the next row
    }

    draw("SCORE", BACKGROUND_YELLOW);  // Draw the "SCORE" label in yellow
    printf(": %d\n", score);           // Display the current score value
}

// Function to handle user input
void input() {
    if (_kbhit()) {
        switch (tolower(_getch())) {
            case 'a':
                // Ignore if the current direction is right (moving to the
                // right) to avoid going backward
                if (dirX != 1) {
                    dirX = -1;  // Move left
                    dirY = 0;
                }
                break;
            case 'd':
                // Ignore if the current direction is left (moving to the left)
                // to avoid going backward
                if (dirX != -1) {
                    dirX = 1;  // Move right
                    dirY = 0;
                }
                break;
            case 'w':
                // Ignore if the current direction is down (moving downwards) to
                // avoid going backward
                if (dirY != 1) {
                    dirX = 0;
                    dirY = -1;  // Move up
                }
                break;
            case 's':
                // Ignore if the current direction is up (moving upwards) to
                // avoid going backward
                if (dirY != -1) {
                    dirX = 0;
                    dirY = 1;  // Move down
                }
                break;
        }
    }
}

// Function to update the game logic
void logic() {
    int prevX = snakeX[0],
        prevY = snakeY[0],
        prev2X, prev2Y;
    snakeX[0] += dirX;
    snakeY[0] += dirY;

    // Teleport the snake to the opposite side of the map if it hits a wall
    if (snakeX[0] == 0)
        snakeX[0] = WIDTH - 2;
    else if (snakeX[0] == WIDTH - 1)
        snakeX[0] = 1;
    if (snakeY[0] == 0)
        snakeY[0] = HEIGHT - 2;
    else if (snakeY[0] == HEIGHT - 1)
        snakeY[0] = 1;

    for (int i = 1; i < snake_length; i++) {
        prev2X = snakeX[i];
        prev2Y = snakeY[i];
        snakeX[i] = prevX;
        snakeY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // Check if the snake eats the food
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        // Generate new random coordinates for food within the game field (excluding the borders)
        do {
            foodX = 1 + rand() % (WIDTH - 2);
            foodY = 1 + rand() % (HEIGHT - 2);
        } while (foodX == snakeX[0] && foodY == snakeY[0]);

        // Check if the food spawns on the snake's body
        int spawnOnBody = true;
        for (int i = 1; i < snake_length; i++) {
            if (foodX == snakeX[i] && foodY == snakeY[i]) {
                spawnOnBody = true;
                break;
            }
        }

        // If the food spawns on the snake's body, try generating new coordinates again
        while (spawnOnBody) {
            foodX = 1 + rand() % (WIDTH - 2);
            foodY = 1 + rand() % (HEIGHT - 2);

            spawnOnBody = false;
            for (int i = 0; i < snake_length; i++) {
                // If the coordinates of the Apple are on the body of the snake, then the cycle continues
                if (foodX == snakeX[i] && foodY == snakeY[i]) {
                    spawnOnBody = true;
                    break;
                }
            }
        }

        score += 10;     // Increase the score
        snake_length++;  // Increase the length of the snake
    }

    // Check for collision with itself
    for (int i = 1; i < snake_length; i++) {
        if (snakeX[i] == snakeX[0] && snakeY[i] == snakeY[0]) {
            gameover = true;  // Game over if the snake collides with itself
            break;
        }
    }

    /*
   // Check for collision with the wall
    if (snakeX[0] == 0 || snakeX[0] == WIDTH - 1 || snakeY[0] == 0 || snakeY[0]
   == HEIGHT - 1)
    {
        gameover = true; // Game over if the snake hits the border
    }
*/
}

// Oh ok, legacy code. Really bad code
// int getBitness() { return sizeof(void *) == 0x4 ? 32 : 64; }

#if defined(__i386__)  // i386 (32-bit)
#define bitness 32
#elif defined(__x86_64__)  // x86-64 (AMD64)
#define bitness 64
#else
#define bitness 32
#endif

void main() {
    char title[short];

    sprintf(title, "Tiny Snake (x%d)", bitness);

    SetConsoleTitleA(title);

    int buffer[] = {(WIDTH * 2) + 1, HEIGHT + 8};

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SMALL_RECT windowSize = {0, 0, buffer[0] - 1, buffer[1] - 1};
    SetConsoleWindowInfo(hConsole, true, &windowSize);

    SetConsoleScreenBufferSize(hConsole, (COORD){buffer[0], buffer[1]});

    CONSOLE_CURSOR_INFO cursorinfo = {0};
    cursorinfo.dwSize = 1;
    cursorinfo.bVisible = FALSE;  // Turns off the blinking cursor (Thank to https://github.com/R32GTT)
    SetConsoleCursorInfo(hConsole, &cursorinfo);

    printf("Coded by DosX-dev (GitHub)\nUSE ONLY ENGLISH KEYBOARD LAYOUT! (WASD)\n");

    setup();  // Initialize the game

    while (!gameover) {
        render();  // Draw the current state of the game
        input();   // Handle user input
        logic();   // Update the game logic

        Sleep(SPEED_DELAY);  // Add a delay to control the snake's speed
    }

    dirX = 0;
    dirY = 0;

    render();  // Run the renderer for the last time to draw the desired colors (lose)

    printf("\n");

    draw("==============\n=", BACKGROUND_RED | FOREGROUND_RED);
    draw(" GAME OVER! ", BACKGROUND_WHITE | FOREGROUND_RED);
    draw("=\n==============", BACKGROUND_RED | FOREGROUND_RED);

    printf("\nPress X to exit");

    while (true) {
        if (tolower(_getch()) == 'x') {
            return;
        }
    }
}

// I like coffee. Preferably with milk
