#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chessSystem.h"
#include "map.h"
#include "tournament.h"
#include "player.h"
#include "game.h"

#define LOCATION_SPACE_CHAR ' '
#define FIRST_CAPITAL_LETTER 'A'
#define LAST_CAPITAL_LETTER 'Z'
#define FIRST_NON_CAPITAL_LETTER 'a'
#define LAST_NON_CAPITAL_LETTER 'z'
#define WIN_POINTS 2
#define DRAW_POINTS 1
#define PLAYER_STATS_COLS 5

typedef enum
{
    PLAYER_ID_COL = 0,
    PLAYER_POINTS_COL,
    PLAYER_LOSSES_COL,
    PLAYER_WINS_COL,
    PLAYER_DRAWS_COL
} PlayerStatsColumn;

/*REMOVE BEFORE SUBMIT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
//Colors for debugging.......
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
//end colors

/* Struct definition */
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
static void setOpponentTheWinner(Game game, int player_id);
static void sortMatrixByCol(int** mat, int len, int col, bool increasing);



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


ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    
    if(tournament_id <=0  || first_player <=0 || second_player <=0 || first_player == second_player || )

    
    

}


void chessDestroy(ChessSystem chess)
{
    if(chess == NULL)
    {
        return;
    }
    
    mapDestroy(chess->tournaments);
    mapDestroy(chess->players);

    free(chess);

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

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    
    Tournament tournament_to_remove = mapGet(chess->tournaments, &tournament_id);
    if(tournament_to_remove == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    /* Remove all of the games in the tournament */
    mapDestroy(tournamentGetGames(tournament_to_remove));
    
    /* Remove the tournament from the list and deallocate the memory. */
    MapResult remove_result = mapRemove(chess->tournaments, &tournament_id);

    return CHESS_SUCCESS;
}

ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if(player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    if(!mapContains(chess->players, &player_id))
    {
        return CHESS_PLAYER_NOT_EXIST;
    }


    int current_tournament_id = 0;
    MAP_FOREACH(int*, tournament_iterator, chess->tournaments)
    {
        current_tournament_id =  *tournament_iterator;
        Tournament current_tournament = mapGet(chess->tournaments, &current_tournament_id);
        if(tournamentGetFinishedState(current_tournament) == true)
        {
            continue;            
        }
        
        int current_game_id = 0;
        MAP_FOREACH(int*, game_iterator, tournamentGetGames(current_tournament))
        {
            current_game_id = *game_iterator;
            Game current_game = mapGet(tournamentGetGames(current_tournament), &current_game_id);

            setOpponentTheWinner(current_game, player_id);
        }
    }

    mapRemove(chess->players, &player_id);

    return CHESS_SUCCESS;
}

ChessResult chessEndTournament(ChessSystem chess, int tournament_id)
{
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    Tournament current_tournament = mapGet(chess->tournaments, &tournament_id);
    if(current_tournament == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    if(tournamentGetFinishedState(current_tournament))
    {
        return CHESS_TOURNAMENT_ENDED;
    }

    if(mapGetSize(tournamentGetGames(current_tournament)) <= 0)
    {
        return CHESS_NO_GAMES;
    }

    int winner_id = calcTournamentWinnerId(current_tournament);

    tournamentSetWinnderId(current_tournament, winner_id);
    tournamentSetFinishedState(current_tournament, true);

    return CHESS_SUCCESS;
}

/* **************************** */
/* helper functions definitions */
/* **************************** */

/* return the id of the tournament winner. */
static int calcTournamentWinnerId(Tournament tournament)
{
    if(tournament == NULL)
    {
        return TOURNAMENT_NULL_ARGUMENT;
    }

    int num_of_players = tournamentGetNumDiffPlayers(tournament);

    int** players_info = (int**) malloc(sizeof(int*) * num_of_players);
    if(players_info == NULL)
    {
        return TOURNAMENT_NULL_ARGUMENT;
    }

    int* hist_of_players_id = malloc(sizeof(*hist_of_players_id) * num_of_players);
    if(hist_of_players_id == NULL)
    {
        free(players_info);
        return TOURNAMENT_NULL_ARGUMENT;
    }

    allocatePlayersInfo(players_info, hist_of_players_id, num_of_players);
    initalizePlayerInfo(players_info, hist_of_players_id, num_of_players);
    initializeHist(hist_of_players_id, tournament, num_of_players);
    initializePlayerIdCol(players_info, hist_of_players_id, num_of_players);

    free(hist_of_players_id);

    sumWinLoseDraw(players_info, num_of_players, tournament);

    for (int i = 0; i < num_of_players; i++)
    {
        players_info[i][PLAYER_POINTS_COL] = WIN_POINTS * players_info[i][PLAYER_WINS_COL] + 
                                             DRAW_POINTS * players_info[i][PLAYER_DRAWS_COL];
    }

    //free mallocations
    if(sortMatrixByCol(players_info, &num_of_players, PLAYER_POINTS_COL, false) == 1)
    {
        freePlayersInfo(players_info, num_of_players);
        return players_info[0][PLAYER_ID_COL];
    }

    if(sortMatrixByCol(players_info, &num_of_players, PLAYER_LOSSES_COL, true) == 1)
    {
        freePlayersInfo(players_info, num_of_players);
        return players_info[0][PLAYER_ID_COL];
    }

    if(sortMatrixByCol(players_info, &num_of_players, PLAYER_WINS_COL, false) == 1)
    {
        freePlayersInfo(players_info, num_of_players);
        return players_info[0][PLAYER_ID_COL];
    }

    sortMatrixByCol(players_info, num_of_players, PLAYER_ID_COL, true);
    freePlayersInfo(players_info, num_of_players);
    return players_info[0][PLAYER_ID_COL];
}

static void freePlayersInfo(int** players_info, int len)
{
    for (int i = 0; i < len; i++)
    {
        free(players_info[i]);
    }
}

initializePlayerIdCol(int** players_info, int* hist_of_players_id, int num_of_players)
{
    for (int i = 0; i < num_of_players; i++)
    {
        players_info[i][PLAYER_ID_COL] = hist_of_players_id[i];
    }
}

static void initalizePlayerInfo(int** players_info, int* hist_of_players_id, int num_of_players)
{
    for (int p = 0; p < num_of_players; p++)
    {
        hist_of_players_id[p] = PLAYER_INVALID_ID;
        for (int i = 0; i < PLAYER_STATS_COLS; i++)
        {
            players_info[p][i] = 0;
        }
    }
}

static bool allocatePlayersInfo(int** players_info, int* hist_of_players_id, int num_of_players)
{
    for (int i = 0; i < num_of_players; i++)
    {
        players_info[i] = malloc(sizeof(int) * PLAYER_STATS_COLS);
        if(players_info[i] == NULL)
        {
            for (int t = 0; t < i; t++)
            {
                free(players_info[t]);
            }
            free(players_info);
            free(hist_of_players_id);
            
            return false;
        }
    }
    return true;
}

static int sortMatrixByCol(int** mat, int* len, int col, bool increasing)
{
    for (int i = 0; i < *len; i++)
    {
        for (int t = i+1; t < (*len) - 1; t++)
        {
            if(increasing)
            {
                if(mat[t][col] >= mat[t+1][col])
                {
                    swapRow(mat[t], mat[t+1]);
                }
            }
            else
            {
                if(mat[t][col] <= mat[t+1][col])
                {
                    swapRow(mat[t], mat[t+1]);
                }
            }
        }
    }


    for (int i = (*len)-1; i >= 0; i--)
    {
        if(mat[i][PLAYER_POINTS_COL] != mat[0][PLAYER_POINTS_COL])
        {
            free(mat[i]);
            (*len) = (*len) - 1;
        }
    }

    return (*len);
}

static void swapRow(int* a, int* b)
{
    int tmp[PLAYER_STATS_COLS];
    for (int i = 0; i < PLAYER_STATS_COLS; i++)
    {
        tmp[i] = a[i];
        a[i] = b[i];
        b[i] = tmp[i];
    }
}

static int getFirstIndexOfValue(int* arr, int len, int value)
{
    for (int i = 0; i < len; i++)
    {
        if(arr[i] == value)
        {
            return i;
        }
    }

    return PLAYER_INVALID_ID;
}

static int countSameValueInArr(int* arr, int len, int value)
{
    int c = 0;
    for (int i = 0; i < len; i++)
    {
        if(arr[i] == value)
        {
            c++;
        }
    }
    
    return c;
}

static int getMax(int** players_info, int num_of_players, int col)
{
    int max = 0;
    for (int i = 0; i < num_of_players; i++)
    {
        if(players_info[i][col] >= max)
        {
            max = players_info[i][col];
        }
    }
    return max;
}

static void sumWinLoseDraw(int** players_info, int num_of_players, Tournament tournament)
{
    MAP_FOREACH(int*, games_iterator, tournamentGetGames(tournament))
    {
        /*int current_game_id = *games_iterator; try to test with this int and without.... */
        Game current_game = mapGet(tournamentGetGames(tournament), games_iterator);

        int current_first_id = gameGetFirstPlayer(current_game);
        int current_second_id = gameGetSecondPlayer(current_game);
        Winner current_winner = gameGetWinner(current_game);

        int hist_index = PLAYER_INVALID_ID;
        switch (current_winner)
        {
        case FIRST_PLAYER:

            hist_index = getHistId(players_info, current_first_id, num_of_players);  //first
            players_info[hist_index][PLAYER_WINS_COL] ++;

            hist_index = getHistId(players_info, current_second_id, num_of_players); //second
            players_info[hist_index][PLAYER_LOSSES_COL] ++;

            break;
        
        case SECOND_PLAYER:

            hist_index = getHistId(players_info, current_second_id, num_of_players);  //second
            players_info[hist_index][PLAYER_WINS_COL] ++;

            hist_index = getHistId(players_info, current_first_id, num_of_players); //first
            players_info[hist_index][PLAYER_LOSSES_COL] ++;

            break;

        case DRAW:

            hist_index = getHistId(players_info, current_first_id, num_of_players); //first
            players_info[hist_index][PLAYER_DRAWS_COL] ++;

            hist_index = getHistId(players_info, current_second_id, num_of_players); //second
            players_info[hist_index][PLAYER_DRAWS_COL] ++;
            break;

        default:
            break;
        }
    }
}

static void initializeHist(int* hist, Tournament tournament, int hist_len)
{
    int i = 0;
    MAP_FOREACH(int*, games_iterator, tournamentGetGames(tournament))
    {
        /*int current_game_id = *games_iterator; try to test with this int and without.... */
        Game current_game = mapGet(tournamentGetGames(tournament), games_iterator);
        int current_first = gameGetFirstPlayer(current_game);
        int current_second = gameGetSecondPlayer(current_game);

        if(hist[i] == PLAYER_INVALID_ID)
        {
            if(!existInHist(hist, hist_len, current_first))
            {
                hist[i] = current_first;
                i++;
            }

            if (!existInHist(hist, hist_len, current_second))
            {
                hist[i] = current_second;
                i++;
            }
        }  
    }
}

static bool existInHist(int* hist, int hist_len, int value)
{
    for (int i = 0; i < hist_len; i++)
    {
        if(hist[i] == value)
        {
            return true;
        }
    }

    return false;
}

static int getHistId(int** players_info, int player_id, int num_of_players)
{
    for (int i = 0; i < num_of_players; i++)
    {
        if(players_info[i][PLAYER_ID_COL] == player_id)
        {
            return i;
        }
    }
    return PLAYER_INVALID_ID;
}

/* set the opponent of the player(with 'player_id' id) the winner of the game.
   if the player didn't play in the game, nothing will change. */
static void setOpponentTheWinner(Game game, int player_id)
{
    if(game == NULL || player_id <= 0)
    {
        return;
    }

    if(gameGetFirstPlayer(game) == player_id)
    {
        gameSetWinner(game, SECOND_PLAYER);
    }

    if(gameGetSecondPlayer(game) == player_id)
    {
        gameSetWinner(game, FIRST_PLAYER);
    }
}

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
    
    //free is done in tournamentDestroy, shouldnt free twice 
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