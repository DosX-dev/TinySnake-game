// Coded by DosX

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <locale.h>

#define WIDTH 25
#define HEIGHT 25
#define SPEED_DELAY 100

// Define foreground text colors
#define FOREGROUND_BLACK 0x00
#define FOREGROUND_BLUE 0x01
#define FOREGROUND_GREEN 0x02
#define FOREGROUND_CYAN (FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FOREGROUND_RED 0x04
#define FOREGROUND_MAGENTA (FOREGROUND_RED | FOREGROUND_BLUE)
#define FOREGROUND_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FOREGROUND_GRAY 0x08
#define FOREGROUND_LIGHT_BLUE (FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define FOREGROUND_LIGHT_GREEN (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define FOREGROUND_LIGHT_CYAN (FOREGROUND_CYAN | FOREGROUND_INTENSITY)
#define FOREGROUND_LIGHT_RED (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define FOREGROUND_LIGHT_MAGENTA (FOREGROUND_MAGENTA | FOREGROUND_INTENSITY)
#define FOREGROUND_LIGHT_YELLOW (FOREGROUND_YELLOW | FOREGROUND_INTENSITY)
#define FOREGROUND_BRIGHT_WHITE (FOREGROUND_WHITE | FOREGROUND_INTENSITY)

// Define background colors
#define BACKGROUND_BLACK 0x00
#define BACKGROUND_BLUE 0x10
#define BACKGROUND_GREEN 0x20
#define BACKGROUND_CYAN (BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BACKGROUND_RED 0x40
#define BACKGROUND_MAGENTA (BACKGROUND_RED | BACKGROUND_BLUE)
#define BACKGROUND_YELLOW (BACKGROUND_RED | BACKGROUND_GREEN)
#define BACKGROUND_WHITE (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BACKGROUND_GRAY 0x80
#define BACKGROUND_LIGHT_BLUE (BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define BACKGROUND_LIGHT_GREEN (BACKGROUND_GREEN | BACKGROUND_INTENSITY)
#define BACKGROUND_LIGHT_CYAN (BACKGROUND_CYAN | BACKGROUND_INTENSITY)
#define BACKGROUND_LIGHT_RED (BACKGROUND_RED | BACKGROUND_INTENSITY)
#define BACKGROUND_LIGHT_MAGENTA (BACKGROUND_MAGENTA | BACKGROUND_INTENSITY)
#define BACKGROUND_LIGHT_YELLOW (BACKGROUND_YELLOW | BACKGROUND_INTENSITY)
#define BACKGROUND_BRIGHT_WHITE (BACKGROUND_WHITE | BACKGROUND_INTENSITY)

#define true 0B1
#define false 0B0

int gameover = false,
    score = 0,
    snakeX[100],
    snakeY[100],
    snake_length = 1,
    foodX = 0, foodY = 0,
    dirX = 0, dirY = 0;

void setup()
{
    snakeX[0] = WIDTH / 2;
    snakeY[0] = HEIGHT / 2;

    srand(time(NULL)); // Initialize the random number generator with the current time
    foodX = 1 + rand() % (WIDTH - 2);
    foodY = 1 + rand() % (HEIGHT - 2);
}

void clearConsole()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD homeCoords = {0, 2}; // Start from the top-left corner of the game field
    DWORD count;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (hOut == INVALID_HANDLE_VALUE)
    {
        return;
    }

    if (!GetConsoleScreenBufferInfo(hOut, &csbi))
    {
        return;
    }

    // Clear the game field area
    SetConsoleCursorPosition(hOut, homeCoords);
}

CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
WORD savedAttributes;

// Function to draw the game field and snake
void draw(hConsole)
{
    clearConsole(); // Clear the console before drawing the new frame
    int i, j;
    for (i = 0; i < HEIGHT; i++)
    {
        for (j = 0; j < WIDTH; j++)
        {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)
            {

                GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
                savedAttributes = consoleInfo.wAttributes;
                SetConsoleTextAttribute(hConsole, BACKGROUND_CYAN);
                printf("  "); // Add a border
                SetConsoleTextAttribute(hConsole, savedAttributes);
            }
            else if (i == snakeY[0] && j == snakeX[0])
            {
                GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
                savedAttributes = consoleInfo.wAttributes;

                SetConsoleTextAttribute(hConsole, BACKGROUND_WHITE | FOREGROUND_BLACK);
                printf("^^"); // Head with eyes
                SetConsoleTextAttribute(hConsole, savedAttributes);
            }
            else if (i == foodY && j == foodX)
            {
                GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
                savedAttributes = consoleInfo.wAttributes;

                SetConsoleTextAttribute(hConsole, BACKGROUND_LIGHT_RED);
                printf("  "); // Apple
                SetConsoleTextAttribute(hConsole, savedAttributes);
            }
            else
            {
                int isBody = 0;
                for (int k = 1; k < snake_length; k++)
                {
                    if (snakeX[k] == j && snakeY[k] == i)
                    {
                        GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
                        savedAttributes = consoleInfo.wAttributes;
                        SetConsoleTextAttribute(hConsole, BACKGROUND_GRAY | FOREGROUND_GRAY);
                        printf("##"); // Body
                        SetConsoleTextAttribute(hConsole, savedAttributes);
                        isBody = 1;
                        break;
                    }
                }
                if (!isBody)
                {
                    printf("  "); // Draw two empty spaces
                }
            }
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
}

// Function to handle user input
void input()
{
    if (_kbhit())
    {
        switch (tolower(_getch()))
        {
        case 'a':
            // Ignore if the current direction is right (moving to the right) to avoid going backward
            if (dirX != 1)
            {
                dirX = -1; // Move left
                dirY = 0;
            }
            break;
        case 'd':
            // Ignore if the current direction is left (moving to the left) to avoid going backward
            if (dirX != -1)
            {
                dirX = 1; // Move right
                dirY = 0;
            }
            break;
        case 'w':
            // Ignore if the current direction is down (moving downwards) to avoid going backward
            if (dirY != 1)
            {
                dirX = 0;
                dirY = -1; // Move up
            }
            break;
        case 's':
            // Ignore if the current direction is up (moving upwards) to avoid going backward
            if (dirY != -1)
            {
                dirX = 0;
                dirY = 1; // Move down
            }
            break;
        }
    }
}

// Function to update the game logic
void logic()
{
    int prevX = snakeX[0];
    int prevY = snakeY[0];
    int prev2X, prev2Y;
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

    for (int i = 1; i < snake_length; i++)
    {
        prev2X = snakeX[i];
        prev2Y = snakeY[i];
        snakeX[i] = prevX;
        snakeY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // Check if the snake eats the food
    if (snakeX[0] == foodX && snakeY[0] == foodY)
    {
        score += 10; // Increase the score
        // Generate new random coordinates for food within the game field (excluding the borders)
        foodX = 1 + rand() % (WIDTH - 2);
        foodY = 1 + rand() % (HEIGHT - 2);
        snake_length++; // Increase the length of the snake
    }

    // Check for collision with itself
    for (int i = 1; i < snake_length; i++)
    {
        if (snakeX[i] == snakeX[0] && snakeY[i] == snakeY[0])
        {
            gameover = true; // Game over if the snake collides with itself
            break;
        }
    }

    // Check for collision with the wall
    if (snakeX[0] == 0 || snakeX[0] == WIDTH - 1 || snakeY[0] == 0 || snakeY[0] == HEIGHT - 1)
    {
        gameover = true; // Game over if the snake hits the border
    }
}

int main()
{
    SetConsoleTitleA("Tiny Snake");
    int buffer[] = {WIDTH * 2 + 1, HEIGHT + 8};

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleScreenBufferSize(hConsole, (COORD){buffer[0], buffer[1]});

    SMALL_RECT windowSize = {0, 0, buffer[0] - 1, buffer[1] - 1};
    SetConsoleWindowInfo(hConsole, true, &windowSize);

    setlocale(LC_ALL, ".65001");
    printf("Coded by DosX-dev (GitHub)\nUSE ONLY ENGLISH KEYBOARD LAYOUT! (WASD)\n");
    setup(); // Initialize the game

    dirX = 0;
    dirY = 1; // Move down

    while (!gameover)
    {
        draw(hConsole); // Draw the current state of the game
        input();        // Handle user input
        logic();        // Update the game logic

        Sleep(SPEED_DELAY); // Add a delay to control the snake's speed
    }
    printf("\n\n====================\nGame Over!\nYour score: %d\n====================\nPress X to exit", score);

    while (true)
    {
        if (tolower(_getch()) == 'x')
        {
            return 0;
        }
    }
}
