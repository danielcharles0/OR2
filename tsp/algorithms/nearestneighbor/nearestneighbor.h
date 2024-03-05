/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : nearestneighbor.h
*/

#include "../../tsp.h"
#include "../../input/settings/settings.h"

void NN_initSol(const TSPInstance*, TSPSolution*);

int NN_solver(const TSPInstance*, TSPSolution*);

void NN_controller(void);
