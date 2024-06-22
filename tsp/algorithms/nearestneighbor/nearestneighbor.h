/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : nearestneighbor.h
*/

#include "../../tsp.h"
#include "../../input/settings/settings.h"

typedef enum{
    START_FIRST_NODE,
	START_RANDOM_NODE,
	SELECT_STARTING_NODE,
    BEST_START
} NN_CONFIG;

int nearestNeighbor(const Settings*, const TSPInstance*, TSPSolution*);

double best_start(const Settings*, const TSPInstance*, TSPSolution*);

int NNRunConfiguration(NN_CONFIG, const Settings*, const TSPInstance*, TSPSolution*);
