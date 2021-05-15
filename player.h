#ifndef PLAYER_H_
#define PLAYER_H_

#define PLAYER_NULL_ARGUMENT -1

typedef struct Player_t *Player;



Player playerCreate(int p_id);
void playerDestroy(Player p);

/* SET */
void playerSetId(Player p, int id);
void playerSetLevel(Player p, double level);


/* GET */
int playerGetId(Player p);
double playerGetLevel(Player p);

#endif