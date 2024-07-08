/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : localbranching.c
*/

#include <assert.h>
#include "../../cplex.h"
#include "localbranching.h"
#include "../../usercut/usercut.h"
#include "../../../../utility/utility.h"

#define K_START 10
#define K_STEP 5

/*
* IP inst tsp instance
* IP sol solution
* IP k number of edges that can change starting from the current best solution
* IP vls coefficients
* OP idxs indexes of the involved CPLEX variables
* OP env CPLEX environment
* OP lp CPLEX linear program
*/
void fix_dges(const TSPInstance* inst, const TSPSolution* sol, int k, const double* vls, int* idxs, CPXENVptr env, CPXLPptr lp){

	int i, err;
	const int zero = 0;
	const char sense = 'G';
	const double rhs = (*inst).dimension - k;
	
	for(i = 0; i < inst->dimension; i++)
		idxs[i] = xpos(sol->path[i], sol->path[(i + 1) % inst->dimension], inst);

	if ((err = CPXaddrows(env, lp, 0, 1, (*inst).dimension, &rhs, &sense, &zero, idxs, vls, NULL, NULL))){
		print_error("Wrong CPXaddrows(..)", err, env, lp);
		exit(1);
	} /* if */

}/* fix_dges */

/*
* IP n size of the array
* OP vls array to allocate and initialize
*/
void initIdxsVls(int n, int** idxs, double** vls){
	
	int i;
	
	*idxs = malloc(n * sizeof(int));
	assert(*idxs != NULL);

	*vls = malloc(n * sizeof(double));
	assert(*vls != NULL);

	for(i = 0; i < n; i++)
		(*vls)[i] = 1.;

}/* initVls */

/*
* IP inst tsp instance
* OP env CPLEX environment
* OP lp CPLEX linear program
*/
void release_edges(const TSPInstance* inst, CPXENVptr env, CPXLPptr lp){

	int err;
	
	/* We have 1 row for each degree constraint + the fix edges row. The indexes starts by 0 */
	if ((err = CPXdelrows(env, lp, inst->dimension, inst->dimension))){
		print_error("CPXdelrows() error", err, env, lp);
		exit(1);
	} /* if */

}/* release_edges */

/*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* OP sol solution
* OP et execution time in seconds
* OR 0 if no error, error code otherwise
*/
int local_branching(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, double* et){

	int *idxs, err, k = K_START;
	double *vls, mipet, ls = -1;
	Settings mipset;
	TSPSolution temp;
	clock_t start = clock();

	if((*set).v)
		printf("Running heuristics and refinement:\n\n");

	if((err = offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, OPT2, inst, sol, set))){
		if((*set).v)
			printf("Error while computing the heuristic solution.");
		return err;
	}/* if */

	if((*set).v)
		printf("\n\nInitial cost: %lf\n\n", sol->val);

	cpSet(set, &mipset);
	mipset.v = 0;

	allocSol((*inst).dimension, &temp);
	
	initIdxsVls(inst->dimension, &idxs, &vls);

	checkTimeLimit(set, start, &ls);

	while (true){
		
		/* true if an improvement was made by the solver */
		bool imp;

		update_solver_time_limit_fraction(set, start, 1, &mipset, env, lp);

		fix_dges(inst, sol, k, vls, idxs, env, lp);

		if(set->v)
			printf(" | Blocking %2d edges", k);
		
		if(callback_solver(&mipset, inst, env, lp, (callback_installer)usercut, &temp, false, &mipet)){
			if((*set).v)
				printf("Error while calling the solver.\nReturning best found solution so far.");
			break;
		}/* if */

		if((imp = updateIncumbentSol(inst, &temp, sol)))
			if((*set).v)
				printf(" | Best solution cost found: %lf", sol->val);

		if(checkTimeLimit(set, start, &ls))
			break;

		if(!imp)
			k += K_STEP;

		release_edges(inst, env, lp);

	}/* while */

	free(vls);
	free(idxs);
	freeSol(&temp);

	*et = getSeconds(start);

	return err;

}/* local_branching */
