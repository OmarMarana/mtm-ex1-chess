#include "player.h"
#include <stdlib.h>

struct Player_t
{
    int id;
    double level;
};

Player playerCreate(int player_id)
{
    Player new_player = malloc(sizeof(*new_player));
    if(new_player == NULL || player_id <= 0)
    {
        return NULL;
    }

    new_player->id = player_id;
    new_player->level = 0;

    return new_player;
}

void playerDestroy(Player player)
{
    free(player);
}


/* ********************** */
/* ******** SET ********* */
/* ********************** */

void playerSetId(Player player, int id)
{
    if(player == NULL || id <= 0)
    {
        return;
    }
    player->id = id;
}

void playerSetLevel(Player player, double level)
{
    if(player == NULL)
    {
        return;
    }
    player->level = level;
}


/* ********************** */
/* ******** GET ********* */
/* ********************** */

int playerGetId(Player player)
{
    if(player == NULL)
    {
        return PLAYER_NULL_ARGUMENT;
    }
    return player->id;
}

double playerGetLevel(Player player)
{
    if(player == NULL)
    {
        return PLAYER_NULL_ARGUMENT;
    }
    return player->level;
}