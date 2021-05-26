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
#define PLAYER_WINS_FACTOR 6.0
#define PLAYER_LOSE_FACTOR 10.0
#define PLAYER_DRAW_FACTOR 2.0
#define PLAY_MAT_NUM_COLS 2



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

// static void selfTest_addTournament(ChessSystem my_chess, int id, int maxGames, const char* loc);
// static void selfTest_addGame(ChessSystem chess, int tour_id, int first, int second, Winner winner, int time);
// static void selfTest_PrintPlayerIds(ChessSystem chess);
// static void selfTest_endTour(ChessSystem chess, int tour_id);



/* ******************************************* */
/* copy/free/compare functions for the Map ADT */
/* ******************************************* */

static int compareInts(MapKeyElement element1, MapKeyElement element2);
static void freePlayer(MapDataElement element);
static void freeTournament(MapDataElement element);
static void freeInt(MapKeyElement element);
static MapDataElement copyDataPlayer(MapDataElement element);
static MapDataElement copyDataTournament(MapDataElement element);
static MapKeyElement copyKeyInt(MapKeyElement element);

/*GAME Map functions*/
static MapDataElement copyGameFunc(MapDataElement game);
static void freeGameFunc(MapDataElement game);



/* **************** */
/* helper functions */
/* **************** */

static bool checkLocationValidation(const char* tournament_location);
static bool checkLetterIfNonCapitalOrSpace(char letter);
static bool checkLetterIfCapital(char letter);
static void setOpponentTheWinner(Game game, int player_id);
static int sortMatrixByCol(int** mat, int* len, int col, bool increasing);
static void freePlayersInfo(int** players_info, int len);
static void freePlayersInfoDouble(double** players_info, int len);
static int calcTournamentWinnerId(ChessSystem chess, Tournament tournament);
static void initializePlayerIdCol(int** players_info, int* hist_of_players_id, int num_of_players);
static void initalizePlayerInfo(int** players_info, int* hist_of_players_id, int num_of_players, int num_cols);
static bool allocatePlayersInfo(int** players_info, int* hist_of_players_id, int num_of_players, int num_cols);
static void swapRow(int* row_a, int* row_b, int row_len);
static void swapDoubleRow(double* row_a, double* row_b, int row_len);
static void sumWinLoseDraw(int** players_info, int num_of_players, Tournament tournament);
static int initializeHist(int* hist, Tournament tournament, int hist_len);
static bool existInHist(int* hist, int hist_len, int value);
static int getHistId(int** players_info, int player_id, int num_of_players);
static int tournamentGetLongestGameTime(Tournament tournament);
static double tournamentGetAverageGameTime(Tournament tournament);
static bool noTournamentsEnded(ChessSystem chess);
static double calcPlayerLevel(ChessSystem chess, int player_id);
static void outOfMemoryError(ChessSystem chess);
static bool allocatePlayersInfoDouble(double** players_info, int num_of_players, int num_cols);
static void setPlayersInfoInMat(ChessSystem chess, double** players_info, int num_of_players);
static void initalizePlayerInfoDouble(double** players_info, int num_of_players, int num_cols);
static void sortDoubleMatrixByCol(double** mat, int start, int end, int col, bool increasing);
static bool gameAlreadyExists(ChessSystem chess, Tournament tournament, int first_player, int second_player);
static int playerCountGamesInTournament(Tournament tournament, int player_id);
static void addPlayersToPlayersList(ChessSystem chess, int first_player, int second_player);
static void updatePlayerStats(ChessSystem chess, Game game);
static void updateTournamentStats(ChessSystem chess , Tournament tournament);
static void updatePlayersAfterTourRemove(ChessSystem chess, Tournament tournament);
static void updatePlayerStatsAfterTourRemove(Player player, int play_time_remove,
                                              Winner game_winner, Winner player_role);


/* ************************* */
/* ChessSystem ADT functions */
/* ************************* */

ChessSystem chessCreate()
{
    ChessSystem chess = malloc(sizeof(*chess));
    if(chess == NULL)
    {
        return NULL;
    }

    Map tournaments = mapCreate(copyDataTournament, copyKeyInt, freeTournament, freeInt, compareInts);
    if(tournaments == NULL)
    {
        outOfMemoryError(chess);
        return NULL;
    }

    chess->tournaments = tournaments;

    Map players = mapCreate(copyDataPlayer, copyKeyInt, freePlayer, freeInt, compareInts);
    if(players == NULL)
    {
        outOfMemoryError(chess);
        return NULL;
    }

    chess->players = players;


    return chess;
}


ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if(file == NULL)
    {
        return CHESS_SAVE_FAILURE;
    }


    int num_of_players = mapGetSize(chess->players);

    double** players_info = (double**) malloc(sizeof(double*) * num_of_players);
    if(players_info == NULL)
    {
        outOfMemoryError(chess); 
    }

    bool allocate_result = allocatePlayersInfoDouble(players_info, num_of_players, PLAY_MAT_NUM_COLS);
    if(!allocate_result)
    {
        freePlayersInfoDouble(players_info, num_of_players);
        outOfMemoryError(chess);
    }


    initalizePlayerInfoDouble(players_info, num_of_players, PLAY_MAT_NUM_COLS);
    
    setPlayersInfoInMat(chess, players_info, num_of_players);

    sortDoubleMatrixByCol(players_info, 0, num_of_players - 1, PLAYER_POINTS_COL, false);

    int start = 0, end = 0;

    while(start < num_of_players)
    {
        if(end == num_of_players)
        {
            sortDoubleMatrixByCol(players_info, start, end-1, PLAYER_ID_COL, true);
            break;
        }

        if(players_info[start][PLAYER_POINTS_COL] == players_info[end][PLAYER_POINTS_COL])
        {
                end++;
        }
        else
        {
            sortDoubleMatrixByCol(players_info, start, end-1, PLAYER_ID_COL, true);
            start = end;
        }
    }

    for (int i = 0; i < num_of_players; i++)
    {
        fprintf(file, "%d %.2f\n", (int) players_info[i][PLAYER_ID_COL], players_info[i][PLAYER_POINTS_COL]);
    }

    freePlayersInfoDouble(players_info, num_of_players);

    return CHESS_SUCCESS;
    
}

static void initalizePlayerInfoDouble(double** players_info, int num_of_players, int num_cols)
{
    for (int p = 0; p < num_of_players; p++)
    {
        for (int i = 0; i < num_cols; i++)
        {
            players_info[p][i] = 0.0;
        }
    }
}



static bool allocatePlayersInfoDouble(double** players_info, int num_of_players, int num_cols)
{
    for (int i = 0; i < num_of_players; i++)
    {
        players_info[i] = malloc(sizeof(double) * num_cols);
        if(players_info[i] == NULL)
        {
            for (int t = 0; t < i; t++)
            {
                free(players_info[t]);
            }
            free(players_info);
            return false;
        }
    }
    return true;
}

static void setPlayersInfoInMat(ChessSystem chess, double** players_info, int num_of_players)
{
    int i = 0;

    MAP_FOREACH(int*, player_iterator, chess->players)
    {
        Player current_player = mapGet(chess->players, player_iterator);
       
        int current_id = playerGetId(current_player);
        double current_level = calcPlayerLevel(chess, current_id);

        if(i < num_of_players)
        {
            players_info[i][PLAYER_ID_COL] = current_id;
            players_info[i][PLAYER_POINTS_COL] = current_level;
        }
        i++;
        free(player_iterator);
    }
}

static double calcPlayerLevel(ChessSystem chess, int player_id)
{
    Player player = mapGet(chess->players, &player_id);

    int wins = playerGetGameStatics(player, PLAYER_WINS);
    int losses = playerGetGameStatics(player, PLAYER_LOSSES);
    int draws = playerGetGameStatics(player, PLAYER_DRAWS);

    int total_games = wins + losses + draws;

    /* if a player doesnt have any games (tournament removed...), return level 0 ? (undefined in pdf) */
    if(total_games == 0)
    {
        return 0;
    }

    double player_level =((PLAYER_WINS_FACTOR * wins) - (PLAYER_LOSE_FACTOR * losses) + (PLAYER_DRAW_FACTOR * draws))
                         / (double) total_games;

    return player_level;
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    
    if(noTournamentsEnded(chess))
    {
        return CHESS_NO_TOURNAMENTS_ENDED;
    }

    FILE* stream = fopen(path_file, "w");

    if(stream == NULL)
    {
        return CHESS_SAVE_FAILURE;
    }

    MAP_FOREACH(int*, tournament_iterator, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, tournament_iterator);
        if(tournamentGetFinishedState(tournament) == false)
        {
            continue;
        }
        
        /*since chess->tournaments is a map, we can assume that its sorted by ids (increasing)*/
        
        fprintf(stream, "%d\n",   tournamentGetWinnderId(tournament));
        fprintf(stream, "%d\n",   tournamentGetLongestGameTime(tournament));
        fprintf(stream, "%.2f\n", tournamentGetAverageGameTime(tournament));
        fprintf(stream, "%s\n",   tournamentGetLocation(tournament));
        fprintf(stream, "%d\n",   tournamentGetGamesPlayed(tournament));
        fprintf(stream, "%d\n",   tournamentGetNumDiffPlayers(tournament));

        free(tournament_iterator);
    }

    fclose(stream);
    return CHESS_SUCCESS;
}

static int tournamentGetLongestGameTime(Tournament tournament)
{
    int max = 0;
    MAP_FOREACH(int*, game_id_iterator, tournamentGetGames(tournament))
    {
        Game current_game = mapGet(tournamentGetGames(tournament), game_id_iterator);
        int current_game_time = gameGetPlayTime(current_game);
        if(current_game_time > max)
        {
            max = current_game_time;
        }
        free(game_id_iterator);
    }
    
    return max;
}

static double tournamentGetAverageGameTime(Tournament tournament)
{
    double game_avg = 0.0;
    int total_games = tournamentGetGamesPlayed(tournament);
    
    MAP_FOREACH(int*, game_id_iterator, tournamentGetGames(tournament))
    {
        Game current_game = mapGet(tournamentGetGames(tournament), game_id_iterator);
        int current_game_time = gameGetPlayTime(current_game);
        game_avg += ((double) current_game_time / total_games);
        free(game_id_iterator);
    }

    return game_avg;
}

static bool noTournamentsEnded(ChessSystem chess)
{
    MAP_FOREACH(int*, tournament_iterator, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, tournament_iterator);
        if(tournamentGetFinishedState(tournament) == true)
        {
            free(tournament_iterator);
            return false;
        }
        free(tournament_iterator);
    }
    return true;
}

double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
    if(chess == NULL)
    {
        *chess_result = CHESS_NULL_ARGUMENT;
        return CHESS_NULL_ARGUMENT;
    }

    if(player_id <= 0)
    {
        *chess_result = CHESS_INVALID_ID;
        return CHESS_INVALID_ID;
    }

    if(!mapContains(chess->players, &player_id))
    {
        *chess_result = CHESS_PLAYER_NOT_EXIST;
        return CHESS_PLAYER_NOT_EXIST;
    }

    Player player = mapGet(chess->players, &player_id);
    return playerGetAvgPlayTime(player);
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    
    if(tournament_id <=0  || first_player <=0 || second_player <=0 || first_player == second_player || 
       (winner != FIRST_PLAYER && winner != SECOND_PLAYER && winner != DRAW))
    {
        return CHESS_INVALID_ID;
    }

    if(!mapContains(chess->tournaments, &tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    
    Tournament tournament = mapGet(chess->tournaments, &tournament_id);
    if(tournamentGetFinishedState(tournament) == true)
    {
        return CHESS_TOURNAMENT_ENDED;
    }

    if(gameAlreadyExists(chess, tournament, first_player, second_player) == true)
    {
        return CHESS_GAME_ALREADY_EXISTS;
    }

    if(play_time < 0)
    {
        return CHESS_INVALID_PLAY_TIME;
    }

    if(playerCountGamesInTournament(tournament,first_player) > tournamentGetMaxGames(tournament) || 
    playerCountGamesInTournament(tournament,second_player) > tournamentGetMaxGames(tournament))
    {
        return CHESS_EXCEEDED_GAMES;
    }
    
    int new_game_id = tournamentGetGamesPlayed(tournament) + 1;
    Game new_game = gameCreate(first_player, second_player, winner, play_time, new_game_id);
    if(new_game == NULL)
    {
        outOfMemoryError(chess);
    }

    MapResult put_result = mapPut(tournamentGetGames(tournament), &new_game_id , new_game);
    if(put_result != MAP_SUCCESS)
    {
        outOfMemoryError(chess);
    }
    



    addPlayersToPlayersList(chess, first_player, second_player);
    updatePlayerStats(chess, new_game);

    gameDestroy(new_game);
    updateTournamentStats(chess, tournament);
    return CHESS_SUCCESS;


}


static void addPlayersToPlayersList(ChessSystem chess, int first_player, int second_player)
{
    if(!mapContains(chess->players, &first_player))
    {
        Player player1 = playerCreate(first_player);
        if(player1 == NULL)
        {
            outOfMemoryError(chess);
        }
        mapPut(chess->players, &first_player, player1);
        playerDestroy(player1);
    }

    if(!mapContains(chess->players, &second_player))
    {
        Player player2 = playerCreate(second_player);
        if(player2 == NULL)
        {
            outOfMemoryError(chess);
        }
        mapPut(chess->players, &second_player, player2);
        playerDestroy(player2);
    }
}

/*update player stats in the players map*/
static void updatePlayerStats(ChessSystem chess, Game game)
{
    Map players_list = chess->players;
    int first_player_id = gameGetFirstPlayer(game);
    int second_player_id = gameGetSecondPlayer(game);
    Player player1 = mapGet(players_list, &first_player_id);
    Player player2 = mapGet(players_list, &second_player_id);

    int player1_wins = playerGetGameStatics(player1, PLAYER_WINS);
    int player1_losses = playerGetGameStatics(player1, PLAYER_LOSSES);
    int player1_draws = playerGetGameStatics(player1, PLAYER_DRAWS);
    
    int player2_wins = playerGetGameStatics(player2, PLAYER_WINS);
    int player2_losses = playerGetGameStatics(player2, PLAYER_LOSSES);
    int player2_draws = playerGetGameStatics(player2, PLAYER_DRAWS);


    switch (gameGetWinner(game))
        {
        case FIRST_PLAYER:
            playerSetGameStatics(player1, PLAYER_WINS, player1_wins + 1);
            playerSetGameStatics(player2, PLAYER_LOSSES, player2_losses + 1 );
            break;
        
        case SECOND_PLAYER:
            playerSetGameStatics(player2, PLAYER_WINS, player2_wins + 1 );
            playerSetGameStatics(player1, PLAYER_LOSSES, player1_losses + 1 );
            break;

        case DRAW:
            playerSetGameStatics(player1, PLAYER_DRAWS, player1_draws + 1);
            playerSetGameStatics(player2, PLAYER_DRAWS, player2_draws + 1);

        default:
            break;
        }

    int player1_total_games = player1_wins + player1_draws + player1_losses + 1;
    int player2_total_games = player2_wins + player2_draws + player2_losses + 1;

    double player1_avg_play_time = playerGetAvgPlayTime(player1);
    double player2_avg_play_time = playerGetAvgPlayTime(player2);

    int game_play_time = gameGetPlayTime(game); 

    double new_player1_avg_play_time = ((player1_avg_play_time * (player1_total_games -1)) + game_play_time)
                                         / player1_total_games;

    playerSetAvgPlayTime(player1, new_player1_avg_play_time);

    double new_player2_avg_play_time = ((player2_avg_play_time * (player2_total_games -1)) + game_play_time)
                                        / player2_total_games;

    playerSetAvgPlayTime(player2, new_player2_avg_play_time);
}

/* count how many different players played in the tournament and update 'num_different_players'*/
static void updateTournamentStats(ChessSystem chess , Tournament tournament)
{
    if(tournament == NULL)
    {
        return;
    }

    int max_length = mapGetSize(chess->players) * 2;
    int* hist_players = malloc(sizeof(*hist_players) * max_length);
    if(hist_players == NULL)
    {
        outOfMemoryError(chess);
    }

    for (int i = 0; i < max_length; i++)
    {
        hist_players[i] = PLAYER_INVALID_ID;
    }

    int num_of_players = initializeHist(hist_players, tournament, max_length);

    tournamentSetNumDiffPlayers(tournament, num_of_players);

    free(hist_players);
}

static int playerCountGamesInTournament(Tournament tournament, int player_id)
{
    int count = 0;

    MAP_FOREACH(int*, game_id_iterator, tournamentGetGames(tournament))
    {
        Game current_game = mapGet(tournamentGetGames(tournament), game_id_iterator);
        int current_first = gameGetFirstPlayer(current_game);
        int current_second = gameGetSecondPlayer(current_game);

        if(current_first  == player_id || current_second == player_id)
        {
            count++;
        }
        free(game_id_iterator);
    }


    return count;
}

static bool gameAlreadyExists(ChessSystem chess, Tournament tournament, int first_player, int second_player)
{
    
    if(!mapContains(chess->players, &first_player) || !mapContains(chess->players, &second_player))
    {
        return false;
    }
    
    MAP_FOREACH(int*, game_id_iterator, tournamentGetGames(tournament))
    {
        Game current_game = mapGet(tournamentGetGames(tournament), game_id_iterator);
        int current_first = gameGetFirstPlayer(current_game);
        int current_second = gameGetSecondPlayer(current_game);

        if(( first_player  == current_first || first_player == current_second) &&
            (second_player == current_first || second_player == current_second ))
        {
            free(game_id_iterator);
            return true;
        }
        free(game_id_iterator);
    }
    return false;
}

static void outOfMemoryError(ChessSystem chess)
{
    printf("OUT OF MEMORY!\n");
    chessDestroy(chess);
    exit(1);
}

void chessDestroy(ChessSystem chess)
{

    if(chess == NULL)
    {
        return;
    }

    mapDestroy(chess->players);

    // MAP_FOREACH(int*, tour_iterator, chess->tournaments)
    // {
    //     Tournament current_tour = mapGet(chess->tournaments, tour_iterator);

    //     mapDestroy(tournamentGetGames(current_tour));

    //     free(tour_iterator);
    // }

    mapDestroy(chess->tournaments);
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
    Map games = mapCreate(copyGameFunc, copyKeyInt, freeGameFunc, freeInt, compareInts);

    if(new_tournament == NULL || games == NULL)
    {
        printf("TT\n");
        outOfMemoryError(chess);
    }

    tournamentSetGameList(new_tournament, games);

    /*changed here*/

    // Game game = gameCreate(100,200,FIRST_PLAYER,500,1);
    // int gameid =1;
    // mapPut(games,&gameid, game);
    
    // gameDestroy(game);

    //now there should be memory leak in map put becuase the freeTour dont work properly

    /*changed here*/



    MapResult putResult = mapPut(chess->tournaments, &tournament_id, new_tournament);

    if(putResult != MAP_SUCCESS)
    {
        printf("putResult = %d\n", putResult);
        outOfMemoryError(chess);
    }

    tournamentDestroy(new_tournament);
    /*changed here*/
    // mapDestroy(games);

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


    updatePlayersAfterTourRemove(chess, tournament_to_remove);
    //remove a player from the players list if he doesn't have any games in other tournaments ???*/


    /* Remove all of the games in the tournament */
    //changed here
    //mapDestroy(tournamentGetGames(tournament_to_remove));
    
    /* Remove the tournament from the list and deallocate the memory. */
    mapRemove(chess->tournaments, &tournament_id);

    return CHESS_SUCCESS;
}

static void updatePlayersAfterTourRemove(ChessSystem chess, Tournament tournament)
{
    Map games = tournamentGetGames(tournament);
    Map players = chess->players;

    MAP_FOREACH(int*, game_iterator, games)
    {
        Game current_game = mapGet(games, game_iterator);
        int first_id = gameGetFirstPlayer(current_game);
        int second_id = gameGetSecondPlayer(current_game);
        Winner current_winner = gameGetWinner(current_game);
        int game_play_time = gameGetPlayTime(current_game);

        Player first = mapGet(players, &first_id);
        Player second = mapGet(players, &second_id);

        updatePlayerStatsAfterTourRemove(first, game_play_time, current_winner, FIRST_PLAYER);
        updatePlayerStatsAfterTourRemove(second, game_play_time, current_winner, SECOND_PLAYER);

        free(game_iterator);
    }
}

static void updatePlayerStatsAfterTourRemove(Player player, int play_time_remove,
                                              Winner game_winner, Winner player_role)
{
    int total_games = playerGetTotalGamesPlayed(player);
    double new_play_time = playerGetAvgPlayTime(player);
    if(total_games <= 1)
    {
        new_play_time = 0.0;
    }
    else
    {
        new_play_time *= total_games;
        new_play_time -= play_time_remove;
        new_play_time /= (total_games-1);
    }

    playerSetAvgPlayTime(player, new_play_time);

    int wins = playerGetGameStatics(player, PLAYER_WINS);
    int losses = playerGetGameStatics(player, PLAYER_LOSSES);
    int draws = playerGetGameStatics(player, PLAYER_DRAWS);


    if(player_role == game_winner)
    {
        playerSetGameStatics(player, PLAYER_WINS, wins - 1);
    }
    else if(game_winner != DRAW)
    {
        playerSetGameStatics(player, PLAYER_LOSSES, losses - 1);
    }
    else
    {
        playerSetGameStatics(player, PLAYER_DRAWS, draws - 1);
    }
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


    MAP_FOREACH(int*, tournament_iterator, chess->tournaments)
    {
        Tournament current_tournament = mapGet(chess->tournaments, tournament_iterator);
        if(tournamentGetFinishedState(current_tournament) == true)
        {
            free(tournament_iterator);
            continue;            
        }
        
        MAP_FOREACH(int*, game_iterator, tournamentGetGames(current_tournament))
        {
            Game current_game = mapGet(tournamentGetGames(current_tournament), game_iterator);

            setOpponentTheWinner(current_game, player_id);
            free(game_iterator);
        }
        free(tournament_iterator);
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
        printf("NO GAMES \n");
        return CHESS_NO_GAMES;
    }

    int winner_id = calcTournamentWinnerId(chess, current_tournament);
    

    tournamentSetWinnderId(current_tournament, winner_id);
    tournamentSetFinishedState(current_tournament, true);

    return CHESS_SUCCESS;
}

/* **************************** */
/* helper functions definitions */
/* **************************** */

/* return the id of the tournament winner. */
static int calcTournamentWinnerId(ChessSystem chess, Tournament tournament)
{

    int num_of_players = tournamentGetNumDiffPlayers(tournament);

    int** players_info = (int**) malloc(sizeof(int*) * num_of_players);
    if(players_info == NULL)
    {
        outOfMemoryError(chess);    
    }

    int* hist_of_players_id = malloc(sizeof(*hist_of_players_id) * num_of_players);
    if(hist_of_players_id == NULL)
    {
        free(players_info);
        outOfMemoryError(chess);
    }

    bool allocate_result = allocatePlayersInfo(players_info, hist_of_players_id, num_of_players, PLAYER_STATS_COLS);
    if(!allocate_result)
    {
        free(hist_of_players_id);
        free(players_info);
        outOfMemoryError(chess);
    }



    initalizePlayerInfo(players_info, hist_of_players_id, num_of_players, PLAYER_STATS_COLS);


    for(int i = 0; i < num_of_players; i++)
    {
        hist_of_players_id[i] = PLAYER_INVALID_ID;
    }

    initializeHist(hist_of_players_id, tournament, num_of_players);

    initializePlayerIdCol(players_info, hist_of_players_id, num_of_players);

    sumWinLoseDraw(players_info, num_of_players, tournament);

    for (int i = 0; i < num_of_players; i++)
    {
        players_info[i][PLAYER_POINTS_COL] = WIN_POINTS * players_info[i][PLAYER_WINS_COL] + 
                                             DRAW_POINTS * players_info[i][PLAYER_DRAWS_COL];
    }

    int winner_id = 0;

    free(hist_of_players_id);

    if(sortMatrixByCol(players_info, &num_of_players, PLAYER_POINTS_COL, false) == 1)
    {
        winner_id = players_info[0][PLAYER_ID_COL];
        freePlayersInfo(players_info, num_of_players);
        return winner_id;
    }

    if(sortMatrixByCol(players_info, &num_of_players, PLAYER_LOSSES_COL, true) == 1)
    {
        winner_id = players_info[0][PLAYER_ID_COL];
        freePlayersInfo(players_info, num_of_players);
        return winner_id;
    }

    if(sortMatrixByCol(players_info, &num_of_players, PLAYER_WINS_COL, false) == 1)
    {
        winner_id = players_info[0][PLAYER_ID_COL];
        freePlayersInfo(players_info, num_of_players);
        return winner_id;
    }


    sortMatrixByCol(players_info, &num_of_players, PLAYER_ID_COL, true);

    winner_id = players_info[0][PLAYER_ID_COL];

    freePlayersInfo(players_info, num_of_players);
    
    return winner_id;
}

static void freePlayersInfo(int** players_info, int len)
{
    for (int i = 0; i < len; i++)
    {
        free(players_info[i]);
    }
    free(players_info);
}

static void freePlayersInfoDouble(double** players_info, int len)
{
    for (int i = 0; i < len; i++)
    {
        free(players_info[i]);
    }
    free(players_info);
}

static void initializePlayerIdCol(int** players_info, int* hist_of_players_id, int num_of_players)
{
    for (int i = 0; i < num_of_players; i++)
    {
        players_info[i][PLAYER_ID_COL] = hist_of_players_id[i];
    }
}

static void initalizePlayerInfo(int** players_info, int* hist_of_players_id, int num_of_players, int num_cols)
{
    for (int p = 0; p < num_of_players; p++)
    {
        if(hist_of_players_id != NULL)
        {
            hist_of_players_id[p] = PLAYER_INVALID_ID;
        }

        for (int i = 0; i < num_cols; i++)
        {
            players_info[p][i] = 0;
        }
    }
}

/* return true if success, false if fail to allocate the memory. */
static bool allocatePlayersInfo(int** players_info, int* hist_of_players_id, int num_of_players, int num_cols)
{
    for (int i = 0; i < num_of_players; i++)
    {
        players_info[i] = malloc(sizeof(int) * num_cols);
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

/* return's the new len of the mat.*/
static int sortMatrixByCol(int** mat, int* len, int col, bool increasing)
{
    int length = *len;
    for (int i = 0; i < length; i++)
    {
        for (int t = 1; t < length - 1; t++)
        {
            if(increasing)
            {
                if(mat[t][col] >= mat[t+1][col])
                {
                    swapRow(mat[t], mat[t+1], PLAYER_STATS_COLS);
                }
            }
            else
            {
                if(mat[t][col] <= mat[t+1][col])
                {
                    swapRow(mat[t], mat[t+1], PLAYER_STATS_COLS);
                }
            }
        }
    }

    for (int i = length-1; i > 0; i--)
    {
        if(mat[i][col] != mat[0][col])
        {
            free(mat[i]);
            length--;
        }
    }
    *len = length;

    return length;
}

static void sortDoubleMatrixByCol(double** mat, int start, int end, int col, bool increasing)
{
    for (int i = start; i <= end; i++)
    {
        for (int t = start; t <= end - 1; t++)
        {
            if(increasing)
            {
                if(mat[t][col] >= mat[t+1][col])
                {
                    swapDoubleRow(mat[t], mat[t+1], PLAY_MAT_NUM_COLS);
                }
            }
            else
            {
                if(mat[t][col] <= mat[t+1][col])
                {
                    swapDoubleRow(mat[t], mat[t+1], PLAY_MAT_NUM_COLS);
                }
            }
        }
    }
}

static void swapDoubleRow(double* row_a, double* row_b, int row_len)
{
    double tmp[row_len];
    for (int i = 0; i < row_len; i++)
    {
        tmp[i] = row_a[i];
        row_a[i] = row_b[i];
        row_b[i] = tmp[i];
    }
}

static void swapRow(int* row_a, int* row_b, int row_len)
{
    int tmp[row_len];
    for (int i = 0; i < row_len; i++)
    {
        tmp[i] = row_a[i];
        row_a[i] = row_b[i];
        row_b[i] = tmp[i];
    }
}

static void sumWinLoseDraw(int** players_info, int num_of_players, Tournament tournament)
{
    MAP_FOREACH(int*, games_iterator, tournamentGetGames(tournament))
    {
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
        free(games_iterator);
    }
}


static int initializeHist(int* hist, Tournament tournament, int hist_len)
{
    int i = 0;
    MAP_FOREACH(int*, games_iterator, tournamentGetGames(tournament))
    {
        Game current_game = mapGet(tournamentGetGames(tournament), games_iterator);
        int current_first = gameGetFirstPlayer(current_game);
        int current_second = gameGetSecondPlayer(current_game);

        if(i < hist_len && hist[i] == PLAYER_INVALID_ID)
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
        free(games_iterator);
    }

    return i;
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

/** Function to be used for copying a tournament as a data to the map */
static MapDataElement copyDataTournament(MapDataElement element)
{
    if (element == NULL)
    {
        return NULL;
    }

    int cpy_id = tournamentGetId((Tournament) element);
    
    
    //maybe should use strcpy and make a new copy of the str location

    const char* cpy_location = tournamentGetLocation((Tournament) element);

    int cpy_max_games = tournamentGetMaxGames((Tournament) element);
    bool cpy_finished = tournamentGetFinishedState((Tournament) element);
    int cpy_tour_winner_id = tournamentGetWinnderId((Tournament) element);
    int cpy_different_players = tournamentGetNumDiffPlayers((Tournament) element);
    Map cpy_game_list = mapCopy(tournamentGetGames((Tournament) element));

    /*//mapDestroy(tournamentGetGames((Tournament) element));*/
    

    Tournament copy_tournament = tournamentCreate(cpy_id, cpy_location, cpy_max_games);
    if (copy_tournament == NULL || cpy_game_list == NULL)
    {
        return NULL;
    }

    tournamentSetFinishedState(copy_tournament, cpy_finished);
    tournamentSetNumDiffPlayers(copy_tournament, cpy_different_players);
    tournamentSetWinnderId(copy_tournament, cpy_tour_winner_id);
    tournamentSetGameList(copy_tournament, cpy_game_list);


    return copy_tournament;
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
    int wins_copy = playerGetGameStatics(element, PLAYER_WINS);
    int losses_copy = playerGetGameStatics(element, PLAYER_LOSSES);
    int draws_copy = playerGetGameStatics(element, PLAYER_DRAWS);

    if(cpy_id <= 0)
    {
        return NULL;
    }

    Player copy = playerCreate(cpy_id);
    if (copy == NULL)
    {
        return NULL;
    }
    
    playerSetGameStatics(element, PLAYER_WINS, wins_copy);
    playerSetGameStatics(element, PLAYER_LOSSES, losses_copy);
    playerSetGameStatics(element, PLAYER_DRAWS, draws_copy);

    playerSetLevel(copy, cpy_level);

    return copy;
}

/** Function to be used by the map for freeing elements */
static void freeInt(MapKeyElement element)
{
    free(element);
}

// /** Function to be used by the map for freeing elements */
static void freeTournament(MapDataElement element)
{
    tournamentDestroy(element);
}

/** Function to be used by the map for freeing elements */
static void freePlayer(MapDataElement element)
{
    playerDestroy(element);
}

/** Function to be used by the map for comparing elements */
static int compareInts(MapKeyElement element1, MapKeyElement element2)
{
    return (*(int *) element1 - *(int *) element2);
}

static MapDataElement copyGameFunc(MapDataElement game)
{
    int first = gameGetFirstPlayer((Game)game);
    int second = gameGetSecondPlayer((Game)game);
    Winner winner = gameGetWinner((Game)game);
    int play_time = gameGetPlayTime((Game)game);
    int game_id = gameGetId(game);

    Game game_copy = gameCreate(first, second, winner, play_time, game_id);
    if(game_copy == NULL)
    {
        return NULL;

    }
    
    return game_copy;
}

static void freeGameFunc(MapDataElement game)
{
    gameDestroy(game);
}






// int main()
// {
    
//     printf(ANSI_COLOR_BLUE "Test started...\n" ANSI_COLOR_RESET);

//     ChessSystem my_chess;
//     my_chess = chessCreate();

//     if(my_chess == NULL)
//     {
//         printf(ANSI_COLOR_RED "couldn't create chess system! - FAIL!\n" ANSI_COLOR_RESET);
//         return 0;
//     }
//     else
//     {
//         printf(ANSI_COLOR_GREEN "Chess System Created successfully.\n" ANSI_COLOR_RESET);
//     }



    
    
//     selfTest_addTournament(my_chess, 1, 4, "London");

//     selfTest_addTournament(my_chess, 2, 5, "London");

//     selfTest_addTournament(my_chess, 3, 10, "Paris");

    

//     selfTest_addGame(my_chess, 1, 87, 1, FIRST_PLAYER, 40);
//     selfTest_addGame(my_chess, 1, 87, 2, FIRST_PLAYER, 40);
//     selfTest_addGame(my_chess, 1, 87, 3, FIRST_PLAYER, 40);
//     selfTest_addGame(my_chess, 1, 87, 4, FIRST_PLAYER, 40);
//     selfTest_addGame(my_chess, 1, 87, 5, FIRST_PLAYER, 40);



//     chessDestroy(my_chess);
//     return 0;

// //     // printf(ANSI_COLOR_BLUE "ending tournament...\n" ANSI_COLOR_RESET);

// //     // selfTest_endTour(my_chess, tour_id);

// //    // printf(ANSI_COLOR_BLUE "Adding new game...\n" ANSI_COLOR_RESET);

// //     selfTest_addGame(my_chess, tour_id, 105, 290, FIRST_PLAYER, 576);


// //     selfTest_PrintPlayerIds(my_chess);


//     chessDestroy(my_chess);
//     return 0;
// }

// // static void selfTest_endTour(ChessSystem chess, int tour_id)
// // {
// //     ChessResult tour_end_rslt = chessEndTournament(chess, tour_id);

// //     switch(tour_end_rslt)
// //     {
// //     case CHESS_SUCCESS:
// //         printf(ANSI_COLOR_GREEN "tournament id = %d ended successfully", tour_id);
// //         printf("\n" ANSI_COLOR_RESET);
// //     break;
        
// //     case CHESS_NULL_ARGUMENT:
// //         printf(ANSI_COLOR_RED "Error! NULL argument.\n" ANSI_COLOR_RESET);
// //     break;

// //     case CHESS_INVALID_ID:
// //         printf(ANSI_COLOR_RED "Error! invalid id.\n" ANSI_COLOR_RESET);
// //     break;

// //     case CHESS_TOURNAMENT_NOT_EXIST:
// //         printf(ANSI_COLOR_RED "Error! tournament not exist.\n" ANSI_COLOR_RESET);
// //     break;

// //     case CHESS_TOURNAMENT_ENDED:
// //         printf( ANSI_COLOR_RED "Error! tournament already ended.\n" ANSI_COLOR_RESET);
// //     break;

// //     case CHESS_NO_GAMES:
// //         printf( ANSI_COLOR_RED "Error! no games.\n" ANSI_COLOR_RESET);
// //     break;

// //     default:
// //         printf(ANSI_COLOR_RED "Error! default error code!!!.\n" ANSI_COLOR_RESET);
// //         break;
// //     }
// // }

// static void selfTest_addGame(ChessSystem chess, int tour_id, int first, int second, Winner winner, int time)
// {
//     printf(ANSI_COLOR_BLUE "Adding new games...\n" ANSI_COLOR_RESET);
//     ChessResult game_add_rslt = chessAddGame(chess, tour_id, first, second, winner, time);

//     switch(game_add_rslt)
//     {
//     case CHESS_SUCCESS:
//         printf(ANSI_COLOR_GREEN "Game Added successfully into tournament id = %d", tour_id);
//         printf("\n" ANSI_COLOR_RESET);
//     break;
        
//     case CHESS_NULL_ARGUMENT:
//         printf(ANSI_COLOR_RED "Error! NULL argument.\n" ANSI_COLOR_RESET);
//     break;

//     case CHESS_INVALID_ID:
//         printf(ANSI_COLOR_RED "Error! invalid id.\n" ANSI_COLOR_RESET);
//     break;

//     case CHESS_TOURNAMENT_NOT_EXIST:
//         printf(ANSI_COLOR_RED "Error! tournament not exist.\n" ANSI_COLOR_RESET);
//     break;

//     case CHESS_TOURNAMENT_ENDED:
//         printf( ANSI_COLOR_RED "Error! tournament already ended.\n" ANSI_COLOR_RESET);
//     break;

//     case CHESS_GAME_ALREADY_EXISTS:
//         printf(ANSI_COLOR_RED "Error! GAME already exists.\n" ANSI_COLOR_RESET);
//     break;

//     case CHESS_INVALID_PLAY_TIME:
//         printf(ANSI_COLOR_RED "Error! invalid play time.\n" ANSI_COLOR_RESET);
//     break;

//     case CHESS_EXCEEDED_GAMES:
//         printf(ANSI_COLOR_RED "Error! player exceeded games.\n" ANSI_COLOR_RESET);
//     break;

//     default:
//         printf(ANSI_COLOR_RED "Error! default error code!!!.\n" ANSI_COLOR_RESET);
//         break;
//     }
// }


// // static void selfTest_PrintPlayerIds(ChessSystem chess)
// // {
// //     int i =0;
// //     MAP_FOREACH(int*, player_iterator, chess->players)
// //     {
// //         Player current_player = mapGet(chess->players, player_iterator);
       
// //         int current_id = playerGetId(current_player);
        
// //         printf("player id num %d is %d\n", i, current_id);
// //         i++;
// //     }
// // }

// static void selfTest_addTournament(ChessSystem my_chess, int id, int maxGames, const char* loc)
// {
//     ChessResult tour_add_rslt = chessAddTournament(my_chess, id, maxGames, loc);
//     Tournament tourny = mapGet(my_chess->tournaments, &id);
    
//     printf("\ntour %d location is %s\n", id,tournamentGetLocation(tourny));

//     switch (tour_add_rslt)
//     {
//     case CHESS_SUCCESS:
//             printf(ANSI_COLOR_GREEN "Tournament Added successfully. id = %d", id);
//             printf("\n" ANSI_COLOR_RESET);
//         break;
//     case CHESS_NULL_ARGUMENT:
//         printf(ANSI_COLOR_RED "Error! NULL argument.\n" ANSI_COLOR_RESET);
//     break;

//         case CHESS_INVALID_ID:
//         printf(ANSI_COLOR_RED "Error! invalid id.\n" ANSI_COLOR_RESET);
//     break;

//         case CHESS_INVALID_LOCATION:
//         printf(ANSI_COLOR_RED "Error! invalid location.\n" ANSI_COLOR_RESET);
//     break;

//     case CHESS_INVALID_MAX_GAMES:
//         printf( ANSI_COLOR_RED "Error! invalid max games per player.\n" ANSI_COLOR_RESET);
//     break;

//     case CHESS_TOURNAMENT_ALREADY_EXISTS:
//         printf(ANSI_COLOR_RED "Error! tournament already exists.\n" ANSI_COLOR_RESET);
//     break;

//     default:
//         printf(ANSI_COLOR_RED "Error! default error code!!!.\n" ANSI_COLOR_RESET);
//         break;
//     }
// }
