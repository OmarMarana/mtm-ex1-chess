#include "player.h"
#include <stdlib.h>
#include <stdbool.h>

struct Player_t
{
    int id;
    double level;
    int game_wins;
    int game_losses;
    int game_draws;
    double average_play_time;
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
    new_player->game_wins = 0;
    new_player->game_losses = 0;
    new_player->game_draws = 0;
    new_player->average_play_time = 0;

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

void playerSetGameStatics(Player player, PlayerStaticsMode mode, int value)
{
    if(player == NULL || value < 0)
    {
        return;
    }

    switch (mode)
    {
    case PLAYER_WINS:
        player->game_wins = value;
        break;

    case PLAYER_LOSSES:
        player->game_losses = value;
        break;

    case PLAYER_DRAWS:
        player->game_draws = value;
        break;

    default:
        return;
    }
}

void playerSetAvgPlayTime(Player player, double average_play_time)
{
    if(player == NULL || average_play_time < 0)
    {
        return;
    }

    player->average_play_time = average_play_time;
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

int playerGetGameStatics(Player player, PlayerStaticsMode mode)
{
    if(player == NULL)
    {
        return PLAYER_NULL_ARGUMENT;
    }

    switch (mode)
    {
    case PLAYER_WINS:
        return player->game_wins;

    case PLAYER_LOSSES:
        return player->game_losses;


    case PLAYER_DRAWS:
        return player->game_draws;

    default:
        return PLAYER_NULL_ARGUMENT;
    }
}

double playerGetAvgPlayTime(Player player)
{
    if(player == NULL)
    {
        return 0;
    }

    return player->average_play_time;
}