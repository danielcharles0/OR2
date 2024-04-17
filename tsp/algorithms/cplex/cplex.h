/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : cplex.h
*/

#pragma once

#include <ilcplex/cplex.h>
#include <time.h>
#include "../../tsp.h"

typedef enum{
    BENDERS,
	BENDERS_PATCH
} EXACTS;

typedef struct{
    
    int nc;		/* nc	:= number of components */
    int* map;	/* map	:= array of components */

} COMP;

int optimize(const Settings*, const TSPInstance*, TSPSolution*);

int xpos(int, int, const TSPInstance*);

void print_error(const char*, int, CPXENVptr, CPXLPptr);

void update_time_limit(const Settings*, clock_t, CPXENVptr);

void allocComp(int, COMP*);

void freeComp(COMP*);

int optimize_model(const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr, TSPSSolution*, COMP*);

double solGap(const TSPSolution*, double);
