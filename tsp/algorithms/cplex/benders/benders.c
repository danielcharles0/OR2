/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : benders.c
*/

#include <time.h>
#include <assert.h>
#include "benders.h"
#include "../../nearestneighbor/nearestneighbor.h"
#include "../../refinement/2opt/2opt.h"
#include "../../../utility/utility.h"

#define MAX_LINE_LENGTH 133 /* 132 characters + '\n' */

/*
 * IP inst input instance
 * IP comp component array storing connected component each node belongs to
 * IP env CPLEX environment
 * IP lp CPLEX linear program
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int add_SEC(const TSPInstance* inst, const COMP* comp, CPXENVptr env, CPXLPptr lp){
	
	int ncols = CPXgetnumcols(env, lp);

	int* idxs = malloc(ncols * sizeof(int));
	assert(idxs != NULL);

	double* vls = malloc(ncols * sizeof(double));
	assert(vls != NULL);

	char** en = malloc(sizeof(char*));
	assert(en != NULL);

	*en = "SEC";
	
	for(int k = 1; k <= (*comp).nc; k++){

		const int zero = 0;
		const char sense = 'L';
		int err, nnz = 0;
		double rhs = -1.0;

		for(int i = 0; i < inst->dimension - 1; i++)
			if((*comp).map[i] == k){
				
				rhs += 1;

				for(int j = i + 1; j < inst->dimension; j++)
					if((*comp).map[j] == k){
						idxs[nnz] = xpos(i, j, inst);
						vls[nnz] = 1;
						nnz++;
					}/* if */
			}/* if */
		
		if((err = CPXaddrows(env, lp, 0, 1, nnz, &rhs, &sense, &zero, idxs, vls, NULL, en))){
			print_error("Wrong CPXaddrows(..)", err, env, lp);
			return err;
		} /* if */

	}/* for */

	free(vls);
	free(idxs);
	free(en);

	return 0;

}/* add_SEC */

/*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* IOP sol solution to be updated
*/
int benders(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol){
	
	int err;
	double lb = 0, ls = -1;
	clock_t start = clock();
	TSPSSolution temp;
	COMP comp;
	
	allocComp(inst->dimension, &comp);
	allocSSol(inst->dimension, &temp);

	checkTimeLimit(set, start, &ls);

	while(true){
		
		err = CPXmipopt(env, lp);

		if(err){
			print_error("CPXmipopt failed.", err, env, lp);
			break;
		}/* if */
		
		CPXgetbestobjval(env, lp, &lb);
		
		build_sol(set, inst, env, lp, &temp, &comp);
		
		if(comp.nc == 1){
			printf("\n\n");
			break;
		}/* if */

		update_time_limit(set, start, env);
		if(checkTimeLimit(set, start, &ls))
			break;
		
		add_SEC(inst, &comp, env, lp);

	}/* while */

	if(comp.nc != 1){
		Settings s;
		cpSet(set, &s);
		s.v = 0;
		best_start(&s, inst, sol);
		opt2(inst, sol);
		printf("Lower Bound: %lf\nBest Solution found: %lf\nGAP: %4.2lf%%\n\n", lb, sol->val, solGap(sol, lb));
	} else
		convertSSol(inst, &temp, sol);

	freeSSol(&temp);
	freeComp(&comp);

	return 0;

}/* benders */
