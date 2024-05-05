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
	BENDERS_PATCH,
    CANDIDATE_CALLBACK,
    USERCUT_CALLBACK
} EXACTS;

typedef struct{
    
    int nc;		/* nc	:= number of components */
    int* map;	/* map	:= array of components */

} COMP;

typedef struct{

    const TSPInstance* inst;
    TSPSSolution* temp;
    int ncols;
    CPXENVptr env;
    CPXLPptr lp;

} CPXInstance;

int optimize(const Settings*, const TSPInstance*, TSPSolution*);

int xpos(int, int, const TSPInstance*);

void print_error(const char*, int, CPXENVptr, CPXLPptr);

void update_time_limit(const Settings*, clock_t, CPXENVptr);

void allocComp(int, COMP*);

void freeComp(COMP*);

int build_sol(const TSPInstance*, CPXENVptr, CPXLPptr, TSPSSolution*, COMP*);

int optimize_model(const TSPInstance*, CPXENVptr, CPXLPptr, TSPSSolution*, COMP*);

double solGap(const TSPSolution*, double);

void initCPXInstance(CPXInstance*, const TSPInstance*, TSPSSolution*, int, CPXENVptr, CPXLPptr);

void build_comp(CPXInstance*, double*, COMP*);
