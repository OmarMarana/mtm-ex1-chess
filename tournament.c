#include <stdlib.h>
#include "tournament.h"
#include "game.h"
#include "player.h"


struct tournament_t
{
    int id;
    int max_games_per_player;
    const char* location;
    bool finished;
    int tournament_winner_id;
    Map game_list;
};

Tournament tournamentCreate(int tournament_id, const char* tournament_location, int max_games_per_player)
{
    if(tournament_id <= 0 || tournament_location == NULL || max_games_per_player <= 0)
    {
        return NULL;
    }

    Tournament tour_to_create = malloc(sizeof(*tour_to_create));
 //   Map games = NULL;//mapCreate(copyGameFunc, copyGameIdFunc, freeGameFunc, freeGameIdFunc, compareGameId);

    if(tour_to_create == NULL)// || games == NULL)
    {
        return NULL;
    }
    
    tour_to_create->finished = false;
    tour_to_create->id = tournament_id;
    tour_to_create->location = tournament_location;
    tour_to_create->max_games_per_player = max_games_per_player;
    tour_to_create->tournament_winner_id = PLAYER_INVALID_ID;
    //tour_to_create->game_list = NULL//games;

    return tour_to_create;
}

void tournamentDestroy(Tournament tournament)
{
    mapDestroy(tournament->game_list);
    free(tournament);
}



/* ********************** */
/* ******** SET ********* */
/* ********************** */

void tournamentSetId(Tournament tournament, int id)
{
    if(tournament == NULL || id <= 0)
    {
        return;
    }

    tournament->id = id;
}

void tournamentSetMaxGames(Tournament tournament, int max_games_per_player)
{
    if(tournament == NULL || max_games_per_player <= 0)
    {
        return;
    }

    tournament->max_games_per_player = max_games_per_player;
}

void tournamentSetLocation(Tournament tournament, char* location)
{
    if(tournament == NULL || location == NULL)
    {
        return;
    }

    tournament->location = location;
}

void tournamentSetFinishedState(Tournament tournament, bool finished)
{
    if(tournament == NULL)
    {
        return;
    }

    tournament->finished = finished;
}

void tournamentSetWinnderId(Tournament tournament, int winner_id)
{
    if(tournament == NULL || winner_id <= 0)
    {
        return;
    }

    tournament->tournament_winner_id = winner_id;
}



/* ********************** */
/* ******** GET ********* */
/* ********************** */

int tournamentGetId(Tournament tournament)
{
    if(tournament == NULL)
    {
        return TOURNAMENT_NULL_ARGUMENT;
    }

    return tournament->id;
}

int tournamentGetMaxGames(Tournament tournament)
{
    if(tournament == NULL)
    {
        return TOURNAMENT_NULL_ARGUMENT;
    }

    return tournament->max_games_per_player;
}

const char* tournamentGetLocation(Tournament tournament)
{
    if(tournament == NULL)
    {
        return NULL;
    }

    return tournament->location;
}

bool tournamentGetFinishedState(Tournament tournament)
{
    if(tournament == NULL)
    {
        return false;
    }

    return tournament->finished;
}

Map tournamentGetGames(Tournament tournament)
{
    if(tournament == NULL)
    {
        return NULL;
    }

    return tournament->game_list;
}

int tournamentGetWinnderId(Tournament tournament)
{
    if(tournament == NULL)
    {
        return TOURNAMENT_NULL_ARGUMENT;
    }

    return tournament->tournament_winner_id;
}

int tournamentGetGamesPlayed(Tournament tournament)
{
    if(tournament == NULL)
    {
        return TOURNAMENT_NULL_ARGUMENT;
    }

    if(tournament->game_list == NULL)
    {
        return 0;
    }

    return mapGetSize(tournament->game_list);
}
