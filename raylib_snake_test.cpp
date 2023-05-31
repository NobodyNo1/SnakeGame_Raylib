// WORKING on Windows (https://github.com/raysan5/raylib/wiki/Working-on-Windows)
// TL:DR; use NotePad++ for RayLib, F6 compile and execute

#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const float SUGGESTED_RATIO_FOR_GRID = 0.5625;

const int GRID_COLUMS = 20 ;
const int GRID_ROWS = (int) (GRID_COLUMS / SUGGESTED_RATIO_FOR_GRID);
//const int GRID_ROWS = 10;

const int size = GRID_ROWS*GRID_COLUMS;
int grid[size]; // [i*GRID_COLUMS + j]
 
const int DRAW_AREA_PADDING = 80;

const int CELL_SPACE = 2; 


const int DIRECTION_UP = -1;
const int DIRECTION_DOWN = 1;
const int DIRECTION_LEFT = -2;
const int DIRECTION_RIGHT = 2;

int SNAKE_START_SIZE = 5; 
int snakeDirection = DIRECTION_UP;
int snakeCurrentSize = SNAKE_START_SIZE;

const int FRAME_UPDATE = 30;

const int GAME_STATE_PAUSE = 0;
const int GAME_STATE_ACTIVE = 1;
const int GAME_STATE_END = 2;

// todo: 
// 1. MAKE self collision stuff
// 2. FOOD/APPLE generation
// 3. EAT mechanics
// 4. Improve UI


int gameState = GAME_STATE_PAUSE;
int updateRate = 0;
#define MAX_UPDATE_RATE 4

struct Position {
    int row;
    int column;
};

struct Position snakeHeadPosition;

void generateFoodPosition() {
    int foodColumn = rand()%GRID_COLUMS; 
    int foodRow    = rand()%GRID_ROWS; 

    // todo: encuring that position is not overlaping with snake
    grid[GRID_ROWS*foodColumn + foodRow] = -1;
}

void generateGrid() {
    snakeCurrentSize = SNAKE_START_SIZE;
    snakeDirection = DIRECTION_UP;
    for(int i = 0; i < GRID_COLUMS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            grid[GRID_ROWS*i + j] = 0;
        }
    }
    
    generateFoodPosition();

    snakeHeadPosition.row = (int)(GRID_ROWS/2);
    snakeHeadPosition.column = GRID_COLUMS/2 - 1;
    // assume snake always looking up
    // Make sure that start size is not outside of bounds
    int maxSnakeStartSize = GRID_COLUMS - snakeHeadPosition.column + 1;
    if(SNAKE_START_SIZE > maxSnakeStartSize){
        SNAKE_START_SIZE = maxSnakeStartSize;
    }
    int snakeCurrentSize = SNAKE_START_SIZE;

    // GRID content
    // - snake body structure is 
    //      1 - head, consective values are the body values
    // - negative values are items 
    //       (-1) is apple
    // - 0 is empty space
    
    // put snake into grid
    
    
    // snake body (assume snake always looking up)
    for (int i = 0; i < SNAKE_START_SIZE; i++) {
        int y = snakeHeadPosition.column + i;
        grid[y*GRID_ROWS + snakeHeadPosition.row] = i+1;
    }
    //grid[snakeHeadPosition.column*GRID_ROWS + snakeHeadPosition.row] = 1;
}


int lastPressedKey = -1;

void updateSnakePosition() {
    snakeDirection = lastPressedKey;
    if(snakeDirection == DIRECTION_UP || snakeDirection == DIRECTION_DOWN) {
        snakeHeadPosition.column = snakeHeadPosition.column + snakeDirection;
    } else if(snakeDirection == DIRECTION_LEFT || snakeDirection == DIRECTION_RIGHT){
        snakeHeadPosition.row = snakeHeadPosition.row + snakeDirection/2;
    }
    
    if(snakeHeadPosition.row >= GRID_ROWS || snakeHeadPosition.row < 0
        || snakeHeadPosition.column >= GRID_COLUMS || snakeHeadPosition.column < 0)
        {
            gameState = GAME_STATE_END;
            return;
        }
    if(grid[snakeHeadPosition.column*GRID_ROWS + snakeHeadPosition.row] == -1){
        snakeCurrentSize++;
        generateFoodPosition();
    }
    for(int i = 0; i < GRID_COLUMS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            int currentCell = grid[GRID_ROWS*i + j];
            if(currentCell + 1 > snakeCurrentSize){
                grid[GRID_ROWS*i + j] = 0;
            } else if(currentCell > 0) {
                grid[GRID_ROWS*i + j] = currentCell + 1;
            }
            
        }
    }
    grid[snakeHeadPosition.column*GRID_ROWS + snakeHeadPosition.row] = 1;
}

void handleKeyEvents(){
    if(snakeDirection != DIRECTION_DOWN && IsKeyPressed(KEY_W)){
        lastPressedKey = DIRECTION_UP;
    } 
    if(snakeDirection != DIRECTION_LEFT && IsKeyPressed(KEY_D)){
        lastPressedKey = DIRECTION_RIGHT;
    } 
    if(snakeDirection != DIRECTION_UP && IsKeyPressed(KEY_S)){
        lastPressedKey = DIRECTION_DOWN;
        
    } 
    if(snakeDirection != DIRECTION_RIGHT &&IsKeyPressed(KEY_A)){
        lastPressedKey = DIRECTION_LEFT;
    }
}

void renderGame(int gridStartX, int gridStartY, int squareSize) {
    for(int i = 0; i < GRID_COLUMS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            Color currentColor = BEIGE; // empty by default
            int cell = grid[GRID_ROWS*i + j];
            if(cell == 1){
                // head color
                currentColor = GREEN;
            } else if (cell > 1){
                currentColor = LIME;
            } else if(cell == -1) {
                currentColor = RED;
            }
            DrawRectangle(
                gridStartX + ((squareSize + CELL_SPACE) * j),
                gridStartY + ((squareSize + CELL_SPACE) * i),
                squareSize,
                squareSize,
                currentColor
            );
        }
    }
}


int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Test");
    srand(time(NULL));
    
    // Drawing area sizes
    int drawWidth = WINDOW_WIDTH - 2*DRAW_AREA_PADDING;
    int drawHeight = WINDOW_HEIGHT - 2*DRAW_AREA_PADDING;
    int drawStartX = DRAW_AREA_PADDING;
    int drawStartY = DRAW_AREA_PADDING;
    
    // todo: input data to grid
    generateGrid();
    // Grid 
    // 1. CELLS - square size
    // 2. There are some padding between
    // 3. Should be scaled depending on draw size
    int maxSizeByWidth = (drawWidth - (GRID_ROWS - 2) * CELL_SPACE)/GRID_ROWS;
    int maxSizeByHeight = (drawHeight - (GRID_COLUMS - 2) * CELL_SPACE)/GRID_COLUMS;
    
    int squareSize = maxSizeByHeight;
    if(maxSizeByWidth < maxSizeByHeight){
        squareSize = maxSizeByWidth;
    } else {
        squareSize = maxSizeByHeight;
    }
    
    // Centering the grid
    int paddingByX = (drawWidth - ((squareSize + CELL_SPACE) * GRID_ROWS))/2;
    int paddingByY = (drawHeight - ((squareSize + CELL_SPACE) * GRID_COLUMS))/2;
    
    SetTargetFPS(FRAME_UPDATE);

    char* startText = "PRESS \"SPACE\" KEY TO START";
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectangle(drawStartX, drawStartY, drawWidth, drawHeight, GRAY);
            if(gameState == GAME_STATE_ACTIVE) {
                handleKeyEvents();
                updateRate++;
                updateRate = updateRate%MAX_UPDATE_RATE;
                if(updateRate == 0)
                    updateSnakePosition();
                
                renderGame(paddingByX + drawStartX, paddingByY + drawStartY, squareSize);
                //WaitTime(FRAME_WAIT_TIME);
            } else if (gameState == GAME_STATE_END) {
                DrawText("LOSE", WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 40, LIGHTGRAY);
                
                if(IsKeyPressed(KEY_SPACE)){
                    generateGrid();
                    gameState = GAME_STATE_PAUSE;
                }
            } else if(gameState == GAME_STATE_PAUSE){
                renderGame(paddingByX + drawStartX, paddingByY + drawStartY, squareSize);
                DrawText(startText, WINDOW_WIDTH/2 - MeasureText(startText, 40)/2, WINDOW_HEIGHT/2, 40, LIGHTGRAY);
                if(IsKeyPressed(KEY_SPACE)){
                    gameState = GAME_STATE_ACTIVE;
                }
            }
            DrawText(TextFormat("ROW: %d", GRID_ROWS), 190, 200, 20, LIGHTGRAY);
            DrawText(TextFormat("COLUMNS: %d", GRID_COLUMS), 190, 220, 20, LIGHTGRAY);
            DrawText(TextFormat("INFO (paddingByY): %d", paddingByY), 190, 240, 20, LIGHTGRAY);
            DrawText(TextFormat("INFO (paddingByX): %d", paddingByX), 190, 260, 20, LIGHTGRAY);
            DrawText(TextFormat("INFO (drawWidth): %d", drawWidth), 190, 280, 20, LIGHTGRAY);
            DrawText(TextFormat("INFO (drawHeight): %d", drawHeight), 190, 300, 20, LIGHTGRAY);
    
            DrawText(TextFormat("INFO (lastPressedKey): %d", lastPressedKey), 190, 320, 20, LIGHTGRAY);
            DrawText(TextFormat("INFO (frameCounter): %d", GetTime()), 190, 340, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}