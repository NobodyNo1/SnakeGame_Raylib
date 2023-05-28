// WORKING on Windows (https://github.com/raysan5/raylib/wiki/Working-on-Windows)
// TL:DR; use NotePad++ for RayLib, F6 compile and execute

#include "raylib.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const float SUGGESTED_RATIO_FOR_GRID = 0.5625;

const int GRID_COLUMS = 10 ;
const int GRID_ROWS = (int) (GRID_COLUMS / SUGGESTED_RATIO_FOR_GRID);
//const int GRID_COLUMS = 10;

const int size = GRID_ROWS*GRID_COLUMS;
int grid[size]; // [i*GRID_COLUMS + j]
 
const int DRAW_AREA_PADDING = 80;

const int CELL_SPACE = 2; 



const int DIRECTION_UP = -1;
const int DIRECTION_DOWN = 1;
const int DIRECTION_LEFT = -2;
const int DIRECTION_RIGHT = 2;

int SNAKE_START_SIZE = 3; 
int snakeDirection = DIRECTION_UP;
struct Vector2 snakeHeadPosition;
int snakeCurrentSize = SNAKE_START_SIZE;

const int FRAME_UPDATE = 1000;



const int GAME_STATE_PAUSE = 0;
const int GAME_STATE_ACTIVE = 1;
const int GAME_STATE_END = 2;

// todo: 
// 1. start with pause
// 2. FIX the bug with rendering 2 snakes at the same time
// 3. FIX border collision (currently top border works fine)
// 4. FIX limitation of the direction 
// 5. MAKE self collision stuff
// 6. FOOD/APPLE generation
// 7. EAT mechanics


int gameState = GAME_STATE_ACTIVE;

void generateGrid() {
    
    for(int i = 0; i < GRID_COLUMS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            grid[GRID_COLUMS*i + j] = 0;
        }
    }
    
    
    snakeHeadPosition.x = GRID_ROWS/2;
    snakeHeadPosition.y = GRID_COLUMS/2 - 1;
    // assume snake always looking up
    // Make sure that start size is not outside of bounds
    int maxSnakeStartSize = GRID_COLUMS - snakeHeadPosition.y + 1;
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
    for (int i = 0; i < SNAKE_START_SIZE; i++){
        int y = (int)snakeHeadPosition.y + i;
        grid[(y)*GRID_COLUMS + (int)snakeHeadPosition.x] = i+1;
    }
    
}


int lastPressedKey = -1;

void updateSnakePosition() {
    snakeDirection = lastPressedKey;
    if(snakeDirection == DIRECTION_UP || snakeDirection == DIRECTION_DOWN) {
        snakeHeadPosition.y = snakeHeadPosition.y + snakeDirection;
    } else if(snakeDirection == DIRECTION_LEFT || snakeDirection == DIRECTION_RIGHT){
        snakeHeadPosition.x = snakeHeadPosition.x + snakeDirection/2;
    }
    
    if(snakeHeadPosition.x > GRID_ROWS || snakeHeadPosition.x < 0
        || snakeHeadPosition.y > GRID_COLUMS || snakeHeadPosition.y < 0){
            gameState = GAME_STATE_END;
            return;
        }
    
    for(int i = 0; i < GRID_COLUMS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            int currentCell = grid[GRID_COLUMS*i + j];
            if(currentCell+1 > snakeCurrentSize){
                grid[GRID_COLUMS*i + j] = 0;
            } else if(currentCell > 0) {
                grid[GRID_COLUMS*i + j] = currentCell + 1;
            }
        }
    }
    grid[((int)snakeHeadPosition.y)*GRID_COLUMS + (int)snakeHeadPosition.x] = 1;
    
}

void handleKeyEvents(){
    if(IsKeyPressed(KEY_W)){
        lastPressedKey = DIRECTION_UP;
    } 
    if(IsKeyPressed(KEY_D)){
        lastPressedKey = DIRECTION_RIGHT;
        
    } 
    if(IsKeyPressed(KEY_S)){
        lastPressedKey = DIRECTION_DOWN;
        
    } 
    if(IsKeyPressed(KEY_A)){
        lastPressedKey = DIRECTION_LEFT;
    }
}
int frameCounter = 0;

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Test");

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
    
      
     
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectangle(drawStartX, drawStartY, drawWidth, drawHeight, RED);
            if(gameState == GAME_STATE_ACTIVE){
                handleKeyEvents();
                frameCounter++;
                if(frameCounter == FRAME_UPDATE){
                    frameCounter = 0;
                    updateSnakePosition();
                }
                for(int i = 0; i < GRID_COLUMS; i++) {
                    for(int j = 0; j < GRID_ROWS; j++) {
                        
                        Color currentColor = BLUE; // empty by default
                        if(grid[GRID_COLUMS*i + j] == 1){
                            // head color
                            currentColor = GREEN;
                        } else if (grid[GRID_COLUMS*i + j] > 1){
                            currentColor = LIME;
                        }
                        
                        DrawRectangle(
                            paddingByX+ drawStartX + ((squareSize +CELL_SPACE)* j),
                            paddingByY+ drawStartY + ((squareSize+CELL_SPACE) * i),
                            squareSize,
                            squareSize,
                            currentColor
                        );
                    }
                }
                
                //WaitTime(FRAME_WAIT_TIME);
            } else {
                DrawText("LOSE", WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 40, LIGHTGRAY);
                
                if(IsKeyPressed(KEY_SPACE)){
                    generateGrid();
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
            DrawText(TextFormat("INFO (frameCounter): %d", frameCounter), 190, 340, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}