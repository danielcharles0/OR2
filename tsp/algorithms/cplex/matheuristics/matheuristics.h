/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : metaheuristics.h
*/

#include "../../../tsp.h"
#include "../../../input/settings/settings.h"

typedef enum {
    HARD_FIXING,
	LOCAL_BRANCHING
} MATHEURISTICS;

int matheur(const Settings*, const TSPInstance*, TSPSolution*);

int hard_fixing(const Settings*, const TSPInstance*, double, CPXENVptr, CPXLPptr, TSPSolution*, double*);
