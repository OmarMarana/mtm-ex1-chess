#include <stdio.h>
#include <stdlib.h>
#include "chessSystem.h"
#include "map.h"
#include "tournament.h"
#include "player.h"
#include <string.h>

#define LOCATION_SPACE_CHAR ' '
#define FIRST_CAPITAL_LETTER 'A'
#define LAST_CAPITAL_LETTER 'Z'
#define FIRST_NON_CAPITAL_LETTER 'a'
#define LAST_NON_CAPITAL_LETTER 'z'


//Colors for debugging.......
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
//end colors



struct chess_system_t
{
    Map tournaments;
    Map players;
};


/* ******************************************* */
/* SELF TEST FUNCTION * ONLY FOR DEBUGGING     */
/* ******************************************* */
static void selfTest_addTournament(ChessSystem my_chess, int id, int maxGames, const char* loc);

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



/* ********************** */
/* small helper functions */
/* ********************** */

static bool checkLocationValidation(const char* tournament_location);
static bool checkLetterIfNonCapitalOrSpace(char letter);
static bool checkLetterIfCapital(char letter);



/* ************************* */
/* ChessSystem ADT functions */
/* ************************* */

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

ChessResult chessAddTournament (ChessSystem chess, int tournament_id,
                                int max_games_per_player, const char* tournament_location)
{
    if(chess == NULL || tournament_location == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    if(checkLocationValidation(tournament_location) == false)
    {
        return CHESS_INVALID_LOCATION;
    }

    if (mapContains(chess->tournaments, &tournament_id))
    {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }

    if(max_games_per_player <= 0)
    {
        return CHESS_INVALID_MAX_GAMES;
    }

    Tournament new_tournament = tournamentCreate(tournament_id, tournament_location, max_games_per_player);
    if(new_tournament == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }

    MapResult putResult = mapPut(chess->tournaments, &tournament_id, new_tournament);

    if(putResult != MAP_SUCCESS)
    {
        return CHESS_OUT_OF_MEMORY;
    }

    return CHESS_SUCCESS;
}




/* **************************** */
/* static functions definitions */
/* **************************** */

/* if location start's with CAPITAL LETTER followed by non-capital letters - return true 
 if it's empty - return false 
 accept spaces made only by the char ' '(defined as LOCATION_SPACE_CHAR). 
 any other space will return false. */
static bool checkLocationValidation(const char* tournament_location)
{
    if(tournament_location == NULL)
    {
        return false;
    }

    int len = strlen(tournament_location);
    if(len == 0)
    {
        return false;
    }

    if(!checkLetterIfCapital(tournament_location[0]))
    {
        return false;
    }

    for (int i = 1; i < len; i++)
    {
        if(!checkLetterIfNonCapitalOrSpace(tournament_location[i]))
        {
            return false;
        }
    }

    return true;
}

/* reutrn true if the letter is a capital letter.*/
/* else return false */
static bool checkLetterIfCapital(char letter)
{
    return (letter >= FIRST_CAPITAL_LETTER && letter <= LAST_CAPITAL_LETTER);
}

/* reutrn true if the letter is a non-capital letter OR is a space*/
/* else return false */
static bool checkLetterIfNonCapitalOrSpace(char letter)
{
    return ( (letter >= FIRST_NON_CAPITAL_LETTER && letter <= LAST_NON_CAPITAL_LETTER) ||
             (letter == LOCATION_SPACE_CHAR) );
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
    const char* cpy_location = tournamentGetLocation((Tournament) element);
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

    playerSetLevel(copy, cpy_level);

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
    printf(ANSI_COLOR_BLUE "Test started...\n" ANSI_COLOR_RESET);

    ChessSystem my_chess;
    my_chess = chessCreate();
    if(my_chess == NULL)
    {
        printf(ANSI_COLOR_RED "couldn't create chess system! - FAIL!\n" ANSI_COLOR_RESET);
        return 0;
        
    }
    else
    {
        printf(ANSI_COLOR_GREEN "Chess System Created successfully.\n" ANSI_COLOR_RESET);
    }

    selfTest_addTournament(my_chess, 7, 1, "Haifa technion");

    printf("trying to add another tournament, with SAME id...\n");

    selfTest_addTournament(my_chess, 7, 7, "Bear sheva");

    printf("trying to add another tournament, with DIFFERENT id...\n");

    selfTest_addTournament(my_chess, 1, 8, "Bear sheva");

    selfTest_addTournament(my_chess, 5, 8, "Bear sheva");

    selfTest_addTournament(my_chess, 2, 8, "Bear sheva");

    printf("Printing all tournament's id:\n");

    int i = 1;
    MAP_FOREACH(int*, iterator, my_chess->tournaments)
    {
        printf("tournament #%d, id = %d\n", i, *( ((int*)iterator) ));
        i++;
    }



    printf(ANSI_COLOR_BLUE "TEST FINISHED!\n" ANSI_COLOR_RESET);
    return 0;
}

static void selfTest_addTournament(ChessSystem my_chess, int id, int maxGames, const char* loc)
{
    ChessResult tour_add_rslt = chessAddTournament(my_chess, id, maxGames, loc);

    switch (tour_add_rslt)
    {
    case CHESS_SUCCESS:
            printf(ANSI_COLOR_GREEN "Tournament Added successfully. id = %d", id);
            printf("\n" ANSI_COLOR_RESET);
        break;
    case CHESS_NULL_ARGUMENT:
        printf(ANSI_COLOR_RED "Error! NULL argument.\n" ANSI_COLOR_RESET);
    break;

        case CHESS_INVALID_ID:
        printf(ANSI_COLOR_RED "Error! invalid id.\n" ANSI_COLOR_RESET);
    break;

        case CHESS_INVALID_LOCATION:
        printf(ANSI_COLOR_RED "Error! invalid location.\n" ANSI_COLOR_RESET);
    break;

    case CHESS_INVALID_MAX_GAMES:
        printf( ANSI_COLOR_RED "Error! invalid max games per player.\n" ANSI_COLOR_RESET);
    break;

    case CHESS_TOURNAMENT_ALREADY_EXISTS:
        printf(ANSI_COLOR_RED "Error! tournament already exists.\n" ANSI_COLOR_RESET);
    break;

    default:
        printf(ANSI_COLOR_RED "Error! default error code!!!.\n" ANSI_COLOR_RESET);
        break;
    }
}