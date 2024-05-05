/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : usercut.c
*/

#include "usercut.h"
#include "../cplex.h"
#include "../candidate/candidate.h"

#include <assert.h>

/*
 * 
 */
int violatedCuts(const TSPInstance* inst, const COMP* comp, CPXENVptr env, CPXLPptr lp, CPXCALLBACKCONTEXTptr context, int ncols){

	/* add concorde functions*/

	return 0;

}/* violatedCuts */

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
* Checks relaxed solution
*
* IP context CPLEX callback context, handled internally by CPLEX
* IP context_id id of the callback context
* IOP userhandle pointer to a structure external to CPLEX
*/
static int CPXPUBLIC checkRelaxedSol(CPXCALLBACKCONTEXTptr context, CPXLONG context_id, void *userhandle){

    CPXInstance* cpx_inst = (CPXInstance*) userhandle; 
    double objval = CPX_INFBOUND;
	int err = 0;
	
	COMP comp;
	allocComp(cpx_inst->inst->dimension, &comp);

	double* xstar = (double*) malloc(cpx_inst->ncols * sizeof(double));  
    assert(xstar != NULL);

    if((err = CPXcallbackgetrelaxationpoint(context, xstar, 0, cpx_inst->ncols - 1, &objval))){ 
        print_error("CPXcallbackgetrelaxationpoint error", err, cpx_inst->env, cpx_inst->lp);
		exit(1);
	}

	build_comp(cpx_inst, xstar, &comp);

	if(comp.nc > 1)
    	violatedCuts(cpx_inst->inst, &comp, cpx_inst->env, cpx_inst->lp, context, cpx_inst->ncols);
	
	freeComp(&comp);
	free(xstar);

	return 0;

}/* checkRelaxedSol */

/*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* IOP sol solution to be updated
* OR error code
*/
int usercutCallback(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol){

	int err = 0;

	COMP comp;
	allocComp(inst->dimension, &comp);

	TSPSSolution temp;
	allocSSol(inst->dimension, &temp);

	CPXInstance cpx_inst;
	initCPXInstance(&cpx_inst, inst, &temp, CPXgetnumcols(env, lp), env, lp);

    if((err = CPXcallbacksetfunc(env, lp, CPX_CALLBACKCONTEXT_CANDIDATE, checkCandidateSol, &cpx_inst)))
        print_error("CPXcallbacksetfunc() error: CandidateCallback", err, env, lp);
    
    if((err = CPXcallbacksetfunc(env, lp, CPX_CALLBACKCONTEXT_RELAXATION, checkRelaxedSol, &cpx_inst)))
        print_error("CPXcallbacksetfunc() error: RelaxationCallback", err, env, lp);

    optimize_model(inst, env, lp, &temp, &comp);

	convertSSol(inst, &temp, sol);

	freeSSol(&temp);
	freeComp(&comp);

	return 0;

}/* usercutCallback */
