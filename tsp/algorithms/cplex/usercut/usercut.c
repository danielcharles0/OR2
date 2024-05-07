/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : usercut.c
*/

#include <assert.h>

#include "usercut.h"
#include "../cplex.h"
#include "../candidate/candidate.h"
#include <cut.h>

/*
 * Adds user cuts
 */
static int add_cuts(double cutval, int num_nodes, int* members, void* userhandle){

    CCInstance* params = (CCInstance*) userhandle;
    CPXInstance* cpx_inst = params->cpx_inst;
    CPXCALLBACKCONTEXTptr context = params->context;

    double rhs = num_nodes - 1;
    char sense = 'L';
    int purgeable = CPX_USECUT_FILTER;
	int local = 0;
    int err = 0, matbeg = 0;
    int num_edges = num_nodes * (num_nodes - 1) / 2;

    double* vls = malloc(num_edges *  sizeof(double));
    assert(vls != NULL);

    int* edges = malloc(num_edges * sizeof(int));
    assert(edges != NULL);

    int k = 0;
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (members[i] >= members[j])
                continue; // undirected graph. If the node in index i is greated than the node in index j, we skip since (i,j) = (j,i)
            edges[k] = xpos(members[i], members[j], cpx_inst->inst);
            vls[k] = 1.0;
            k++;
        }
    }
    
    if((err = CPXcallbackaddusercuts(context, 1, num_edges, &rhs, &sense, &matbeg, edges, vls, &purgeable, &local))){
        print_error("CPXcallbackaddusercuts() error", err, cpx_inst->env, cpx_inst->lp);
        exit(1);
    }
    
    free(vls);
    
    return 0;

}/* add_cuts */

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
	int err = 0;
	int ncomp = 1;

	double* xstar = (double*) malloc(cpx_inst->ncols * sizeof(double));  
    assert(xstar != NULL);

    /* copied */

    int *elist = malloc(2*cpx_inst->ncols * sizeof(int)); // elist contains each pair of vertex such as (1,2), (1,3), (1,4), (2, 3), (2,4), (3,4) so in list becomes: 1,2,1,3,1,4,2,3,2,4,3,4
    assert(elist != NULL);
    
    int *compscount = NULL; 
    int *comps = NULL;
    int k = 0, num_edges = 0, node = -1;

    CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODECOUNT, &node);
    
    /* do it once every 10 calls */
    if(node % 10 != 0)
        return 0;

    for (int i = 0; i < cpx_inst->inst->dimension; i++) {
        for (int j = i+1; j < cpx_inst->inst->dimension; j++) {
            elist[k++] = i;
            elist[k++] = j;
            num_edges++;
        }
    }
    // Checking whether or not the graph is connected with the fractional solution.
    if((err = CCcut_connect_components(cpx_inst->inst->dimension, num_edges, elist, xstar, &ncomp, &compscount, &comps))){
        print_error("CCcut_connect_components error", err, cpx_inst->env, cpx_inst->lp);
        exit(1);
    }


    if (ncomp == 1) { 
        CCInstance params = {.context = context, .cpx_inst = cpx_inst};
        /* 
        At this point we have a connected graph. This graph could not be a "tsp". We interpret the fractional
        solution as capacity of a cut. A cut of a graph G is composed by S and T = V - S where V is the nodes set.
        The capacity of the cut is the sum of all ingoing and outgoing edges of the cut. Since we have a TSP,
        we want that for each cut, we have a capacity of 2 (i.e. one ingoing edge and one outgoing edge).
        So we want to seek the cuts which don't have a capacity of 2. The cuts with capacity < 2 violates the 
        constraints and we are going to add SEC to them.
        NB: We use cutoff as 1.9 for numerical stability due the fractional values we obtain in the solution. 
        */
        if((err = CCcut_violated_cuts(cpx_inst->inst->dimension, cpx_inst->ncols, elist, xstar, 1.9, add_cuts, &params))){
            print_error("CCcut_violated_cuts error", err, cpx_inst->env, cpx_inst->lp);
            exit(1);
        }
    } else if (ncomp > 1) {

        int startindex = 0;

        int *components = malloc(cpx_inst->inst->dimension * sizeof(int));

        // Transforming the concorde's component format into our component format in order to use our addSEC function
        for (int subtour = 0; subtour < ncomp; subtour++) {

            for (int i = startindex; i < startindex + compscount[subtour]; i++) {
                int index = comps[i];
                components[index] = subtour + 1;
            }

            startindex += compscount[subtour];
            
        }
    }
    /* end copied */
	
	free(elist);
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
int usercutCallback(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, bool warm_start){

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
        update_time_limit(set, start, env);
    }
    
    if((err = CPXcallbacksetfunc(env, lp, CPX_CALLBACKCONTEXT_CANDIDATE, checkCandidateSol, &cpx_inst))){
        print_error("CPXcallbacksetfunc() error: CandidateCallback", err, env, lp);
        return err;
    }

    if((err = CPXcallbacksetfunc(env, lp, CPX_CALLBACKCONTEXT_RELAXATION, checkRelaxedSol, &cpx_inst))){
        print_error("CPXcallbacksetfunc() error: RelaxationCallback", err, env, lp);
        return err;
    }

    optimize_model(inst, env, lp, &temp, &comp);

	convertSSol(inst, &temp, sol);

	freeSSol(&temp);
	freeComp(&comp);

	return 0;

}/* usercutCallback */
