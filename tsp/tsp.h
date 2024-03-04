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

typedef struct {
    char name[MAX_NAME_LENGTH];
    int dimension;
    Point* points;
} TSPInstance;

void allocInst(TSPInstance*);

void freeInst(TSPInstance*);

void printInst(const TSPInstance*);