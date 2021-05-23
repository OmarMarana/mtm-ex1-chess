#ifndef PLAYER_H_
#define PLAYER_H_

#define PLAYER_NULL_ARGUMENT -1
#define PLAYER_INVALID_ID -1

typedef struct Player_t *Player;

typedef enum
{
    PLAYER_WINS = 1,
    PLAYER_LOSSES,
    PLAYER_DRAWS
} PlayerStaticsMode;



Player playerCreate(int player_id);
void playerDestroy(Player player);

/* SET */
void playerSetId(Player player, int id);
void playerSetLevel(Player player, double level);
void playerSetGameStatics(Player player, PlayerStaticsMode mode, int value);
void playerSetAvgPlayTime(Player player, double average_play_time);

/* GET */
int playerGetId(Player player);
double playerGetLevel(Player player);
int playerGetGameStatics(Player player, PlayerStaticsMode mode);
double playerGetAvgPlayTime(Player player);

#endif