/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : usercut.c
*/

#include <assert.h>
#include <string.h>

#include "usercut.h"
#include "../candidate/candidate.h"
#include "../cplex.h"
#include "concorde.h"
#include "../candidate/candidate.h"
#include "../benders/benders.h"
#include "../../refinement/2opt/2opt.h"
#include "../../../utility/utility.h"
#include "../../../output/output.h"

/*
 * Adds user cuts, called by CCcut_violated_cuts
 *
 * IP cutval
 * IP num_nodes number of nodes in the cut.
 * IP members array containing the nodes belonging to this cut.
 * IP userhandle user defined data structure.
 */
static int add_cuts(double cutval, int num_nodes, int* members, void* userhandle){

    CCInstance* params = (CCInstance*) userhandle;
    CPXInstance* cpx_inst = params->cpx_inst;
    CPXCALLBACKCONTEXTptr context = params->context;

    double rhs = num_nodes - 1; /* because ncomp = 1 */
    char sense = 'L';
    int purgeable = CPX_USECUT_FILTER;
	int local = 0;
    int err = 0, matbeg = 0, nnz = 0;
    int num_edges = num_nodes * (num_nodes - 1) / 2;

    double* values = (double*) malloc(num_edges *  sizeof(double));
    assert(values != NULL);

    int* indices = (int*) malloc(num_edges * sizeof(int));
    assert(indices != NULL);

    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (members[i] >= members[j])
                continue; // undirected graph. If the node in index i is greater than the node in index j, we skip since (i,j) = (j,i)

            indices[nnz] = xpos(members[i], members[j], cpx_inst->inst);
            values[nnz] = 1.0;
            nnz++;
        }
    }
    
    if((err = CPXcallbackaddusercuts(context, 1, nnz, &rhs, &sense, &matbeg, indices, values, &purgeable, &local))){
        print_error("CPXcallbackaddusercuts() error", err, cpx_inst->env, cpx_inst->lp);
        exit(1);
    }
    
    free(indices);
    free(values);
    
    return 0;

}/* add_cuts */

/*
* Called by the user.
*
* IP cpx_inst user defines data structure passed by CPLEX.
* IP context context in which the callback is called from.
* IP current_tour current subtour we are analyzing to add constraint of.
* IP comp array that specifies che component each node belongs to.
*/
static int add_SEC_relaxation(CPXInstance* cpx_inst, CPXCALLBACKCONTEXTptr context, int ncomp, int* comp) {
    
    int* indices = (int*) malloc(cpx_inst->ncols * sizeof(int));
    assert(indices != NULL);

    double* values = (double*) malloc(cpx_inst->ncols * sizeof(double));
    assert(values != NULL);

    for(int k = 1; k <= ncomp; k++){
        
        double rhs = -1.0; 
        char sense = 'L';
        int zero = 0; // Contains the index of the beginning column. In this case we add 1 row at a time so no need for an array
        int purgeable = CPX_USECUT_FILTER;
	    int local = 0, err = 0, nnz = 0;

        for(int i = 0; i < cpx_inst->inst->dimension; i++){
            if(comp[i] != k)
                continue;
                
            rhs += 1;

            for(int j = i + 1; j < cpx_inst->inst->dimension; j++){
                if(comp[j] != k)
                    continue;

                indices[nnz] = xpos(i, j, cpx_inst->inst);
                values[nnz] = 1.0;
                nnz++;
            }
        }
        
        if(nnz > 0){

            if((err = CPXcallbackaddusercuts(context, 1, nnz, &rhs, &sense, &zero, indices, values, &purgeable, &local))){ /* add one violated cut */ 
                print_error("CPXcallbackaddusercuts() error", err, cpx_inst->env, cpx_inst->lp);
                return err;
            }

        }

    }

    free(values);
    free(indices);

    return 0;

}/* add_SEC_relaxation */

/*
* Just a static proxy
*/
static int CPXPUBLIC checkCandidateSol(CPXCALLBACKCONTEXTptr context, CPXLONG context_id, CPXInstance* cpx_inst){
	return checkCandidateSSol(context, context_id, cpx_inst);
}/* checkCandidateSol */

/*
* Checks relaxed solution
*
* IP context CPLEX callback context, handled internally by CPLEX
* IP context_id id of the callback context
* IOP userhandle pointer to a structure external to CPLEX
*/
static int CPXPUBLIC checkRelaxedSol(CPXCALLBACKCONTEXTptr context, CPXLONG context_id, CPXInstance* cpx_inst){

	int err = 0;
	int ncomp = 0;

	double* xstar = NULL;
    int* elist = NULL;
    int *compscount = NULL; 
    int *comps = NULL;
    double objval = CPX_INFBOUND;
    int k = 0, num_edges = 0, node = -1;

    CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODECOUNT, &node);

    /* user cuts applied (approximately) once every 10 calls */
    if(node % 10 != 0)
        return 0;

    xstar = (double*) malloc(cpx_inst->ncols * sizeof(double));  
    assert(xstar != NULL);

    elist = (int*) malloc(2*cpx_inst->ncols * sizeof(int)); // elist contains each pair of vertex such as (1,2), (1,3), (1,4), (2, 3), (2,4), (3,4) so in list becomes: 1,2,1,3,1,4,2,3,2,4,3,4
    assert(elist != NULL);

    if((err = CPXcallbackgetrelaxationpoint(context, xstar, 0, cpx_inst->ncols - 1 , &objval))){
        print_error("CPXcallbackgetrelaxationpoint() error", err, cpx_inst->env, cpx_inst->lp);
        exit(1);
    }

    for (int i = 0; i < cpx_inst->inst->dimension; i++)
        for (int j = i+1; j < cpx_inst->inst->dimension; j++) {
            elist[k++] = i;
            elist[k++] = j;
            num_edges++;
        }

    // Checking whether or not the graph is connected with the fractional solution.
    if((err = CCcut_connect_components(cpx_inst->inst->dimension, num_edges, elist, xstar, &ncomp, &compscount, &comps))){
        print_error("CCcut_connect_components() error", err, cpx_inst->env, cpx_inst->lp);
        exit(1);
    }

    if (ncomp == 1) { 

        CCInstance params = {.context = context, .cpx_inst = cpx_inst};
    
        if((err = CCcut_violated_cuts(cpx_inst->inst->dimension, num_edges, elist, xstar, 1.9, add_cuts, &params))){
            print_error("CCcut_violated_cuts() error", err, cpx_inst->env, cpx_inst->lp);
            exit(1);
        }

    } else if (ncomp > 1) {

        int startindex = 0;
        
        int* components = (int*) malloc(cpx_inst->inst->dimension * sizeof(int));
        assert(components != NULL);

        /* Transforming the concorde's component format into our component format in order to use our add_SEC_relaxation function */
        for (int subtour = 0; subtour < ncomp; subtour++) {

            for (int i = startindex; i < startindex + compscount[subtour]; i++) {
                int index = comps[i];
                components[index] = subtour + 1;
            }

            startindex += compscount[subtour];
            
        }

        if((err = add_SEC_relaxation(cpx_inst, context, ncomp, components)))
            return err;

        free(components);
        
    }
    
    free(comps);
    free(compscount);
	free(elist);
	free(xstar);

	return 0;

}/* checkRelaxedSol */

/*
* Dispatcher callback.
*
* IP context CPLEX callback context, handled internally by CPLEX
* IP context_id id of the callback context
* IOP userhandle pointer to a structure external to CPLEX
*/
static int CPXPUBLIC dispatcher(CPXCALLBACKCONTEXTptr context, CPXLONG context_id, void* userhandle){

    if(context_id == CPX_CALLBACKCONTEXT_CANDIDATE)
        return checkCandidateSol(context, context_id, (CPXInstance*)userhandle);
    else if ( context_id == CPX_CALLBACKCONTEXT_RELAXATION)
        return checkRelaxedSol(context, context_id, (CPXInstance*)userhandle);
       
    return 0;

}/* dispatcher */

/*
* Installs the candidate and relaxation callbacks and runs the solver.
*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* IOP sol solution to be updated
* IP warm_start true if MIPSTART is required
* OP et execution time in seconds
* OR error code
*/
int usercut(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, bool warm_start, double* et){

    int err = 0;
    clock_t start = clock();

    COMP comp;
	TSPSSolution temp;
    CPXInstance cpx_inst;

    if(warm_start){
        if((err = mip_start(set, inst, env, lp)))
            return err;
        update_time_limit(set, start, env, lp);
    }/* if */

    allocCPXInstance(&cpx_inst, set, inst, env, lp);

    CPXLONG context = CPX_CALLBACKCONTEXT_CANDIDATE | CPX_CALLBACKCONTEXT_RELAXATION;

    if((err = CPXcallbacksetfunc(env, lp, context, dispatcher, &cpx_inst))){
        print_error("CPXcallbacksetfunc() error: dispatcher", err, env, lp);
		freeCPXInstance(&cpx_inst);
        return err;
    }/* if */

	allocComp(inst->dimension, &comp);
	allocSSol(inst->dimension, &temp);

    optimize_model(inst, env, lp, &temp, &comp);

    convertSSol(inst, &temp, sol);
    
	freeSSol(&temp);
    freeComp(&comp);
	freeCPXInstance(&cpx_inst);

	*et = getSeconds(start);

    return 0;

}/* usercut */
