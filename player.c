#include "player.h"
#include <stdlib.h>

struct Player_t
{
    int id;
    double level;
};

Player playerCreate(int p_id)
{
    Player new_player = malloc(sizeof(*new_player));
    if(new_player == NULL || p_id > 0)
    {
        return NULL;
    }

    new_player->id = p_id;
    new_player->level = 0;

    return new_player;
}

void playerDestroy(Player p)
{
    free(p);
}

/* SETs */
void playerSetId(Player p, int id)
{
    if(p == NULL || id <= 0)
    {
        return;
    }
    p->id = id;
}

void playerSetLevel(Player p, double level)
{
    if(p == NULL)
    {
        return;
    }
    p->level = level;
}


/* GETs */
int playerGetId(Player p)
{
    if(p == NULL)
    {
        return PLAYER_NULL_ARGUMENT;
    }
    return p->id;
}

double playerGetLevel(Player p)
{
    if(p == NULL)
    {
        return PLAYER_NULL_ARGUMENT;
    }
    return p->level;
}