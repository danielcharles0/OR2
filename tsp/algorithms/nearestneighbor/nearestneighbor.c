/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : nearestneighbor.c
*/

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "nearestneighbor.h"
#include "../../utility/utility.h"
#include "../refinement/2opt/2opt.h"

#define TIMEOUT_WARNING_MESSAGE "Warning: The method exceeded the time limit! the solution that will be returned is the best founded so far\n\n"

/*
* Finds index of next node to visit (nearest neighbor), puts next node to visit close to last visited and updates cost of the solution.
* IP len current index we have to update in $sol->path
* IP inst tsp instance
* IOP sol solution we want to update
*/
void visitNext(int len, const TSPInstance* inst, TSPSolution* sol){
    
	int i;

    int last = sol->path[len-1];
    int curr = sol->path[len];
    int next = len;

    double min_dist = getDist(last, curr, inst);
    double curr_dist;
    
    for(i=len+1; i<inst->dimension; i++){
        
        curr = sol->path[i]; /* index in inst->points of next unvisited node */

        curr_dist = getDist(last, curr, inst); /* get distance between last visited node and the current unvisited node in path */

        if(curr_dist < min_dist){
            min_dist = curr_dist;
            next = i;
        }
    }

    swapInt(&(sol->path[len]), &(sol->path[next]));
    sol->val += min_dist;

}/* minDistNode */

/*
* IP sp starting point index
* IP inst tsp instance
* IOP sol tsp solution allocated and initialized with starting node in first position
*/
void NN_initSol(int sp, const TSPInstance* inst, TSPSolution* sol){

    ascendentSol(inst, sol);

    sol->val = 0;

    swapInt(&(sol->path[0]), &(sol->path[sp])); 

}/* initSol */

/*
* IP sp starting point index
* IP inst tsp instance to solve
* IOP sol tsp solution
* OR time at which solution was found
*/
int NN_solver(int sp, const TSPInstance* inst, TSPSolution* sol){
    
	clock_t start = clock();
	int curr;

    NN_initSol(sp, inst, sol);
    
   for(curr=1; curr<inst->dimension; curr++)
        visitNext(curr, inst, sol);

    sol->val += getDist(sol->path[0], sol->path[curr-1], inst); /* add cost of connection of last to first node */

    return getSeconds(start);

}/* NN_solver */

/*
* Print algorithm configurations.
*/
void algorithmConfigurations(void){
    
    printf("Available nearest neighbor search configurations:\n");
    printf("\t- Code: %d, Algorithm: Search starts from the first node\n", START_FIRST_NODE);
    printf("\t- Code: %d, Algorithm: Search starts from a random node\n", START_RANDOM_NODE);
	printf("\t- Code: %d, Algorithm: Search starts from a node you select\n", SELECT_STARTING_NODE);
	printf("\t- Code: %d, Algorithm: Search will be performed starting from any node, the best solution is returned\n", BEST_START);
    printf("\n");
    
}/* algorithmLegend */

/*
* IP start time
* IP it current iteration
* IP n total number of iterations
* OV process bar
*/
void NNBar(clock_t start, int it, int n, int* ls){
	
	int s = getSeconds(start);

	if(s - *ls >= PRINT_FREQUENCY || it == n){
		processBar(it, n);
		printSeconds("Running time: ", s);
		*ls = s;
	}/* if */

}/* NNBar */

/*
* IP set settings
* IP inst tsp instance
* OP sol solution, the best founded within the time limit
* OR int execution seconds
*/
double best_start(const Settings* set, const TSPInstance* inst, TSPSolution* sol){
	
	clock_t start = clock();
	TSPSolution temp;
	int i, ls = -1;

	allocSol((*inst).dimension, &temp);

	NN_solver(0, inst, sol);

	if((*set).v)
		NNBar(start, 1, (*inst).dimension, &ls);

	for(i = 1; i < (*inst).dimension; i++){

		if(isTimeOutWarning(TIMEOUT_WARNING_MESSAGE, start, (*set).tl, (*set).v))
			break;

		NN_solver(i, inst, &temp);

		if(temp.val < (*sol).val)
			cpSol(inst, &temp, sol);

		if((*set).v)
			NNBar(start, i + 1, (*inst).dimension, &ls);
			
	}/* for */

	if((*set).v)
		printf("\n\n");

	freeSol(&temp);
	
	return getSeconds(start);

}/* best_start */

/*
* IP conf configuration code
* OR int execution seconds, -1 if error
*/
int runConfiguration(NN_CONFIG conf, const Settings* set, const TSPInstance* inst, TSPSolution* sol){

	switch (conf){
	    case START_FIRST_NODE:
	        return NN_solver(0, inst, sol);
	        break;
	    case START_RANDOM_NODE:
	        return NN_solver(rand0N((*inst).dimension), inst, sol);
	        break;
		case SELECT_STARTING_NODE:
						/* i-th node in position i - 1 */
			return NN_solver(readIntRange(1, inst->dimension, "Insert starting node") - 1, inst, sol);
	        break;
		case BEST_START:
			return best_start(set, inst, sol);
			break;
	    default:
	        printf("Error: Algorithm code not found.\n\n");
	        break;
    }
	
	return -1;

}/* runConfiguration */

/*
* IP conf configuration code
* OR int execution seconds, -1 if error
* Note: It is just a new name for runConfiguration to make it available in the .h
*/
int NNRunConfiguration(NN_CONFIG conf, const Settings* set, const TSPInstance* inst, TSPSolution* sol){
	return runConfiguration(conf, set, inst, sol);
}/* NNRunConfiguration */

/*
* IP set settings
* IP inst tsp instance
* IOP sol solution
* OP false if found a valid solution, true otherwise.
* OR int execution seconds, -1 if error
*/
int nearestNeighbor(const Settings* set, const TSPInstance* inst, TSPSolution* sol){

	algorithmConfigurations();

	return runConfiguration(readInt("Insert the configuration code you want to run: "), set, inst, sol);

}/* nearestNeighbor */
