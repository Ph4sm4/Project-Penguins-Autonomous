#include "./GameSystem.h"
#include "stdio.h"
#include "string.h"
#include "Windows.h"
#include "../Enums/ExceptionHandler.h"
#include "../Enums/GameState.h"

#define welcomeLine() printf("\n---- PROJECT \"PENGUINS\" ----\n\n");

// =========================================
// available public functions:

enum ExceptionHandler setup(struct GameSystem *game, int argc, char *argv[]);
enum ExceptionHandler performAction(struct GameSystem *game);
void exitWithErrorMessage(enum ExceptionHandler error);

// private functions:

// Function to move a penguin from an initial point to a destination point
enum ExceptionHandler moveAPenguin(struct GameSystem *game, struct GridPoint *initialPoint, struct GridPoint *destination);

// Function to place a penguin on the grid
enum ExceptionHandler placeAPenguin(struct GameGrid *gameGrid, struct GameSystem *game);

// find a perfect placement point, e.g. 10 30 (the point returned will be the first one)
struct GridPoint *findPerfectPointToPlaceRowWise(struct GameGrid *gameGrid);

// find second best e.g. if there are no 'perfect' points available, then choose such a point which
// is in the same row as the point with the highest possible fish number and at the same time it has
// no obstacles between the initial point and the highest one
struct GridPoint *findSecondBestPointToPlaceRowWise(struct GameGrid *gameGrid);

// =========================================

struct GameSystem
createGameSystemObject()
{
    struct GameSystem obj;
    // setting all of the objects
    obj.phase = (enum GameState)Unset;
    obj.maxNumberOfPlayers = 9;
    obj.numberOfPlayers = 0;

    obj.fullPlayersData = (char **)malloc(obj.maxNumberOfPlayers * sizeof(char *)); // allocate memory for max players
    for (int i = 0; i < obj.maxNumberOfPlayers; i++)
    {
        obj.fullPlayersData[i] = (char *)malloc(1000 * sizeof(char)); // max length of a 1000 characters for a single player data line
        // e.g. the nickname, id and score
    }

    obj.myPlayer = createPlayerObject();

    // setting function references
    obj.setup = &setup;
    obj.performAction = &performAction;
    obj.exitWithErrorMessage = &exitWithErrorMessage;

    obj.numberOfPenguins = -1;

    return obj;
}

void exitWithErrorMessage(enum ExceptionHandler error)
{
    MessageBeep(MB_ICONEXCLAMATION);

    switch (error)
    {
    case UnknownParamsException:
    {
        MessageBox(NULL, "Unsupported command line parameters", "Error", MB_ICONWARNING);
        exit(3);
    }
    case PenguinsNumValueException:
    {
        MessageBox(NULL, "Unsupported number of penguins", "Error", MB_ICONWARNING);
        exit(3);
    }
    case GamePhaseValueException:
    {
        MessageBox(NULL, "Invalid game phase (should be: movement/placement)", "Error", MB_ICONWARNING);
        exit(3);
    }
    case FileFormatException:
    {
        MessageBox(NULL, "Invalid file format. Only .txt is supported", "Error", MB_ICONWARNING);
        exit(3);
    }
    case FileOpenException:
    {
        MessageBox(NULL, "Error while opening the file. Please provide a valid path (relative to the .exe file)", "Error", MB_ICONWARNING);
        exit(3);
    }
    case MoveImpossible:
    {
        MessageBox(NULL, "Impossible to make a move for current phase", "Error", MB_ICONWARNING);
        exit(3);
    }
    }
}

enum ExceptionHandler setup(struct GameSystem *game, int argc, char *argv[])
{
    // argv will contain:
    // 1) phase=placement penguins=3 inputboardfile.txt output.txt
    // or:
    // 2) phase=movement inputboard.xt outputboard.txt
    // or at last:
    // name -> program returns the name of our player

    for (int i = 0; i < argc; i++)
    {
        printf("%s\n", argv[i]);
    }

    switch (argc)
    {
    case 5:
    {
        //
        //
        // PLACEMENT PHASE
        //
        //

        if (strncmp(argv[1], "phase=", 6) || strncmp(argv[2], "penguins=", 9))
            return (enum ExceptionHandler)UnknownParamsException;

        char *gamePhase = malloc(100 * sizeof(char));

        if (sscanf(argv[1], "phase=%s", gamePhase) == 0 || strcmp(gamePhase, "placement"))
            return (enum ExceptionHandler)GamePhaseValueException;

        game->phase = (enum GameState)PlacingPhase;

        if (sscanf(argv[2], "penguins=%d", &game->numberOfPenguins) == 0)
            return (enum ExceptionHandler)PenguinsNumValueException;

        const char *suffix = ".txt";
        size_t suffixLength = strlen(suffix);
        size_t inputLen = strlen(argv[3]); // lengths of file names
        size_t outputLen = strlen(argv[4]);

        if (inputLen >= suffixLength && strcmp(argv[3] + (inputLen - suffixLength), suffix) != 0 ||
            outputLen >= suffixLength && strcmp(argv[4] + (outputLen - suffixLength), suffix) != 0)
        {
            return (enum ExceptionHandler)FileFormatException;
        }

        game->gameGrid->inputFile = argv[3];
        game->gameGrid->outputFile = argv[4];

        enum ExceptionHandler readStatus = game->gameGrid->readGridData(&game->myPlayer, game->gameGrid);

        if (readStatus != NoError)
            return readStatus;

        break;
    }
    case 4:
    {
        break;
    }
    case 2:
    {
        if (strcmp(argv[1], "name"))
            return (enum ExceptionHandler)UnknownParamsException;

        const char *playerName = game->myPlayer.name;
        size_t msgSize = strlen("Player's name: ") + strlen(playerName) + 1;
        char *msg = (char *)malloc(msgSize);
        snprintf(msg, msgSize, "Player's name: %s", playerName);

        MessageBox(NULL, msg, "Information", MB_ICONINFORMATION);
        free(msg);

        exit(0);

        break;
    }
    default:
    {
        return (enum ExceptionHandler)UnknownParamsException;
    }
    }

    return (enum ExceptionHandler)NoError;
}

enum ExceptionHandler performAction(struct GameSystem *game)
{
    switch (game->phase)
    {
    case (enum GameState)PlacingPhase:
    {
        return placeAPenguin(game->gameGrid, game);
    }
    case (enum GameState)MovementPhase:
    {
        int penguinX, penguinY;
        int toX, toY; // move to

        if (game->myPlayer.blockedPenguins == game->numberOfPenguins)
        {
            // return an appropriate error code - the program is unable to make a move
            return (enum ExceptionHandler)MoveImpossible;
        }

        struct GridPoint *initialPoint = NULL;

        initialPoint->selected = true;

        struct GridPoint *destination;

        return moveAPenguin(game, initialPoint, destination);
    }
    }
}

enum ExceptionHandler placeAPenguin(struct GameGrid *gameGrid, struct GameSystem *game)
{
    const struct Player *ourPlayer = &game->myPlayer;

    int x, y;

    struct GridPoint *p = findPerfectPointToPlaceRowWise(gameGrid);
    if (p == NULL)
    {
        printf("\nperfect nulled");
        p = findSecondBestPointToPlaceRowWise(gameGrid);
    }
    if (p == NULL)
    {
        return (enum ExceptionHandler)MoveImpossible;
    }
    printf("\npoint chosen: %d %d %d", p->x, p->y, p->numberOfFishes);

    p->owner = &game->myPlayer;
    p->numberOfFishes = 0;
    game->myPlayer.collectedFishes++;

    return game->gameGrid->writeGridData(&game->myPlayer, game->gameGrid);
}

struct GridPoint *findPerfectPointToPlaceRowWise(struct GameGrid *gameGrid)
{
    // perfect means that we have found two adjecent cells where one of which is a 10 and the second one is 30
    // (a perfect place)

    // first left-right: 10 30
    for (int i = 0; i < gameGrid->rows; i++)
    {
        for (int j = 0; j < gameGrid->cols - 1; j++)
        {
            // printf("\nconsidering: %d %d %d and %d %d %d", i, j, gameGrid->grid[i][j].numberOfFishes, i, j + 1, gameGrid->grid[i][j + 1].numberOfFishes);
            if (gameGrid->grid[i][j].numberOfFishes == 1 && gameGrid->grid[i][j + 1].numberOfFishes == 3)
            {
                return &gameGrid->grid[i][j];
            }
        }
    }

    // secondly right-left: 30 10
    for (int i = 0; i < gameGrid->rows; i++)
    {
        for (int j = 1; j < gameGrid->cols; j++)
        {
            // printf("\nconsidering: %d %d %d and %d %d %d", i, j, gameGrid->grid[i][j].numberOfFishes, i, j + 1, gameGrid->grid[i][j + 1].numberOfFishes);
            if (gameGrid->grid[i][j].numberOfFishes == 1 && gameGrid->grid[i][j - 1].numberOfFishes == 3)
            {
                return &gameGrid->grid[i][j];
            }
        }
    }
    return NULL;
}

struct GridPoint *findSecondBestPointToPlaceRowWise(struct GameGrid *gameGrid)
{
    // perfect means that we have found two adjecent cells where one of which is a 10 and the second one is 30
    // (a perfect place)

    // first left-right: 10 10 10 20
    for (int fishNumber = 3; fishNumber >= 1; fishNumber--)
    {
        for (int i = 0; i < gameGrid->rows; i++)
        {
            for (int j = 0; j < gameGrid->cols; j++)
            {
                if (gameGrid->grid[i][j].numberOfFishes != 1)
                    continue;
                for (int k = j + 1; k < gameGrid->cols; k++)
                {
                    // we cannot allow any untraversable points to be in between
                    if (gameGrid->grid[i][k].owner != NULL)
                        break;
                    // this is the situation in terms of a row: 00 10 10 10 30
                    // then the second from left point is gonna get returned
                    if (gameGrid->grid[i][k].numberOfFishes == fishNumber)
                    {
                        return &gameGrid->grid[i][j];
                    }
                }
            }
        }
    }
    // secondly right-left: 20 10 10 10
    for (int fishNumber = 3; fishNumber >= 1; fishNumber--)
    {
        for (int i = 0; i < gameGrid->rows; i++)
        {
            for (int j = gameGrid->cols - 1; j >= 0; j--)
            {
                if (gameGrid->grid[i][j].numberOfFishes != 1)
                    continue;
                for (int k = j - 1; k >= 0; k--)
                {
                    // we cannot allow any untraversable points to be in between
                    if (gameGrid->grid[i][k].owner != NULL)
                        break;
                    // this is the situation in terms of a row: 00 10 10 10 30
                    // then the second from left point is gonna get returned
                    if (gameGrid->grid[i][k].numberOfFishes == fishNumber)
                    {
                        return &gameGrid->grid[i][j];
                    }
                }
            }
        }
    }

    // if all of them fail, just pick first 10 in the grid
    for (int i = 0; i < gameGrid->rows; i++)
    {
        for (int j = 0; j < gameGrid->cols; j++)
        {
            if (gameGrid->grid[i][j].numberOfFishes == 1)
                return &gameGrid->grid[i][j];
        }
    }

    // no available placement tile
    return NULL;
}

enum ExceptionHandler moveAPenguin(struct GameSystem *game, struct GridPoint *initialPoint, struct GridPoint *destination)
{
    initialPoint->removed = true;
    initialPoint->owner = NULL;

    // strcpy(destination->label, game->playerTurn == 0 ? "P1" : "P2");

    game->gameGrid->checkForBlockedPenguins(game->gameGrid);
}
