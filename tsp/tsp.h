/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.h
*/

#pragma once
#include "point/point.h"

#define MAX_NAME_LENGTH 64
#define POINTS_TO_PRINT 10

typedef struct {
    char name[MAX_NAME_LENGTH];
    int dimension;
    Point2D* points;
    int** dist;     /* dist  := precomputed distances among nodes */
} TSPInstance;

typedef struct {
    int* succ;	/* succ := array containing the indexes of the nodes of the instance in the order they have to be visited */
    int val;	/* val  := value of the cost of the solution $succ */
} TSPSolution;

void allocInst(int, TSPInstance*);

void freeInst(TSPInstance*);

void allocSol(int, TSPSolution*);

void freeSol(TSPSolution*);

void printInst(const TSPInstance*);

void plotSolution(const TSPSolution*, const TSPInstance*);
