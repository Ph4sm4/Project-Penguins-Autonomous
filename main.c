#include <stdio.h>
#include <stdlib.h>
#include "./GameSystem/GameSystem.h"

int main(int argc, char *argv[])
{
    struct GameSystem *gameSystem = (struct GameSystem *)malloc(sizeof(struct GameSystem));
    *(gameSystem) = createGameSystemObject();

    gameSystem->gameGrid = (struct GameGrid *)malloc(sizeof(struct GameGrid));
    *(gameSystem->gameGrid) = createGameGridObject();
    gameSystem->gameGrid->gameInstance = (struct GameSystem *)malloc(sizeof(struct GameSystem));
    gameSystem->gameGrid->gameInstance = gameSystem;

    enum ExceptionHandler setupStatus = gameSystem->setup(gameSystem, argc, argv);
    if (setupStatus != (enum ExceptionHandler)NoError)
    {
        gameSystem->exitWithErrorMessage(setupStatus);
    }

    enum ExceptionHandler actionStatus = gameSystem->performAction(gameSystem);
    if (actionStatus != (enum ExceptionHandler)NoError)
    {
        gameSystem->exitWithErrorMessage(actionStatus);
    }

    return 0;
}