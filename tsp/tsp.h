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

typedef enum {
    RANDOM,
    NEAREST_NEIGHBOR,
	/* HERE MORE HEURISTICS */
	__END_HEURISTIC
} ALGORITHM;

void allocInst(int, TSPInstance*);

void allocSol(int, TSPSolution*);

void freeInst(TSPInstance*);

void freeSol(TSPSolution*);

void computeDistances(TSPInstance*);

void printInst(const TSPInstance*);

void algorithmLegend(void);

bool run(ALGORITHM, const TSPInstance*, TSPSolution*, const Settings*);

double getDist(int, int, const TSPInstance*);

void ascendentSol(const TSPInstance*, TSPSolution*);

double getSolCost(const TSPInstance*, const TSPSolution*);

bool checkSol(const TSPInstance*, const TSPSolution*);

bool isDistinct(int, int*);

void cpSol(const TSPInstance*, const TSPSolution*, TSPSolution*);
