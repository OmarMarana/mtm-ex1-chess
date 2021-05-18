#include <stdlib.h>
#include "chessSystem.h"
#include "map.h"
#include "tournament.h"
#include "player.h"

struct chess_system_t
{
    Map tournaments;
    Map players;
};


/* ******************************************* */
/* copy/free/compare functions for the Map ADT */
/* ******************************************* */

static int compareInts(MapKeyElement element1, MapKeyElement element2);
static void freePlayer(MapKeyElement element);
static void freeTournament(MapKeyElement element);
static void freeInt(MapKeyElement element);
static MapDataElement copyDataPlayer(MapDataElement element);
static MapDataElement copyDataTournament(MapDataElement element);
static MapKeyElement copyKeyInt(MapKeyElement element);


ChessSystem chessCreate()
{
    ChessSystem chessSystem = malloc(sizeof(*chessSystem));
    if(chessSystem == NULL)
    {
        return NULL;
    }

    //key element is the tournament id. (int)
    Map tournaments = mapCreate(copyDataTournament, copyKeyInt, freeTournament, freeInt, compareInts);
    if(tournaments == NULL)
    {
        return NULL;
    }

    //key element is the player id. (int)
    Map players = mapCreate(copyDataPlayer, copyKeyInt, freePlayer, freeInt, compareInts);
    if(players == NULL)
    {
        return NULL;
    }

    chessSystem->players = players;
    chessSystem->tournaments = tournaments;

    return chessSystem;
}

/** Function to be used for copying an int as a key to the map */
static MapKeyElement copyKeyInt(MapKeyElement element)
{
    if (element == NULL)
    {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (copy == NULL)
    {
        return NULL;
    }

    *copy = *(int *) element;

    return copy;
}

/** Function to be used for copying a char as a data to the map */
static MapDataElement copyDataTournament(MapDataElement element)
{
    if (element == NULL)
    {
        return NULL;
    }

    int cpy_id = tournamentGetId((Tournament) element);
    char* cpy_location = tournamentGetLocation((Tournament) element);
    int cpy_max_games = tournamentGetMaxGames((Tournament) element);
    if(cpy_id <= 0 || cpy_location == NULL || cpy_max_games <= 0 )
    {
        return NULL;
    }

    Tournament copy = tournamentCreate(cpy_id, cpy_location, cpy_max_games);
    if (copy == NULL)
    {
        return NULL;
    }

    return copy;
}

/** Function to be used for copying a char as a data to the map */
static MapDataElement copyDataPlayer(MapDataElement element)
{
    if (element == NULL)
    {
        return NULL;
    }

    int cpy_id = playerGetId((Player) element);
    double cpy_level = playerGetLevel((Player) element);

    if(cpy_id <= 0)
    {
        return NULL;
    }

    Player copy = playerCreate(cpy_id);
    if (copy == NULL)
    {
        return NULL;
    }

    return copy;
}

/** Function to be used by the map for freeing elements */
static void freeInt(MapKeyElement element)
{
    free(element);
}

/** Function to be used by the map for freeing elements */
static void freeTournament(MapKeyElement element)
{
    tournamentDestroy(element);
    free(element);
}

/** Function to be used by the map for freeing elements */
static void freePlayer(MapKeyElement element)
{
    playerDestroy(element);
    free(element);
}

/** Function to be used by the map for comparing elements */
static int compareInts(MapKeyElement element1, MapKeyElement element2)
{
    return (*(int *) element1 - *(int *) element2);
}

int main()
{
    return 0;
}