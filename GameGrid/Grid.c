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
enum ExceptionHandler readGridData(struct Player *myPlayer, const struct GameGrid *gameGrid);

void checkForBlockedPenguins(struct GameGrid *gameGrid);

// private functions:

bool canMoveToPoint(struct GameGrid *gameGrid, const int x, const int y);
void initializeGrid(struct GameGrid *gameGrid);
enum ExceptionHandler initializeMyPlayer(const struct GameGrid *gameGrid, struct Player *player);

// =========================================

struct GameGrid createGameGridObject()
{
    struct GameGrid obj;

    obj.checkForBlockedPenguins = &checkForBlockedPenguins;
    obj.readGridData = &readGridData;

    obj.inputFile = malloc(100 * sizeof(char));
    obj.outputFile = malloc(100 * sizeof(char));

    return obj;
}

enum ExceptionHandler readGridData(struct Player *myPlayer, const struct GameGrid *gameGrid)
{
    // allocate memory for grid points
    initializeGrid(gameGrid);

    enum ExceptionHandler playerInitResult = initializeMyPlayer(gameGrid, myPlayer);
    if (playerInitResult != NoError)
        return playerInitResult;

    FILE *inputFile = fopen(gameGrid->inputFile, "r");
    if (inputFile == NULL)
        return (enum ExceptionHandler)FileOpenException;

    // read grid dimensions
    fscanf(inputFile, "%d %d", &gameGrid->rows, &gameGrid->cols);

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
            (*p).owner = field[1] - '0' == myPlayer->id ? myPlayer : 1;
            (*p).removed = false;
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
    int numberOfPlayers = 0;

    // loop through data related to players
    while (fscanf(file, "%s %d %d", playerName, &playerId, &playerPoints) == 3)
    {
        printf("%s %d %d \n", playerName, playerId, playerPoints);
        numberOfPlayers++;

        if (!strcmp(player->name, playerName))
        {
            found = true;
            player->id = playerId;
            player->collectedFishes = playerPoints;
            break;
        }
    }
    if (found)
        printf("player name, id and points: %s %d %d", player->name, player->id, player->collectedFishes); // later delete this line and replace it with
    // opposite condition - less code
    else
    {
        player->id = !numberOfPlayers ? 1 : playerId + 1; // playerId will hold the id of the last player written in the file
    }

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

void checkForBlockedPenguins(struct GameGrid *gameGrid)
{
    for (int x = 0; x < gameGrid->rows; x++)
    {
        for (int y = 0; y < gameGrid->cols; y++)
        {
            if (gameGrid->grid[x][y].owner == NULL)
                continue;

            if (!gameGrid->grid[x][y].penguinBlocked && !(canMoveToPoint(gameGrid, x + 1, y) || canMoveToPoint(gameGrid, x - 1, y) ||
                                                          canMoveToPoint(gameGrid, x, y + 1) ||
                                                          canMoveToPoint(gameGrid, x, y - 1)))
            {
                gameGrid->grid[x][y].penguinBlocked = true;
                gameGrid->grid[x][y].owner->blockedPenguins++;
            }
        }
    }
}

bool canMoveToPoint(struct GameGrid *gameGrid, const int x, const int y)
{
    return gameGrid->grid[x][y].owner == NULL && gameGrid->grid[x][y].removed == false;
}
