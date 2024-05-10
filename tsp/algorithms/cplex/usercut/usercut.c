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
#include <concorde.h>

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
    int err = 0, matbeg = 0;
    int num_edges = num_nodes * (num_nodes - 1) / 2;

    double* values = malloc(num_edges *  sizeof(double));
    assert(values != NULL);

    int* indices = malloc(num_edges * sizeof(int));
    assert(indices != NULL);

    int k = 0;
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (members[i] >= members[j])
                continue; // undirected graph. If the node in index i is greated than the node in index j, we skip since (i,j) = (j,i)
            indices[k] = xpos(members[i], members[j], cpx_inst->inst);
            values[k++] = 1.0;
        }
    }
    
    if((err = CPXcallbackaddusercuts(context, 1, k, &rhs, &sense, &matbeg, indices, values, &purgeable, &local))){
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
* IP indices
* IP values
*/
static int add_SEC_relaxation(CPXInstance* cpx_inst, CPXCALLBACKCONTEXTptr context, int ncomp, int* comp, int* indices, double* values) {
    
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

    return 0;

}/* add_SEC_relaxation */

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
        print_error("CPXcallbackgetcandidatepoint() error", err, cpx_inst->env, cpx_inst->lp);
		exit(1);
	}

	build_comp(cpx_inst, xstar, &comp);

	if(comp.nc > 1)
    	add_SEC_candidate(cpx_inst->inst, &comp, cpx_inst->env, cpx_inst->lp, context, cpx_inst->ncols);
	else{

		/* run 2opt or patching and then post solution */

	}
    
    free(xstar);
	freeComp(&comp);

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

	double* xstar = NULL;
    int* elist = NULL;
    int *compscount = NULL; 
    int *comps = NULL;
    double objval = CPX_INFBOUND;
    int k = 0, num_edges = 0, node = -1;

    CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODECOUNT, &node);
    
    /* user cuts applied once every 10 calls */
    if(node % 10 != 0)
        return 0;

    xstar = (double*) malloc(cpx_inst->ncols * sizeof(double));  
    assert(xstar != NULL);

    elist = malloc(2*cpx_inst->ncols * sizeof(int)); // elist contains each pair of vertex such as (1,2), (1,3), (1,4), (2, 3), (2,4), (3,4) so in list becomes: 1,2,1,3,1,4,2,3,2,4,3,4
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

        printf("NCOMP = 1\n");

        CCInstance params = {.context = context, .cpx_inst = cpx_inst};
    
        if((err = CCcut_violated_cuts(cpx_inst->inst->dimension, num_edges, elist, xstar, 1.9, add_cuts, &params))){
            print_error("CCcut_violated_cuts() error", err, cpx_inst->env, cpx_inst->lp);
            exit(1);
        }

    } else if (ncomp > 1) {

        printf("NCOMP > 1\n");

        int startindex = 0;
        
        int* components = malloc(cpx_inst->inst->dimension * sizeof(int));
        assert(components != NULL);

        int* indices = (int*) malloc(cpx_inst->ncols * sizeof(int));
        assert(indices != NULL);

        double* values = (double*) malloc(cpx_inst->ncols * sizeof(double));
        assert(values != NULL);

        /* Transforming the concorde's component format into our component format in order to use our add_SEC_relaxation function */
        for (int subtour = 0; subtour < ncomp; subtour++) {

            for (int i = startindex; i < startindex + compscount[subtour]; i++) {
                int index = comps[i];
                components[index] = subtour + 1;
            }

            startindex += compscount[subtour];
            
        }

        if((err = add_SEC_relaxation(cpx_inst, context, ncomp, components, indices, values)))
            return err;

        /*
        for (int subtour = 1; subtour <= ncomp; subtour++)
            // For each subtour we add the constraints in one shot
            if((err = add_SEC_relaxation(cpx_inst, context, subtour, components, indices, values)))
                return err;
        */

        free(values);
        free(indices);
        free(components);
        
    }
    
	free(elist);
	free(xstar);

	return 0;

}/* checkRelaxedSol */

/*
* Installs the candidate and relaxation callbacks and runs the solver.
*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* IOP sol solution to be updated
* OR error code
*/
int usercut(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, bool warm_start){

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
    
    if((err = CPXcallbacksetfunc(env, lp, CPX_CALLBACKCONTEXT_CANDIDATE, checkCandidateSol, &cpx_inst))){
        print_error("CPXcallbacksetfunc() error: CandidateCallback", err, env, lp);
        return err;
    }

    if((err = CPXcallbacksetfunc(env, lp, CPX_CALLBACKCONTEXT_RELAXATION, checkRelaxedSol, &cpx_inst))){
        print_error("CPXcallbacksetfunc() error: RelaxationCallback", err, env, lp);
        return err;
    }

    optimize_model(inst, env, lp, &temp, &comp);

    /* START: TEST PURPOSE*/
    /*
    double cost = 0;
    for(int i=0; i<inst->dimension; i++){
        cost += getDist(i, temp.succ[i], inst);
        printf("%d\n", temp.succ[i]);
    }

    printf("\nCHECK COST BEFORE CONVERSION: %lf\n", cost);

    int ncols = CPXgetnumcols(env,lp);
    double* xstar = (double*) malloc(ncols * sizeof(double));

    int counter = 0;
    for(int i = 0; i<inst->dimension; i++){
        for(int j = i+1; j<inst->dimension; j++)
            if(xstar[counter++] == 1)
                printf("%d: %d\n", i, j);
    }
    */
    /* END: TEST PURPOSE*/

	convertSSol(inst, &temp, sol);

	freeSSol(&temp);
	freeComp(&comp);

	return 0;

}/* usercut */
