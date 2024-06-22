/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : 2opt.h
*/

#include "../../../tsp.h"

double opt2(const Settings*, const TSPInstance*, TSPSolution*);

void opt2move(int, int, const TSPInstance*, TSPSolution*);

double delta2OptMoveCost(int, int, const TSPInstance*, const TSPSolution*);
