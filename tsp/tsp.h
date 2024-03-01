/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.h
*/

#pragma once

#define MAX_NAME_LENGTH 64

typedef struct {
    int id;
    double x;
    double y;
} Point;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int dimension;
    Point* points;
} TSPInstance;

void allocInst(TSPInstance*);

void freeInst(TSPInstance*);
