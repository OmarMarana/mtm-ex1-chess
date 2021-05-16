#ifndef GAME_H_
#define GAME_H_

#ifndef _CHESSSYSTEM_H
#include "chessSystem.h"
#endif

#ifndef MAP_H_
#include "map.h"
#endif



typedef struct Game_t *Game;


typedef enum GameResult_t {
    GAME_SUCCESS = 1,
    GAME_OUT_OF_MEMORY,
    GAME_NULL_ARGUMENT
} GameResult;



Game gameCreate(int first_player, int second_player, Winner game_winner, int play_time);
void gameDestroy(Game game);

/*Get*/
int gameGetFirstPlayer(Game game);
int gameGetSecondPlayer(Game game);
Winner gameGetWinner(Game game);
int gameGetPlayTime(Game game);


/*Set*/
int gameSetFirstPlayer(Game game, int first_player);
int gameSetSecondPlayer(Game game, int second_player);
Winner gameSetWinner(Game game, Winner game_winner);
int gameSetPlayTime(Game game,int play_time);


MapDataElement copyGameFunc(MapDataElement game);
MapKeyElement copyGameIdFunc(MapKeyElement game_id);
void freeGameFunc(MapDataElement game);
void freeGameIdFunc(MapKeyElement game_id);
int compareGameId(MapKeyElement game_id_1, MapKeyElement game_id_2);

#endif /* GAME_H_ */