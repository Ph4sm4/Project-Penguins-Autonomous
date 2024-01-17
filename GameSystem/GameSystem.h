#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

#include "../Player/Player.h"
#include "stdbool.h"
#include "../Enums/GameState.h"
#include "../Enums/ExceptionHandler.h"
#include "../GameGrid/Grid.h"

struct GameSystem
{
    struct GameGrid *gameGrid;
    struct Player myPlayer;

    int numberOfPenguins;

    enum GameState phase;

    char **fullPlayersData;

    int maxNumberOfPlayers;
    int numberOfPlayers;

    // Function to set up the game and read board data from a file
    enum ExceptionHandler (*setup)(struct GameSystem *game, int argc, char *argv[]);

    void (*exitWithErrorMessage)(enum ExceptionHandler error);

    // Function to ask for player action based on the current game phase (Placing, Movement, Finish)
    enum ExceptionHandler (*performAction)(struct GameSystem *game);
};

// creates the object and sets all default values including references to functions
struct GameSystem createGameSystemObject();

#endif