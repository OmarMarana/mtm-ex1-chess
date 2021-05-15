#ifndef TOURNAMENT_H_
#define TOURNAMENT_H_

#include <stdbool.h>
#include "map.h"


typedef struct tournament_t *Tournament;

/* ********************** */
/* Functions declarations */
/* ********************** */

Tournament tournamentCreate(int t_id, char* t_location, int max_games_per_player);
void tournamentDestroy(Tournament t);


/* ********************** */
/* SET */
/* ********************** */
void tournamentSetId(Tournament p, int id);
void tournamentSetMaxGames(Tournament p, int max_games_per_player);
void tournamentSetLocation(Tournament p, char* location);
void tournamentSetFinishedState(Tournament p, bool finished);

/* ********************** */
/* GET */
/* ********************** */
int tournamentGetId(Tournament p);
int tournamentGetMaxGames(Tournament p);
char* tournamentGetLocation(Tournament p);
bool tournamentGetFinishedState(Tournament p);
Map tournamentGetGames(Tournament p);

#endif