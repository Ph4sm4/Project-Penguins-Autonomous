#ifndef PLAYER_H
#define PLAYER_H

struct Player
{
    char *name;
    int id;
    int collectedFishes;

    int blockedPenguins;
};

// creates the object and allocates memory for the name
struct Player createPlayerObject();

#endif