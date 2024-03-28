/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : cplex.c
*/

#include <stdio.h>
#include <ilcplex/cplex.h>  

#include "cplex.h"  

int cplex(TSPInstance* inst, TSPSolution* sol){  

	int error, ncols, neigh, curr = 0;
    double* xstar = NULL;

	CPXENVptr env = CPXopenCPLEX(&error);
	CPXLPptr lp = CPXcreateprob(env, &error, NULL); 

	build_model(inst, env, lp);

	if(CPXmipopt(env,lp)) 
        print_error("CPXmipopt() error");    
	
	ncols = CPXgetnumcols(env, lp);
	xstar = (double *) calloc(ncols, sizeof(double));

	if(CPXgetx(env, lp, xstar, 0, ncols-1)) 
        print_error("CPXgetx() error");	


    createPath(inst, xstar, sol);
	
	free(xstar);
	 
	CPXfreeprob(env, &lp);
	CPXcloseCPLEX(&env); 

	return 0;

}
	
/*
* IP set settings
* IP inst tsp instance
* IP env cplex environment variable
* IP lp cplex linear programming variable
*/
void build_model(const Settings* set, TSPInstance* inst, CPXENVptr env, CPXLPptr lp){    

	int izero = 0;
	char binary = 'B'; 
	
	char** cname = (char **) calloc(1, sizeof(char*));
	cname[0] = (char *) calloc(100, sizeof(char));

	for ( int i = 0; i < inst->dimension; i++ ){

		for ( int j = i+1; j < inst->dimension; j++ ){

			sprintf(cname[0], "x(%d,%d)", i+1,j+1);  
			double obj = getDist(i, j, inst);
			double lb = 0.0;
			double ub = 1.0;

			if (CPXnewcols(env, lp, 1, &obj, &lb, &ub, &binary, cname)) 
                print_error(" wrong CPXnewcols on x var.s");

    		if (CPXgetnumcols(env,lp)-1 != xpos(i,j, inst)) 
                print_error(" wrong position for x var.s");

		}

	} 

	/* add degree constr.s */ 

	int *index = (int*) malloc(inst->dimension * sizeof(int));
	double *value = (double*) malloc(inst->dimension * sizeof(double));  
	
	for(int h = 0; h < inst->dimension; h++){

		double rhs = 2.0;
		char sense = 'E';   /* equality constraint */                     
		sprintf(cname[0], "degree(%d)", h+1); 
		int nnz = 0;
		
        for(int i = 0; i < inst->dimension; i++){
			if(i == h) 
                continue;
		
        	index[nnz] = xpos(i,h, inst);
			value[nnz] = 1.0;
			nnz++;
		}
		
		if(CPXaddrows(env, lp, 0, 1, nnz, &rhs, &sense, &izero, index, value, NULL, &cname[0])) 
            print_error(" wrong CPXaddrows [degree]");
	} 

    free(value);
    free(index);	

	if (set->v) 
        CPXwriteprob(env, lp, "model.lp", NULL);   

	free(cname[0]);
	free(cname);

}

#define DEBUG    // da commentare se non si vuole il debugging
#define EPS 1e-5

void build_sol(const double *xstar, TSPInstance* inst, int* succ, int* comp, int* ncomp){

	*ncomp = 0;
	for(int i = 0; i < inst->dimension; i++){

		succ[i] = -1;
		comp[i] = -1;

	}
	
	for(int start = 0; start < inst->dimension; start++){
		
        if ( comp[start] >= 0 ) 
            continue;  /* node "start" was already visited, just skip it */

		(*ncomp)++;
		int i = start;
		int done = 0;
		while(!done){
			
            comp[i] = *ncomp;
			done = 1;
			
            for(int j = 0; j < inst->dimension; j++){
				
                if(i != j && xstar[xpos(i,j,inst)] > 0.5 && comp[j] == -1 ){
					
                    succ[i] = j;
					i = j;
					done = 0;
					break;
				}
			}
		}	
		succ[i] = start;  // last arc to close the cycle
		
		// go to the next component...
	}

	int izero = 0;
	int index[3]; 
	double value[3];

	// add lazy constraints  1.0 * u_i - 1.0 * u_j + M * x_ij <= M - 1, for each arc (i,j) not touching node 0	
	double big_M = inst->dimension - 1.0;
	double rhs = big_M -1.0;
	char sense = 'L';
	int nnz = 3;
	for ( int i = 1; i < inst->dimension; i++ ) // excluding node 0
	{
		for ( int j = 1; j < inst->dimension; j++ ) // excluding node 0 
		{
			if ( i == j ) 
                continue;

			sprintf(cname[0], "u-consistency for arc (%d,%d)", i+1, j+1);
			index[0] = upos(i,inst);	
			value[0] = 1.0;	
			index[1] = upos(j,inst);
			value[1] = -1.0;
			index[2] = xpos_compact(i,j,inst);
			value[2] = big_M;
			if ( CPXaddlazyconstraints(env, lp, 1, nnz, &rhs, &sense, &izero, index, value, cname) ) print_error("wrong CPXlazyconstraints() for u-consistency");
		}
	}
	
	// add lazy constraints 1.0 * x_ij + 1.0 * x_ji <= 1, for each arc (i,j) with i < j
	rhs = 1.0; 
	char sense = 'L';
	nnz = 2;
	for ( int i = 0; i < inst->dimension; i++ ) 
	{
		for ( int j = i+1; j < inst->dimension; j++ ) 
		{
			sprintf(cname[0], "SEC on node pair (%d,%d)", i+1, j+1);
			index[0] = xpos_compact(i,j,inst);
			value[0] = 1.0;
			index[1] = xpos_compact(j,i,inst);
			value[1] = 1.0;
			if ( CPXaddlazyconstraints(env, lp, 1, nnz, &rhs, &sense, &izero, index, value, cname) ) print_error("wrong CPXlazyconstraints on 2-node SECs");
		}
	}
}