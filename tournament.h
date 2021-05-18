#ifndef TOURNAMENT_H_
#define TOURNAMENT_H_

#include <stdbool.h>
#include "map.h"

#define TOURNAMENT_NULL_ARGUMENT -1

typedef struct tournament_t *Tournament;

/* ********************** */
/* Functions declarations */
/* ********************** */

Tournament tournamentCreate(int t_id, char* t_location, int max_games_per_player);
void tournamentDestroy(Tournament tournament);


/* ********************** */
/* ******** SET ********* */
/* ********************** */

void tournamentSetId(Tournament tournament, int id);
void tournamentSetMaxGames(Tournament tournament, int max_games_per_player);
void tournamentSetLocation(Tournament tournament, char* location);
void tournamentSetFinishedState(Tournament tournament, bool finished);
void tournamentSetWinnderId(Tournament tournament, int winner_id);

/* ********************** */
/* ******** GET ********* */
/* ********************** */
int tournamentGetId(Tournament tournament);
int tournamentGetMaxGames(Tournament tournament);
char* tournamentGetLocation(Tournament tournament);
bool tournamentGetFinishedState(Tournament tournament);
Map tournamentGetGames(Tournament tournament);
int tournamentGetWinnderId(Tournament tournament);

#endif