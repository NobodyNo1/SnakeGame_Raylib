// SNAKE game implemented using raylib

// TODO: 
// 2. Logic to increase speed +- 
// 3. Food generation improvement (state where most of cells are snake's body) ?
// 4. Build for android
// 5. add snake AI 

#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define  WINDOW_WIDTH       1280
#define  WINDOW_HEIGHT      720
 
#define FRAMES_PER_SECOND   30

#define GAME_STATE_PAUSE    0
#define GAME_STATE_ACTIVE   1
#define GAME_STATE_END      2
#define GAME_STATE_WIN      3

#define DRAW_AREA_PADDING   80

#define CELL_SPACE          2

#define DIRECTION_UP      (-1)
#define DIRECTION_DOWN      1
#define DIRECTION_LEFT    (-2)
#define DIRECTION_RIGHT     2

#define SNAKE_START_SIZE    5 

#define OPTIMISATION_DEFAULT             0
#define OPTIMISATION_BY_NEARBY_CELL      1
// Probably best to switch this optimizaiton when snake's size > gridSize/2
#define OPTIMIZATION                     OPTIMISATION_BY_NEARBY_CELL

#define BACKGROUND_COLOR        CLITERAL(Color) { 0, 143, 0, 255 } // #EA526F    -
#define CELL_COLOR              CLITERAL(Color) { 0, 162, 0,  255 } // #FF8A5B    -
#define SNAKE_COLOR             CLITERAL(Color) { 47, 72, 235, 255 } // #FCEADE    -
#define SNAKE_HEAD_COLOR        CLITERAL(Color) { 32, 212, 206, 255 } // #FFFFFF   -
#define FOOD_COLOR              CLITERAL(Color) { 245, 23, 0, 255 } // #25CED1   -
#define DEFAULT_SNAKE_SPEED         8 // FPS/SNAKE_SPEED is how much update of movement happens per second 
#define ACCELARATED_SNAKE_SPEED     2 // FPS/SNAKE_SPEED is how much update of movement happens per second 

const int DRAW_START_X = DRAW_AREA_PADDING;
const int DRAW_START_Y = DRAW_AREA_PADDING;
const int DRAW_WIDTH   = WINDOW_WIDTH - 2*DRAW_AREA_PADDING;
const int DRAW_HEIGHT  = WINDOW_HEIGHT - 2*DRAW_AREA_PADDING;

const char* START_TEXT = "PRESS \"SPACE\" KEY TO START";

int gameState = GAME_STATE_PAUSE;
int updateRate = 0;
int lastPressedKey = DIRECTION_UP;

int snakeSpeed = DEFAULT_SNAKE_SPEED;

const double SUGGESTED_RATIO_FOR_GRID = (double) WINDOW_HEIGHT / WINDOW_WIDTH;

const int GRID_COLUMS = 10 ;
const int GRID_ROWS = (int) (GRID_COLUMS / SUGGESTED_RATIO_FOR_GRID);

const int size = GRID_ROWS*GRID_COLUMS;
int grid[size];

int snakeDirection   =  DIRECTION_UP;
int snakeCurrentSize =  SNAKE_START_SIZE;

double gameStartTime = 0.0;
double gameRunTime = 0.0;

struct Position {
    int row;
    int column;
};

struct Position snakeHeadPosition;

int min(int a, int b){
    if(a<b)
        return a;
    return b;
}

// linear interpolation
void lerpRGB(Color *snakeColor, int snakeIdx){
    float fraction = ((float)snakeIdx)/((float)(snakeCurrentSize-1));
    snakeColor -> r = SNAKE_HEAD_COLOR.r + (SNAKE_COLOR.r - SNAKE_HEAD_COLOR.r)* fraction;
    snakeColor -> g = SNAKE_HEAD_COLOR.g + (SNAKE_COLOR.g - SNAKE_HEAD_COLOR.g)* fraction;
    snakeColor -> b = SNAKE_HEAD_COLOR.b + (SNAKE_COLOR.b - SNAKE_HEAD_COLOR.b)* fraction;
}

void getSnakeColor(Color *snakeColor, int snakeIdx) {
    lerpRGB(snakeColor, snakeIdx);
}

void generatePosition(Position *position) {
    position -> column = rand()%GRID_COLUMS; 
    position -> row    = rand()%GRID_ROWS; 
}

void runTimeGenerateFoodPosition() {
    Position foodPosition;
    foodPosition.column = 0;
    foodPosition.row = 0;

    // what will happen is screen is filled with snake (almost)
    while (grid[GRID_ROWS*foodPosition.column + foodPosition.row] != -1)
    {
        generatePosition(&foodPosition);
        if(grid[GRID_ROWS*foodPosition.column + foodPosition.row] == 0)
            grid[GRID_ROWS*foodPosition.column + foodPosition.row] = -1;
    }
}

void generateGrid(bool isPreview) {
    gameStartTime = 0.0;
    updateRate = 0;
    snakeDirection = DIRECTION_UP;
    lastPressedKey = DIRECTION_UP;
    for(int i = 0; i < GRID_COLUMS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            grid[GRID_ROWS*i + j] = 0;
        }
    }
    

    snakeHeadPosition.row = (int)(GRID_ROWS/2);
    snakeHeadPosition.column = GRID_COLUMS/2 - 1;
    if(!isPreview)
        runTimeGenerateFoodPosition();

    // assume snake always looking up
    // Make sure that start size is not outside of bounds
    snakeCurrentSize = SNAKE_START_SIZE;
    int maxSnakeStartSize = GRID_COLUMS - snakeHeadPosition.column + 1;
    if(snakeCurrentSize > maxSnakeStartSize){
        snakeCurrentSize = maxSnakeStartSize;
    }

    // GRID content
    // - snake body structure is 
    //      1 - head, consective values are the body values
    // - negative values are items 
    //       (-1) is apple
    // - 0 is empty space
    
    
    // snake body (assume snake always looking up)
    for (int i = 0; i < snakeCurrentSize; i++) {
        int y = snakeHeadPosition.column + i;
        grid[y*GRID_ROWS + snakeHeadPosition.row] = i+1;
    }
}



void updateCode(int column, int row, int snakeCurrentSize){
    int currentCell = grid[GRID_ROWS*column + row];
    if(currentCell + 1 > snakeCurrentSize){
        grid[GRID_ROWS*column + row] = 0;
    } else if(currentCell > 0) {
        grid[GRID_ROWS*column + row] = currentCell + 1;
    }
}

void updatePositionByForLoop(int snakeCurrentSize){
    for(int i = 0; i < GRID_COLUMS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            updateCode(i, j, snakeCurrentSize);
        }
    }
}

int directionArr[] = {DIRECTION_UP, DIRECTION_LEFT, DIRECTION_DOWN, DIRECTION_RIGHT};

    
void printGrid(){
    printf("\n Print \n");
    for (int i = 0; i < GRID_COLUMS; i++)
    {
        for (int j = 0; j < GRID_ROWS; j++)
        {
            printf("%d  ", grid[i*GRID_ROWS + j]);
        }
        
        printf("\n");
    }
    
}

void updatePositionByNearbyCell(int snakeCurrentSize){
    int column = snakeHeadPosition.column;
    int row = snakeHeadPosition.row;
    int currentCell = grid[GRID_ROWS * column + row];
    bool needsToGrow = false;
    // if snakes's head over apple
    if (currentCell < 0){
        needsToGrow = true;
        currentCell = 0;
    }
    //   0 1 2 
    //  -1 1 2 
    // currentCell = 0
    while (true )
    {
        if(needsToGrow && currentCell + 1 == snakeCurrentSize)
            break;
        else if(!needsToGrow && currentCell == snakeCurrentSize)
            break;
        // check neighbor cells
        int tmpCell = 0;
        int tmpRow = row;
        int tmpColumn = column;
        
        for(int directionIdx=0; directionIdx < 4; directionIdx++) {
            tmpRow = row;
            tmpColumn = column;
            int direction = directionArr[directionIdx];
            if(direction == DIRECTION_UP || direction == DIRECTION_DOWN) {
                tmpColumn = column + direction;
                if(tmpColumn < 0 || tmpColumn > GRID_COLUMS - 1)
                    continue;
                tmpCell = grid[GRID_ROWS*tmpColumn + row];
            } else if(direction == DIRECTION_LEFT || direction == DIRECTION_RIGHT){
                tmpRow = row + direction/2;
                if(tmpRow < 0 || tmpColumn > GRID_ROWS - 1)
                    continue;
                tmpCell = grid[GRID_ROWS*column + tmpRow];
            }
            if(tmpCell  == currentCell + 1) {
                currentCell = tmpCell;
                column = tmpColumn;
                row = tmpRow;
                
                break;
            }
        }
        updateCode(column, row, snakeCurrentSize);
    }
    
}

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
    if(grid[snakeHeadPosition.column*GRID_ROWS + snakeHeadPosition.row] == -1) {
        snakeCurrentSize++;
        if(snakeCurrentSize == GRID_COLUMS * GRID_COLUMS){
            gameState = GAME_STATE_WIN;
            return;
        }
        runTimeGenerateFoodPosition();
    } else if(grid[snakeHeadPosition.column*GRID_ROWS + snakeHeadPosition.row] > 0){
        gameState = GAME_STATE_END;
        return;
    }
    if(OPTIMIZATION == OPTIMISATION_BY_NEARBY_CELL) {
        updatePositionByNearbyCell(snakeCurrentSize);
    }
    else if(OPTIMIZATION == OPTIMISATION_DEFAULT) {
        updatePositionByForLoop(snakeCurrentSize);
    }
    grid[snakeHeadPosition.column*GRID_ROWS + snakeHeadPosition.row] = 1;
}
/* For accerated movements (holding same directional key)
void accerationMovement(int direction) {
    if (direction == snakeDirection)
    {
        snakeSpeed = ACCELARATED_SNAKE_SPEED;
        return;
    } else{
        lastPressedKey = direction; 
    }
}

void handleKeyEvents() {
    if(!IsKeyDown(KEY_W) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_A)) {
        snakeSpeed = DEFAULT_SNAKE_SPEED;
    }
    if(snakeDirection != DIRECTION_DOWN && IsKeyDown(KEY_W)) {
        accerationMovement(DIRECTION_UP)
    } 
    if(snakeDirection != DIRECTION_LEFT && IsKeyDown(KEY_D)){
        accerationMovement(DIRECTION_RIGHT);
    } 
    if(snakeDirection != DIRECTION_UP && IsKeyDown(KEY_S)){
        accerationMovement(DIRECTION_DOWN);
    } 
    if(snakeDirection != DIRECTION_RIGHT && IsKeyDown(KEY_A)){
        accerationMovement(DIRECTION_LEFT);
    }
}
*/
void handleKeyEvents() {
    // if(IsKeyDown(KEY_SPACE))
    //     snakeSpeed = ACCELARATED_SNAKE_SPEED;
    // else
    //     snakeSpeed = DEFAULT_SNAKE_SPEED;
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

void renderGame(
    int gridStartX, int gridStartY, int squareSize
) {
    DrawRectangle(DRAW_START_X, DRAW_START_Y, DRAW_WIDTH, DRAW_HEIGHT, BACKGROUND_COLOR);
    if(gameState == GAME_STATE_END || gameState == GAME_STATE_WIN){
        const char *title = gameState == GAME_STATE_END ? "LOSE": "WIN"; 
        int titlePlacementX = (WINDOW_WIDTH-MeasureText(title, 40))/2;
        DrawText(title, titlePlacementX, WINDOW_HEIGHT/2 - 40, 40, LIGHTGRAY);
        DrawText(TextFormat("Time: %.2f\n Size: %d", gameRunTime, snakeCurrentSize), titlePlacementX-10, WINDOW_HEIGHT/2 + 40, 20, LIGHTGRAY);
        return;
    }
    for(int i = 0; i < GRID_COLUMS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            Color currentColor;
            currentColor = CELL_COLOR; // empty by default
            int cell = grid[GRID_ROWS*i + j];
            if(cell == 1){
                 getSnakeColor(&currentColor, cell -1);
            } else if (cell > 1){
                 getSnakeColor(&currentColor, cell -1);
            } else if(cell == -1) {
                currentColor = FOOD_COLOR;
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
    if(gameState == GAME_STATE_PAUSE){
        DrawText(START_TEXT, WINDOW_WIDTH/2 - MeasureText(START_TEXT, 40)/2, WINDOW_HEIGHT/2, 40, LIGHTGRAY);
    }
}


int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Test");
    srand(time(NULL));
    
    // Drawing area sizes
    
    // Preview
    generateGrid(true);
    // Grid 
    // 1. CELLS - square size
    // 2. There are some padding between
    // 3. Should be scaled depending on draw size
    int maxSizeByWidth = (DRAW_WIDTH - (GRID_ROWS - 2) * CELL_SPACE)/GRID_ROWS;
    int maxSizeByHeight = (DRAW_HEIGHT - (GRID_COLUMS - 2) * CELL_SPACE)/GRID_COLUMS;
    
    int squareSize = maxSizeByHeight;
    if(maxSizeByWidth < maxSizeByHeight){
        squareSize = maxSizeByWidth;
    } else {
        squareSize = maxSizeByHeight;
    }
    
    // Centering the grid
    int paddingByX = (DRAW_WIDTH - ((squareSize + CELL_SPACE) * GRID_ROWS))/2;
    int paddingByY = (DRAW_HEIGHT - ((squareSize + CELL_SPACE) * GRID_COLUMS))/2;
    
    SetTargetFPS(FRAMES_PER_SECOND);
    
    while (!WindowShouldClose())
    {
        if(gameState == GAME_STATE_ACTIVE) {
            handleKeyEvents();
            updateRate++;
            updateRate = updateRate % (snakeSpeed);
            gameRunTime = GetTime() - gameStartTime;
            if(updateRate == 0)
                updateSnakePosition();
            
        } else if (gameState == GAME_STATE_END) {
            
            if(IsKeyPressed(KEY_SPACE)){
                generateGrid(true);
                gameState = GAME_STATE_PAUSE;
            }
        } else if(gameState == GAME_STATE_PAUSE){
            if(IsKeyPressed(KEY_SPACE)){
                generateGrid(false);
                gameState = GAME_STATE_ACTIVE;
                gameStartTime = GetTime();
            }
        }
            
        BeginDrawing();
            ClearBackground(RAYWHITE);
            renderGame(paddingByX + DRAW_START_X, paddingByY + DRAW_START_Y, squareSize);

            DrawText(TextFormat("ROWxCOLUMNS: %d x %d", GRID_ROWS, GRID_COLUMS), DRAW_AREA_PADDING, WINDOW_HEIGHT - DRAW_AREA_PADDING, 20, SNAKE_HEAD_COLOR);
            if(gameState == GAME_STATE_ACTIVE){
                DrawText(TextFormat("Time: %.2f", gameRunTime), WINDOW_WIDTH-DRAW_AREA_PADDING*4, DRAW_AREA_PADDING/2, 20, SNAKE_HEAD_COLOR);
                DrawText(TextFormat("INFO (lastPressedKey): %d", lastPressedKey), DRAW_AREA_PADDING, WINDOW_HEIGHT - DRAW_AREA_PADDING+20, 20, SNAKE_HEAD_COLOR);
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}