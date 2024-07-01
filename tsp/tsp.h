/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.h
*/

#pragma once
#include <stdbool.h>

#include "point/point.h"
#include "input/settings/settings.h"

#define MAX_NAME_LENGTH 64
#define POINTS_TO_PRINT 10

typedef struct {
    char name[MAX_NAME_LENGTH];
    int dimension;
    Point2D* points;
	double** dist;	/* dist  := precomputed distances among nodes */
} TSPInstance;

typedef struct {
    int* path;	/* path := array containing the indexes of the nodes of the instance in the order they have to be visited */
    double val;	/* val  := value of the cost of the solution $path */
} TSPSolution;

typedef struct {
    int* succ;	/* succ := array containing the successor nodes of the instance */
    double val;	/* val  := value of the cost of the solution $path */
} TSPSSolution;

typedef enum {
    /* HEURISTICS */
	RANDOM,
    NEAREST_NEIGHBOR,
	__END_HEURISTIC,
	/* EXACTS */
	CPLEX,
	__END_EXACTS,
	/* MATHEURISTICS */
	MATHEURISTIC
} ALGORITHM;

typedef enum {
	SKIP,
    OPT2,
	TABU,
	TABU_TRIANG,
	TABU_SQUARE,
	TABU_SAWTOO,
	VNS
} REFINEMENT_ALGORITHM;

typedef enum {
    O_RANDOM,
	O_NEAREST_NEIGHBOR_START_FIRST_NODE,
    O_NEAREST_NEIGHBOR_START_RANDOM_NODE,
	O_NEAREST_NEIGHBOR_BEST_START,
	O___END_HEURISTIC//,
	// O_CPLEX
} OFFLINE_ALGORITHM;

void allocInst(int, TSPInstance*);

void allocSol(int, TSPSolution*);

void allocSSol(int, TSPSSolution*);

void freeInst(TSPInstance*);

void freeSol(TSPSolution*);

void freeSSol(TSPSSolution*);

void computeDistances(TSPInstance*);

void printInst(const TSPInstance*);

void algorithmLegend(void);

bool run(ALGORITHM, const TSPInstance*, TSPSolution*, const Settings*);

double getDist(int, int, const TSPInstance*);

void ascendentSol(const TSPInstance*, TSPSolution*);

double getSolCost(const TSPInstance*, const TSPSolution*);

double getSSolCost(const TSPInstance*, const TSPSSolution*);

bool checkSol(const TSPInstance*, const TSPSolution*);

bool isDistinct(int, int*);

void cpSol(const TSPInstance*, const TSPSolution*, TSPSolution*);

void cpSSol(const TSPInstance*, const TSPSSolution*, TSPSSolution*);

bool updateIncumbentSol(const TSPInstance*, const TSPSolution*, TSPSolution*);

void convertSSol(const TSPInstance*, const TSPSSolution*, TSPSolution*);

bool offline_run_refinement(OFFLINE_ALGORITHM, REFINEMENT_ALGORITHM, const TSPInstance*, TSPSolution*, const Settings*);
