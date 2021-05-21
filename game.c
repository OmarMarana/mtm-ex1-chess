#include "game.h"
#include <stdlib.h>


struct Game_t
{
    int first_player;
    int second_player;

    Winner game_winner;

    int play_time;
};



/* ************************ */
/* MAP Functions definition */
/* ************************ */


MapDataElement copyGameFunc(MapDataElement game)
{
    int first = gameGetFirstPlayer((Game)game);
    int second = gameGetSecondPlayer((Game)game);
    Winner winner = gameGetWinner((Game)game);
    int play_time = gameGetPlayTime((Game)game);
    
    Game game_copy = gameCreate(first, second, winner, play_time);
    if(game_copy = NULL)
    {
        return NULL;

    }

    return game_copy;
}

MapKeyElement copyGameIdFunc(MapKeyElement game_id)
{
    int* new_id = malloc(sizeof(int));
    if(new_id == NULL)
    {
        return NULL;
    }
    *new_id = *( (int*)game_id );
   return new_id;
}

void freeGameFunc(MapDataElement game)
{
    gameDestroy(game);
}

void freeGameIdFunc(MapKeyElement game_id)
{
    free((int*)game_id);
}

int compareGameId(MapKeyElement game_id_1, MapKeyElement game_id_2)
{
   return( (*((int*)game_id_1))  -  (*((int*)game_id_2)) );
}


/*SET*/

void gameSetFirstPlayer(Game game, int first_player)
{
    game->first_player = first_player;
}


void gameSetSecondPlayer(Game game, int second_player)
{
    game->second_player = second_player;
}

void gameSetWinner(Game game, Winner game_winner)
{
    game->game_winner = game_winner;
}

void gameSetPlayTime(Game game,int play_time)
{
    game->play_time = play_time;
}



/*GET*/


int gameGetFirstPlayer(Game game)
{
    return game->first_player;
}

int gameGetSecondPlayer(Game game)
{
    return game->second_player;
}

Winner gameGetWinner(Game game)
{
    return game->game_winner;
}


int gameGetPlayTime(Game game)
{
    return game->play_time;
}



/*gameCreate*/
Game gameCreate(int first_player, int second_player, Winner game_winner, int play_time)
{
    Game new_game = malloc(sizeof(*new_game));
    if(new_game == NULL)
    {
        return NULL;
    }

    new_game->first_player = first_player; 
    new_game->second_player = second_player;
    new_game->game_winner = game_winner;
    new_game->play_time = play_time;
    
    return new_game;
}



/*gameDestroy*/
void gameDestroy(Game game)
{
    free(game);
}
