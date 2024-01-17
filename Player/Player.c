#include "Player.h"
#include "stdlib.h"

struct Player createPlayerObject()
{
    struct Player obj;
    obj.collectedFishes = 0;
    obj.blockedPenguins = 0;
    obj.name = "Phasma";

    return obj;
}