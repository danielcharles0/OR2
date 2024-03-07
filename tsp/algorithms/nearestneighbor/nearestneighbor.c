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

/*
* Finds index of next node to visit (nearest neighbor), puts next node to visit close to last visited and updates cost of the solution.
* IP len current index we have to update in $sol->succ
* IP inst tsp instance
* IOP sol solution we want to update
*/
void visitNext(int len, const TSPInstance* inst, TSPSolution* sol){
    int i;

    int last = sol->succ[len-1];
    int curr = sol->succ[len];
    int next = len;

    double min_dist = getDist(last, curr, inst);
    double curr_dist;
    
    for(i=len+1; i<inst->dimension; i++){
        
        curr = sol->succ[i]; /* index in inst->points of next unvisited node */

        curr_dist = getDist(last, curr, inst); /* get distance between last visited node and the current unvisited node in succ */

        if(curr_dist < min_dist){
            min_dist = curr_dist;
            next = i;
        }
    }

    swapInt(&(sol->succ[len]), &(sol->succ[next]));
    sol->val += min_dist;

}/* minDistNode */

/*
* IP inst tsp instance
* IOP sol tsp solution allocated and initialized with starting node in first position
*/
void NN_initSol(const TSPInstance* inst, TSPSolution* sol){

	int i;

    ascendentSol(inst, sol);

    sol->val = 0;
    
	i = readIntRange(1, inst->dimension, "Insert starting node") - 1; 

    swapInt(&(sol->succ[0]), &(sol->succ[i])); 

}/* initSol */

/*
* IP inst tsp instance to solve
* IOP sol tsp solution
* OR time at which solution was found
*/
int NN_solver(const TSPInstance* inst, TSPSolution* sol){
    
	int len=1;
    clock_t start = clock();
    
    while(len < inst->dimension){

        visitNext(len, inst, sol);

        len++;
    }

    sol->val += getDist(sol->succ[0], sol->succ[len-1], inst); /* add cost of connection of last to first node */

    return getSeconds(start, clock());

}/* NN_solver */

/*
* IP set settings
* IP inst tsp instance
* IOP sol solution
* OR false if found a valid solution, true otherwise.
*/
void nearestNeighbor(const Settings* set, const TSPInstance* inst, TSPSolution* sol){
    
	int time;

    NN_initSol(inst, sol);

    time = NN_solver(inst, sol);

}/* nearestNeighbor */

/*
* IP sn starting node index
* IP inst tsp instance
* OP sol tsp solution initialized with starting node in first position
*/
void initSolV2(int sn, const TSPInstance* inst, TSPSolution* sol){

    ascendentSol(inst, sol);

    swapInt((*sol).succ, &((*sol).succ[sn])); 

}/* initSolV2 */

/*
* IP p point
* IP n number of points, n > 0
* IP ps set of $n points
* OP oi the index (in the array, not the poind id) of the nearest point to $p in $ps
* OR the distance to the nearest point
*/
double nearestPoint(int p, int n, const int* ps, const TSPInstance* inst, int* oi){
	
	int i;
	double mini = 0, mind, temp;
	/* mind := minimum distance so far */
	/* mini := minimum distance point index so far */

	mind = getDist(p, mini, inst);

	for(i = 1; i < n; i++)
		if((temp = getDist(p, ps[i], inst)) < mind){
			mini = i;
			mind = temp;
		}/* if */

	*oi = mini;
	return mind;

}/* nearestPoint */

/*
* IP sn starting node index
* IP inst tsp instance to solve
* IOP sol tsp solution
*/
void NN_solverV2(int sn, const TSPInstance* inst, TSPSolution* sol){
	
	int i;

	initSolV2(sn, inst, sol);

	(*sol).val = 0;

	for(i = 1; i < (*inst).dimension - 1; i++){
		
		int idx, curr = (*sol).succ[i - 1], *ps = &((*sol).succ[i]);

		(*sol).val += nearestPoint(curr, (*inst).dimension - i, ps, inst, &idx);

		swapInt(ps, &(ps[idx]));

	}/* for */

	(*sol).val += getDist((*sol).succ[(*inst).dimension - 2], (*sol).succ[(*inst).dimension - 1], inst);
	(*sol).val += getDist((*sol).succ[(*inst).dimension - 1], (*sol).succ[0], inst);

}/* NN_solverV2 */

/*
* Print algorithm configurations.
*/
void algorithmConfigurations(){
    
    printf("Available nearest neighbor search configurations:\n");
    printf("Code: %d, Algorithm: Search starts from the first node\n", START_FIRST_NODE);
    printf("Code: %d, Algorithm: Search starts from a random node\n", START_RANDOM_NODE);
	printf("Code: %d, Algorithm: Search starts from a node you select\n", SELECT_STARTING_NODE);
	printf("Code: %d, Algorithm: Search will be performed starting from any node, the best solution is returned\n", BEST_START);
    printf("\n");
    
}/* algorithmLegend */

/*
* IP conf configuration code
*/
void runConfiguration(NN_CONFIG conf, const TSPInstance* inst, TSPSolution* sol){

	switch (conf){
	    case START_FIRST_NODE:
	        NN_solverV2(0, inst, sol);
	        break;
	    case START_RANDOM_NODE:
	        NN_solverV2(rand0N((*inst).dimension), inst, sol);
	        break;
		case SELECT_STARTING_NODE:
						/* i-th node in position i - 1 */
			NN_solverV2(readIntRange(1, inst->dimension, "Insert starting node: ") - 1, inst, sol);
	        break;
		case BEST_START:
			/* TO IMPLEMENT */
			break;
	    default:
	        printf("Error: Algorithm code not found.\n\n");
	        break;
    }

}/* runConfiguration */

/*
* IP set settings
* IP inst tsp instance
* IOP sol solution
* OP false if found a valid solution, true otherwise.
*/
void nearestNeighborV2(const Settings* set, const TSPInstance* inst, TSPSolution* sol){
	
	int conf;

	algorithmConfigurations();

	conf = readInt("Insert the configuration code you want to run: ");

	runConfiguration(conf, inst, sol);

}/* nearestNeighborV2 */
