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
      
    return gameCreate(first, second, winner, game);
}

MapKeyElement copyGameIdFunc(MapKeyElement game_id)
{
    int* id = malloc(sizeof(int));
    *id = *( (int*)game_id );
   return id;
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