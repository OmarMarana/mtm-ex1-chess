#include "game.h"
#include <stdlib.h>


struct Game_t
{
    int game_id;
    int first_player;
    int second_player;
    Winner game_winner;
    int play_time;
    bool first_removed;
    bool second_removed;
};



/* ************************ */
/* MAP Functions definition */
/* ************************ */



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

void gameRemoveFirst(Game game)
{
    game->first_removed = true;
}

void gameRemoveSecond(Game game)
{
    game->second_removed = true;
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

int gameGetId(Game game)
{
    return game->game_id;
}

bool gameFirstRemoved(Game game)
{
    return game->first_removed;
}

bool gameSecondRemoved(Game game)
{
    return game->second_removed;
}


/*gameCreate*/
Game gameCreate(int first_player, int second_player, Winner game_winner, int play_time, int game_id)
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
    new_game->game_id = game_id;
    new_game->first_removed = false;
    new_game->second_removed = false;

    return new_game;
}



/*gameDestroy*/
void gameDestroy(Game game)
{
    free(game);
}
