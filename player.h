#ifndef PLAYER_H_
#define PLAYER_H_

#define PLAYER_NULL_ARGUMENT -1
#define PLAYER_INVALID_ID -1

typedef struct Player_t *Player;



Player playerCreate(int player_id);
void playerDestroy(Player player);

/* SET */
void playerSetId(Player player, int id);
void playerSetLevel(Player player, double level);


/* GET */
int playerGetId(Player player);
double playerGetLevel(Player player);

#endif