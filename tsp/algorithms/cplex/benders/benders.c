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
 * IP inst input instance
 * IP sol solution to patch
 * IP a node of the first component
 * IP b node of the second component
 * OR delta cost of the patch
 */
int delta_patch_cost(const TSPInstance *inst, const TSPSSolution *sol, int a, int b)
{

	return (getDist(a, (*sol).succ[b], inst) + getDist(b, (*sol).succ[a], inst)) - (getDist(a, (*sol).succ[a], inst) + getDist(b, (*sol).succ[b], inst));

} /* delta_patch_cost */

/*
 * IP inst input instance
 * IP c1 first component
 * IP c2 second component
 * IOP sol solution to patch
 * IOP comp array of components
 */
void merge_components(const TSPInstance *inst, int c1, int c2, TSPSSolution *sol, COMP *comp)
{

	int i, sc = c1, uc = c2;

	if (c2 < c1)
	{
		sc = c2;
		uc = c1;
	} /* if */

	/* DO WE NEED CONSECUTIVE COMPONENTS OR WE CAN JUST GO THROW SUCC WHEN WE FIND THE FIRST INDEX OF THE COMPONENT? */
	for (i = 0; i < (*inst).dimension; i++)
		if ((*comp).map[i] == uc)
			(*comp).map[i] = sc;
		else if ((*comp).map[i] > uc)
			(*comp).map[i]--;

	(*comp).nc--;

} /* merge_components */

/*
 * IP inst input instance
 * IP a node of the first component
 * IP b node of the second component
 * IOP sol solution to patch
 * IOP comp array of components
 */
void patch_components(const TSPInstance *inst, int a, int b, TSPSSolution *sol, COMP *comp)
{

	int a1 = (*sol).succ[a];

	(*sol).val += delta_patch_cost(inst, sol, a, b);

	(*sol).succ[a] = (*sol).succ[b];
	(*sol).succ[b] = a1;

	merge_components(inst, (*comp).map[a], (*comp).map[b], sol, comp);

} /* patch_components */

/*
 * IP set settings
 * IP inst input instance
 * IOP sol solution to patch
 * IOP comp array of components
 */
void dummypatch(const Settings *set, const TSPInstance *inst, TSPSSolution *sol, COMP *comp){}

/*
 * IP set settings
 * IP inst input instance
 * IOP sol solution to patch
 * IOP comp array of components
 */
void patch(const Settings *set, const TSPInstance *inst, TSPSSolution *sol, COMP *comp)
{

	int i, j, bi, bj, bdc;

	/*if ((*set).v)
		printf("\nStarting patching phase..\n");*/

	while ((*comp).nc > 1)
	{
		bdc = INT_MAX;
		for (i = 0; i < (*inst).dimension; i++)
			for (j = i + 1; j < (*inst).dimension; j++)
				/* I will not go twice on the same pair so i can check just for different components */
				if ((*comp).map[i] != (*comp).map[j])
				{
					int delta;
					if ((delta = delta_patch_cost(inst, sol, i, j)) < bdc)
					{
						bi = i;
						bj = j;
						bdc = delta;
					}
				} /* if */
		patch_components(inst, bi, bj, sol, comp);
	} /* while */

	(*sol).val = getSSolCost(inst, sol);

	/*if ((*set).v)
		printf("\nPatching phase ended.\n");*/

} /* patch */

/*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* IOP sol solution to be updated
* IP ptc patching function
* OR 0 if no error, error code otherwise
* OV error message if any
*/
int benders(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, patchfunc ptc, bool warm_start){
	
	int err;
	double lb = 0, ls = -1;
	clock_t start = clock();
	TSPSSolution temp;
	COMP comp;
	
	allocComp(inst->dimension, &comp);
	allocSSol(inst->dimension, &temp);

	checkTimeLimit(set, start, &ls);

	if(warm_start){
        if((err = mip_start(set, inst, env, lp)))
            return err;
        update_time_limit(set, start, env);
    }

	while(true){

		if((err = optimize_model(inst, env, lp, &temp, &comp)))
			break;
		
		if((err = CPXgetbestobjval(env, lp, &lb)))
			break;
		
		if(comp.nc == 1){
			printf("\n\n");
			break;
		}/* if */

		if((*set).v)
			printf(", LB: %f", lb);
		
		if((err = add_SEC(inst, &comp, env, lp)))
			break;

		ptc(set, inst, &temp, &comp);

		update_time_limit(set, start, env, lp);

		if(checkTimeLimit(set, start, &ls))
			break;

	}/* while */

	if(!err){
		if(comp.nc != 1){
			Settings s;
			cpSet(set, &s);
			s.v = 0;
			best_start(&s, inst, sol);
			opt2(&s, inst, sol);
			printf("Lower Bound: %lf\nBest Solution found: %lf\nGAP: %4.2lf%%\n\n", lb, sol->val, solGap(sol, lb));
		} else
			convertSSol(inst, &temp, sol);
	} else
		print_error("Error in BENDERS", err, env, lp);

	freeSSol(&temp);
	freeComp(&comp);

	return err;

}/* benders */
