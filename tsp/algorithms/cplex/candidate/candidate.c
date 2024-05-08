/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : candidate.c
*/

#include <assert.h>

#include "candidate.h"
#include "../cplex.h"

/*
 * IP inst input instance
 * IP comp component array storing connected component each node belongs to
 * IP env CPLEX environment
 * IP lp CPLEX linear program
 * IP context cplex context
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int add_SEC_candidate(const TSPInstance* inst, const COMP* comp, CPXENVptr env, CPXLPptr lp, CPXCALLBACKCONTEXTptr context, int ncols){

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
		
		if(nnz > 0) /* means that the solution is infeasible and a violated cut has been found */
		{	
			if ((err = CPXcallbackrejectcandidate(context, 1, nnz, &rhs, &sense, &zero, idxs, vls))){
				print_error("CPXcallbackrejectcandidate() error", err, env, lp); 
				exit(1);
			}
		}

	}/* for */

	free(vls);
	free(idxs);
	free(en);

	return 0;

}/* add_SEC_candidate */

/*
* Checks candidate solution
*
* IP context CPLEX callback context, handled internally by CPLEX
* IP context_id id of the callback context
* IOP userhandle pointer to a structure external to CPLEX
*/
static int CPXPUBLIC checkCandidateSol(CPXCALLBACKCONTEXTptr context, CPXLONG context_id, void *userhandle){

    CPXInstance* cpx_inst = (CPXInstance*) userhandle; 
    double objval = CPX_INFBOUND;
	int err = 0;
	
	COMP comp;
	allocComp(cpx_inst->inst->dimension, &comp);

	double* xstar = (double*) malloc(cpx_inst->ncols * sizeof(double));  
    assert(xstar != NULL);

    if((err = CPXcallbackgetcandidatepoint(context, xstar, 0, cpx_inst->ncols - 1, &objval))){ 
        print_error("CPXcallbackgetcandidatepoint error", err, cpx_inst->env, cpx_inst->lp);
		exit(1);
	}

	build_comp(cpx_inst, xstar, &comp);

	if(comp.nc > 1)
    	add_SEC_candidate(cpx_inst->inst, &comp, cpx_inst->env, cpx_inst->lp, context, cpx_inst->ncols);
	
	freeComp(&comp);
	free(xstar);

	return 0;

}/* checkCandidateSol */

/*
* Installs the candidate callback and runs the solver.
*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* IOP sol solution to be updated
* OR error code
*/
int candidateCallback(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, bool warm_start){

    CPXLONG context_id = CPX_CALLBACKCONTEXT_CANDIDATE;
	int err = 0;
	clock_t start = clock();

	COMP comp;
	allocComp(inst->dimension, &comp);

	TSPSSolution temp;
	allocSSol(inst->dimension, &temp);

	CPXInstance cpx_inst;
	initCPXInstance(&cpx_inst, inst, &temp, CPXgetnumcols(env, lp), env, lp);

	if(warm_start){
        if((err = mip_start(set, inst, env, lp)))
            return err;
        update_time_limit(set, start, env, lp);
    }

    if((err = CPXcallbacksetfunc(env, lp, context_id, checkCandidateSol, &cpx_inst))){
        print_error("CPXcallbacksetfunc() error", err, env, lp);
		return err;
	}
    optimize_model(inst, env, lp, &temp, &comp);

	convertSSol(inst, &temp, sol);

	freeSSol(&temp);
	freeComp(&comp);

	return 0;

}/* candidateCallback */
