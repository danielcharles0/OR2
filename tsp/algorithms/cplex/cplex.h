/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : cplex.h
*/

#include "../../tsp.h"

typedef enum{
    BENDERS
} EXACTS;

int optimize(const Settings*, const TSPInstance*, TSPSolution*);
