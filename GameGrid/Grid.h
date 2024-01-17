#ifndef GRID_H
#define GRID_H

#include "./GridPoint.h"
#include "../Enums/GameState.h"
#include "../Enums/ExceptionHandler.h"
#include "../GameSystem/GameSystem.h"

struct GameGrid
{
    int rows;
    int cols;
    struct GridPoint **grid;

    enum ExceptionHandler (*readGridData)(struct Player *myPlayer, struct GameGrid *gameGrid);
    enum ExceptionHandler (*writeGridData)(struct Player *myPlayer, struct GameGrid *gameGrid);

    char *inputFile;
    char *outputFile;

    struct GameSystem *gameInstance;

    // Function to check for blocked penguins on the grid and update the blocked penguins count
    void (*checkForBlockedPenguins)(struct GameGrid *gameGrid);
};

struct GameGrid createGameGridObject();

#endif