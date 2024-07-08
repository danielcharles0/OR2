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
	/* EXACTS */
    BENDERS,
	BENDERS_PATCH,
    CANDIDATE_CALLBACK,
    USERCUT_CALLBACK,
	__END,
	/* MATHEURISTICS */
	_HARD_FIXING,
	_LOCAL_BRANCHING
} EXACTS;

typedef enum {
    HARD_FIXING,
	LOCAL_BRANCHING
} MATHEURISTICS;

typedef struct{
    
    int nc;		/* nc	:= number of components */
    int* map;	/* map	:= array of components */

} COMP;

typedef struct{

    int ncols;

	CPXLPptr lp;
	CPXENVptr env;
	const Settings* set;
	const TSPInstance* inst;
	
	/* Working memory allocated just once */
	int *indices, **elist, **sec_idxs, **usercut_comps;
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

int optimize_offline(const Settings*, const TSPInstance*, bool, EXACTS, double, TSPSolution*, double*);

int xpos(int, int, const TSPInstance*);

void print_error(const char*, int, CPXENVptr, CPXLPptr);

void update_time_limit(const Settings*, time_t, CPXENVptr, CPXLPptr);

void allocComp(int, COMP*);

void freeComp(COMP*);

int build_sol(const TSPInstance*, CPXENVptr, CPXLPptr, TSPSSolution*, COMP*);

int optimize_model(const TSPInstance*, CPXENVptr, CPXLPptr, TSPSSolution*, COMP*);

double solGap(const TSPSolution*, double);

void allocCPXInstance(CPXInstance*, const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr);

void freeCPXInstance(CPXInstance*);

int mip_start(const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr);

void build_sol_xstar(const TSPInstance*, const double*, TSPSSolution*, COMP*);

void postPatchedSSol2CPX(CPXInstance*, CPXCALLBACKCONTEXTptr, COMP*, TSPSSolution*);

int callback_solver(const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr, callback_installer, TSPSolution*, bool, double*);

int setdblparam(int, double, CPXENVptr, CPXLPptr);

int matheur(const Settings*, const TSPInstance*, TSPSolution*);

void update_solver_time_limit(const Settings*, time_t, Settings*, CPXENVptr, CPXLPptr);

void update_solver_time_limit_fraction(const Settings*, time_t, double, Settings*, CPXENVptr, CPXLPptr);
