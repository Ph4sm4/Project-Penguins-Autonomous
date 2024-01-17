#ifndef GRID_POINT_H
#define GRID_POINT_H

#include <stdbool.h>

struct GridPoint
{
    int x;
    int y;
    int numberOfFishes;

    struct Player *owner;
};

#endif