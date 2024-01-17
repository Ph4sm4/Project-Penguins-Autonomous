#include "Grid.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "time.h"
#include "../Player/Player.h"
#include "../Enums/ExceptionHandler.h"

// =========================================
// available public functions:

struct GameGrid createGameGridObject();
enum ExceptionHandler readGridData(struct Player *myPlayer, struct GameGrid *gameGrid);
enum ExceptionHandler writeGridData(struct Player *myPlayer, struct GameGrid *gameGrid);

// private functions:

void initializeGrid(struct GameGrid *gameGrid);
enum ExceptionHandler initializeMyPlayer(const struct GameGrid *gameGrid, struct Player *player);

// =========================================

struct GameGrid createGameGridObject()
{
    struct GameGrid obj;

    obj.readGridData = &readGridData;
    obj.writeGridData = &writeGridData;

    obj.inputFile = malloc(100 * sizeof(char));
    obj.outputFile = malloc(100 * sizeof(char));

    return obj;
}

enum ExceptionHandler writeGridData(struct Player *myPlayer, struct GameGrid *gameGrid)
{
    FILE *file = fopen(gameGrid->outputFile, "w");
    if (file == NULL)
        return (enum ExceptionHandler)FileOpenException;

    // print grid size
    fprintf(file, "%d %d\n", gameGrid->rows, gameGrid->cols);

    // print grid content
    for (int i = 0; i < gameGrid->rows; i++)
    {
        for (int j = 0; j < gameGrid->cols; j++)
        {
            struct GridPoint *t = &gameGrid->grid[i][j];
            fprintf(file, "%d%d ", t->numberOfFishes, t->owner ? t->owner->id : 0);
        }
        fprintf(file, "\n");
    }

    // print player data
    for (int i = 0; i < gameGrid->gameInstance->numberOfPlayers; i++)
    {
        if (i + 1 == myPlayer->id)
        {
            char buffer[100];
            sprintf(buffer, "%s %d %d", myPlayer->name, myPlayer->id, myPlayer->collectedFishes);
            printf("\nbuffer: %s", buffer);
            fprintf(file, buffer);
            fprintf(file, "\n");
        }
        else
        {
            printf("\nplayer: %s", gameGrid->gameInstance->fullPlayersData[i]);

            fprintf(file, gameGrid->gameInstance->fullPlayersData[i]);
            fprintf(file, "\n");
        }
    }

    fclose(file);
    return (enum ExceptionHandler)NoError;
}

enum ExceptionHandler readGridData(struct Player *myPlayer, struct GameGrid *gameGrid)
{

    enum ExceptionHandler playerInitResult = initializeMyPlayer(gameGrid, myPlayer);
    if (playerInitResult != NoError)
        return playerInitResult;

    FILE *inputFile = fopen(gameGrid->inputFile, "r");
    if (inputFile == NULL)
        return (enum ExceptionHandler)FileOpenException;

    // read grid dimensions
    fscanf(inputFile, "%d %d", &gameGrid->rows, &gameGrid->cols);

    // allocate memory for grid points
    initializeGrid(gameGrid);

    // read grid content
    for (int i = 0; i < gameGrid->rows; i++)
    {
        for (int j = 0; j < gameGrid->cols; j++)
        {
            char *field = malloc(2 * sizeof(char));
            fscanf(inputFile, "%s", field);

            struct GridPoint *p = &gameGrid->grid[i][j];

            (*p).x = i;
            (*p).y = j;
            // we do not care about the owner, we just need to know
            // that the tile belongs to somebody and so the pointer cannot be null :)
            if (field[1] - '0' == myPlayer->id)
            {
                (*p).owner = myPlayer;
                gameGrid->gameInstance->numberOfPlacedPenguins++;
            }
            else
            {
                struct Player tempPlayer = createPlayerObject();
                tempPlayer.id = field[1] - '0';
                if (tempPlayer.id == 0)
                {
                    (*p).owner = NULL;
                }
                else
                {
                    (*p).owner = (struct Player *)malloc(sizeof(struct Player));
                    *((*p).owner) = tempPlayer;
                }
            }

            (*p).numberOfFishes = field[0] - '0';
        }
    }

    fclose(inputFile);
    return (enum ExceptionHandler)NoError;
}

enum ExceptionHandler initializeMyPlayer(const struct GameGrid *gameGrid, struct Player *player)
{
    FILE *file = fopen(gameGrid->inputFile, "r");
    if (file == NULL)
        return (enum ExceptionHandler)FileOpenException;

    char buffer[1000];

    // rows + 1 cause of the first line being the dimensions of the grid
    char *field = malloc(50 * sizeof(char));

    int rows, cols;
    // read grid dimensions
    fscanf(file, "%d %d", &rows, &cols);

    // skip reading the lines containing grid data
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fscanf(file, "%s", field);
        }
    }

    char *playerName = malloc(100 * sizeof(char));
    int playerId = 0;
    int playerPoints;

    bool found = false;

    // loop through data related to players
    while (fscanf(file, "%s %d %d", playerName, &playerId, &playerPoints) == 3)
    {
        char buffer[100];
        sprintf(buffer, "%s %d %d", playerName, playerId, playerPoints); // buffer with the formatted string: playerName playerId playerScore
        strcpy(gameGrid->gameInstance->fullPlayersData[gameGrid->gameInstance->numberOfPlayers], buffer);

        gameGrid->gameInstance->numberOfPlayers++;

        // our player nickname found
        if (!strcmp(player->name, playerName))
        {
            found = true;
            player->id = playerId;
            player->collectedFishes = playerPoints;
        }
    }
    if (!found)
    {
        player->id = !gameGrid->gameInstance->numberOfPlayers ? 1 : playerId + 1; // playerId will hold the id of the last player written in the file
        gameGrid->gameInstance->numberOfPlayers++;
    }

    for (int i = 0; i < gameGrid->gameInstance->numberOfPlayers; i++)
    {
        printf("\nPlayer %d: %s\n", i + 1, gameGrid->gameInstance->fullPlayersData[i]);
    }

    printf("\nour player name, id and points: %s %d %d", player->name, player->id, player->collectedFishes);

    fclose(file);

    return (enum ExceptionHandler)NoError;
}

void initializeGrid(struct GameGrid *gameGrid)
{
    const int rows = gameGrid->rows;
    const int cols = gameGrid->cols;

    // allocate memory for array of pointers (to another arrays)
    gameGrid->grid = (struct GridPoint **)malloc(rows * sizeof(struct GridPoint *));
    // initialize memory for arrays of pointers (to GridPoint objects)
    for (int i = 0; i < rows; i++)
    {
        gameGrid->grid[i] = (struct GridPoint *)malloc(cols * sizeof(struct GridPoint));
    }
}