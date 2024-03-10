/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : 2opt.h
*/

#include "../../../tsp.h"

typedef enum {
	SKIP,
    OPT2
} REFINEMENT_ALGORITHM;

void runRefinement(const TSPInstance* inst, TSPSolution* sol);
