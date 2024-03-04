/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.h
*/

#include "input/point/point.h"

#pragma once

#define MAX_NAME_LENGTH 64
#define POINTS_TO_PRINT 10
#define PLOT_STYLE "pointtype 7 pointsize 2 linewidth 2"

typedef struct {
    char name[MAX_NAME_LENGTH];
    int dimension;
    Point* points;
} TSPInstance;

typedef struct {
    int* succ;   /* succ := array containing the indexes of the nodes of the instance in the order they have to be visited */
    int val;  /* val  := value of the cost of the solution $succ */
} TSPSolution;

void allocInst(TSPInstance*);

void freeInst(TSPInstance*);

void allocSol(TSPInstance*, TSPSolution*);

void freeSol(TSPSolution*);

void printInst(const TSPInstance*);

void plotSolution(const TSPSolution*, const TSPInstance*);
