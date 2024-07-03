/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : hardfixing.c
*/

#include <stdio.h>
#include <assert.h>
#include "../../cplex.h"
#include "hardfixing.h"
#include "../../usercut/usercut.h"
#include "../../../../utility/utility.h"

#define MIP_TIMELIMIT_FRACTION 10.

/*
 * IP set settings
 * IP start execution time
 * OP mipset settings to update timelimit
 * OP env CPLEX environment
 * OP lp CPLEX linear program
 */
void update_solver_time_limit(const Settings* set, clock_t start, Settings* mipset, CPXENVptr env, CPXLPptr lp)
{
	double /* remining time */ rt = step((*set).tl - getSeconds(start));

	(*mipset).tl = min_dbl((*set).tl / MIP_TIMELIMIT_FRACTION, rt);

	if(setdblparam(CPX_PARAM_TILIM, (*mipset).tl, env, lp))
		exit(1);

} /* update_time_limit */

/*
* IP inst tsp instance
* IP sol solution
* OP fe the edges fixed (index of the CPLEX variable position), array assumed to be already allocated
*/
void convertSolIndicesIntoCPXXPos(const TSPInstance* inst, const TSPSolution* sol, ArrayDinaInt* fe){

	int i;

	for (i = 0; i < (*fe).n; i++){
		int si = fe->v[i];
		int a = (*sol).path[si], b = (*sol).path[(si + 1) % (*inst).dimension];
		fe->v[i] = xpos(a, b, inst);
	}/* for */

}/* convertSolIndicesIntoCPXXPos */

/*
* IP set settings
* IP inst tsp instance
* IP sol current best tsp solution
* IP lb lower bound array
* OP env CPLEX environment
* OP lp CPLEX linear program
* OP fe the edges fixed (index of the CPLEX variable position), array assumed to be already allocated
*/
void fix_edges(const Settings* set, const TSPInstance* inst, const TSPSolution* sol, const char* lbc, double *lb[2], CPXENVptr env, CPXLPptr lp, ArrayDinaInt* fe){

	ArrayDinaInt soledges;
	
	soledges.n = (*inst).dimension;
	soledges.v = (*sol).path;

	// if((*set).v)
	// 	printf("Fixing %d edges\n", fe->n);

	reservoirSamplingIndices(&soledges, fe);

	convertSolIndicesIntoCPXXPos(inst, sol, fe);

	CPXchgbds(env, lp, fe->n, fe->v, lbc, lb[1]);

	// if((*set).v)
	// 	printf("%d edges fixed\n", fe->n);

}/* fix_edges */

/*
* IP inst tsp instance
* OP lb lower bound array to be allocated and initialized
*/
void initLB(const TSPInstance* inst, char** lbc, double *lb[2]){

	int i;

	*lbc = malloc(inst->dimension * sizeof(char));
	assert(*lbc != NULL);

	lb[0] = malloc(inst->dimension * sizeof(double));
	assert(lb[0] != NULL);

	lb[1] = malloc(inst->dimension * sizeof(double));
	assert(lb[1] != NULL);

	for(i = 0; i < inst->dimension; i++){
		(*lbc)[i] = 'L';
		lb[0][i] = 0.;
		lb[1][i] = 1.;
	}/* for */

}/* initLB */

/*
* IP set settings
* IP inst tsp instance
* IP fef fixed edges fraction (0, 1)
* IP env CPLEX environment
* IP lp CPLEX linear program
* OP sol solution
* OP et execution time in seconds
* OR 0 if no error, error code otherwise
*/
int hard_fixing(const Settings* set, const TSPInstance* inst, double fef, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, double* et){
	
	int err;
	char *lbc;
	double mipet, *lb[2], ls = -1;
	ArrayDinaInt fe;
	Settings mipset;
	TSPSolution temp;
	clock_t start = clock();

	if(fef <= 0 || fef >= 1){
		printf("Error: wrong parameter, the fixed edges fraction must belong to the interval (0, 1).");
		exit(1);
	}/* if */

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

	initArrayDinaInt((*inst).dimension, &fe);
	allocSol((*inst).dimension, &temp);
	
	initLB(inst, &lbc, lb);

	fe.n = ((double)(*inst).dimension) * fef; /* we can make it dinamic by analizing wheater the mip solver does not find new good solutions or it exeedes the tl */

	if((*set).v)
		printf("Running hard fixing with %d fixed edges:\n\n", fe.n);

	checkTimeLimit(set, start, &ls);

	while (true){
		
		update_solver_time_limit(set, start, &mipset, env, lp);

		fix_edges(set, inst, sol, lbc, lb, env, lp, &fe);
		
		/* TO CHECK THAT IT IS THE BEST METHOD by doing perfprof */
		if(callback_solver(&mipset, inst, env, lp, (callback_installer)usercut, &temp, false, &mipet)){
			if((*set).v)
				printf("Error while calling the solver.\nReturning best found solution so far.");
			break;
		}/* if */

		if(updateIncumbentSol(inst, &temp, sol))
			if((*set).v)
				printf(" Better solution cost found: %lf", sol->val);

		// if(mipet > (mipset.tl)); /* we reached the timelimit */
		// update fe.n

		if(checkTimeLimit(set, start, &ls))
			break;

		/* Free edges */
		CPXchgbds(env, lp, fe.n, fe.v, lbc, lb[0]);

	}/* while */

	free(lb[1]);
	free(lb[0]);
	free(lbc);
	freeSol(&temp);
	freeArrayDinaInt(&fe);

	*et = getSeconds(start);
	
	return 0;

}/* hard_fixing */
