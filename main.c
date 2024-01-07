#include <stdio.h>
#include "./GameSystem/GameSystem.h"

int main(int argc, char *argv[])
{

    struct GameSystem gameSystem = createGameSystemObject();

    enum ExceptionHandler setupStatus = gameSystem.setup(&gameSystem, argc, argv);
    if (setupStatus != (enum ExceptionHandler)NoError)
    {
        gameSystem.exitWithErrorMessage(setupStatus);
    }

    return 0;
}