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

    int ncols;
    int* indices;

	CPXLPptr lp;
	CPXENVptr env;
	const Settings* set;
	const TSPInstance* inst;
	
	/* Working memory allocated just once */
	int** sec_idxs;
	double** sec_vls;
	double** xstars;
	TSPSolution* sols;
	TSPSSolution* ssols;
	COMP* comps;

} CPXInstance;

/*
* Installs the candidate callback
*
* IP env CPLEX environment
* IP lp CPLEX linear program
* OP cpx_inst cplex instance
* OR error code
*/
typedef int (*callback_installer)(CPXENVptr, CPXLPptr, CPXInstance*);

int optimize(const Settings*, const TSPInstance*, TSPSolution*);

int optimize_offline(const Settings*, const TSPInstance*, bool, EXACTS, TSPSolution*, double*);

int xpos(int, int, const TSPInstance*);

void print_error(const char*, int, CPXENVptr, CPXLPptr);

void update_time_limit(const Settings*, clock_t, CPXENVptr, CPXLPptr);

void allocComp(int, COMP*);

void freeComp(COMP*);

int build_sol(const TSPInstance*, CPXENVptr, CPXLPptr, TSPSSolution*, COMP*);

int optimize_model(const TSPInstance*, CPXENVptr, CPXLPptr, TSPSSolution*, COMP*);

double solGap(const TSPSolution*, double);

void allocCPXInstance(CPXInstance*, const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr);

void freeCPXInstance(CPXInstance*);

int mip_start(const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr);

void build_sol_xstar(const TSPInstance*, const double*, TSPSSolution*, COMP*);

void postCPXSol(const CPXInstance*, CPXCALLBACKCONTEXTptr, const TSPSSolution*);
