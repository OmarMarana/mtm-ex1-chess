#include <stdlib.h>
#include "tournament.h"
#include "game.h"
#include "player.h"


struct tournament_t
{
    int id;
    int max_games_per_player;
    char* location;
    bool finished;
    int tournament_winner;
    Map game_list;
};

Tournament tournamentCreate(int tournament_id, char* tournament_location, int max_games_per_player)
{
    if(tournament_id <= 0 || tournament_location == NULL || max_games_per_player <= 0)
    {
        return NULL;
    }

    Tournament tour_to_create = malloc(sizeof(*tour_to_create));
    Map games = mapCreate(copyGameFunc, copyGameIdFunc, freeGameFunc, freeGameIdFunc, compareGameId);

    if(tour_to_create == NULL || games == NULL)
    {
        return NULL;
    }

    tour_to_create->finished = false;
    tour_to_create->id = tournament_id;
    tour_to_create->location = tournament_location;
    tour_to_create->max_games_per_player = max_games_per_player;
    tour_to_create->tournament_winner = PLAYER_INVALID_ID;
    tour_to_create->game_list = games;

    return tour_to_create;
}