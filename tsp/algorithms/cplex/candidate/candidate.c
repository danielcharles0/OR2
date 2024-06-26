/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : candidate.c
*/

#include <assert.h>
#include <string.h>

#include "candidate.h"
#include "../cplex.h"
#include "../benders/benders.h"
#include "../../refinement/2opt/2opt.h"
#include "../../../utility/utility.h"

/*
 * IP cpx_inst input instance
 * IP comp component array storing connected component each node belongs to
 * IP env CPLEX environment
 * IP lp CPLEX linear program
 * IP context cplex context
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int add_SEC_candidate(CPXInstance* cpx_inst, const COMP* comp, CPXENVptr env, CPXLPptr lp, CPXCALLBACKCONTEXTptr context, int ncols){

	int thread_id, *idxs;
	double* vls;

	CPXcallbackgetinfoint(context, CPXCALLBACKINFO_THREADID, &thread_id);

	idxs = cpx_inst->sec_idxs[thread_id];
	vls = cpx_inst->sec_vls[thread_id];
	
	for(int k = 1; k <= (*comp).nc; k++){

		const int zero = 0;
		const char sense = 'L';
		int err, nnz = 0;
		double rhs = -1.0;

		for(int i = 0; i < (*cpx_inst).inst->dimension; i++)
			if((*comp).map[i] == k){
				
				rhs += 1;

				for(int j = i + 1; j < (*cpx_inst).inst->dimension; j++)
					if((*comp).map[j] == k){
						idxs[nnz] = xpos(i, j, (*cpx_inst).inst);
						vls[nnz] = 1.0;
						nnz++;
					}/* if */
			}/* if */
		
		if(nnz > 0) /* means that the solution is infeasible and a violated cut has been found */
			if ((err = CPXcallbackrejectcandidate(context, 1, nnz, &rhs, &sense, &zero, idxs, vls))){
				print_error("CPXcallbackrejectcandidate() error", err, env, lp);
				exit(1);
			}/* if */

	}/* for */

	return 0;

}/* add_SEC_candidate */

/*
* Checks candidate solution
*
* IP context CPLEX callback context, handled internally by CPLEX
* IP context_id id of the callback context
* IOP userhandle pointer to a structure external to CPLEX
*/
int CPXPUBLIC checkCandidateSSol(CPXCALLBACKCONTEXTptr context, CPXLONG context_id, CPXInstance* cpx_inst){
	
	int err = 0, thread_id;
	double objval = CPX_INFBOUND, *xstar;

	COMP* comp;
	TSPSSolution* temp;

	CPXcallbackgetinfoint(context, CPXCALLBACKINFO_THREADID, &thread_id);

	xstar = cpx_inst->xstars[thread_id];

    if((err = CPXcallbackgetcandidatepoint(context, xstar, 0, cpx_inst->ncols - 1, &objval))){
        print_error("CPXcallbackgetcandidatepoint error", err, cpx_inst->env, cpx_inst->lp);
        exit(1);
    }/* if */

	temp = &(cpx_inst->ssols[thread_id]);
    comp = &((*cpx_inst).comps[thread_id]);
	
	build_sol_xstar((*cpx_inst).inst, xstar, temp, comp);
	
	/* if we have one component we do not post a 2opt refined version of it because it is too much time consuming */
    if((*comp).nc > 1){
        add_SEC_candidate(cpx_inst, comp, cpx_inst->env, cpx_inst->lp, context, cpx_inst->ncols);
		postPatchedSSol2CPX(cpx_inst, context, comp, temp);
	}/* if */

    return 0;

}/* checkCandidateSSol */

/*
* Just a static proxy
*/
static int CPXPUBLIC checkCandidateSol(CPXCALLBACKCONTEXTptr context, CPXLONG context_id, void* userhandle){
	return checkCandidateSSol(context, context_id, (CPXInstance*)userhandle);
}/* checkCandidateSol */

/*
* Installs the candidate callback
* IP env CPLEX environment
* IP lp CPLEX linear program
* OP cpx_inst cplex instance
* OR error code
*/
int candidate(CPXENVptr env, CPXLPptr lp, CPXInstance* cpx_inst){
	
	int err;

	CPXLONG context = CPX_CALLBACKCONTEXT_CANDIDATE;

     if((err = CPXcallbacksetfunc(env, lp, context, checkCandidateSol, cpx_inst))){
        print_error("CPXcallbacksetfunc() error", err, env, lp);
		return err;
	}/* if */

	return err;

}/* candidate */
