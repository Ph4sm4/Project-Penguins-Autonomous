#include "GameSystem.h"
#include "stdio.h"
#include "string.h"
#include "Windows.h"
#include "../Enums/ExceptionHandler.h"
#include "../Enums/GameState.h"

#define welcomeLine() printf("\n---- PROJECT \"PENGUINS\" ----\n\n");

// =========================================
// available public functions:

enum ExceptionHandler setup(struct GameSystem *game, int argc, char *argv[]);
void askForAction(struct GameSystem *game);
void exitWithErrorMessage(enum ExceptionHandler error);

// private functions:

// Function to place a penguin on the grid
void moveAPenguin(struct GameSystem *game, struct GridPoint *initialPoint, struct GridPoint *destination);

// Function to move a penguin from an initial point to a destination point
void placeAPenguin(struct GameGrid *gameGrid, struct GameSystem *game);

// =========================================

struct GameSystem
createGameSystemObject()
{
    struct GameSystem obj;
    // setting all of the objects
    obj.gameGrid = createGameGridObject();
    obj.phase = (enum GameState)Unset;

    obj.myPlayer = createPlayerObject();

    // setting function references
    obj.setup = &setup;
    obj.askForAction = &askForAction;
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

    struct GameGrid grid = createGameGridObject();
    game->gameGrid = grid;

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

        game->gameGrid.inputFile = argv[3];
        game->gameGrid.outputFile = argv[4];

        enum ExceptionHandler readStatus = game->gameGrid.readGridData(&game->myPlayer, &game->gameGrid);
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

void askForAction(struct GameSystem *game)
{
    switch (game->phase)
    {
    case (enum GameState)PlacingPhase:
    {

        placeAPenguin(&game->gameGrid, game);

        break;
    }
    case (enum GameState)MovementPhase:
    {
        int penguinX, penguinY;
        int toX, toY; // move to

        if (game->myPlayer.blockedPenguins == game->numberOfPenguins)
        {
            // return an appropriate error code - the program is unable to make a move
            return;
        }

        struct GridPoint *initialPoint = NULL;
        if (initialPoint == NULL)
            return;

        initialPoint->selected = true;

        struct GridPoint *destination;

        moveAPenguin(game, initialPoint, destination);

        break;
    }
    }
}

void placeAPenguin(struct GameGrid *gameGrid, struct GameSystem *game)
{
    // point->owner = getCurrentPlayer(game);
    // point->removed = true;
    // strcpy(point->label, game->playerTurn == 0 ? "P1" : "P2");
}

void moveAPenguin(struct GameSystem *game, struct GridPoint *initialPoint, struct GridPoint *destination)
{
    initialPoint->removed = true;
    initialPoint->owner = NULL;

    // strcpy(destination->label, game->playerTurn == 0 ? "P1" : "P2");

    game->gameGrid.checkForBlockedPenguins(&game->gameGrid);
}
