#ifndef GRID_POINT_H
#define GRID_POINT_H

#include <stdbool.h>

struct GridPoint
{
    int x;
    int y;
    bool removed;
    int numberOfFishes;
    bool selected;
    bool penguinBlocked;

    struct Player *owner;
};

#endif