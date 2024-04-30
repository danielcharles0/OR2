/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : refinement.h
*/

#include "../../tsp.h"

typedef enum {
	SKIP,
    OPT2,
	TABU,
	TABU_TRIANG,
	TABU_SQUARE,
	TABU_SAWTOO,
	VNS
} REFINEMENT_ALGORITHM;

bool runRefinement(const Settings*, const TSPInstance*, TSPSolution*);
