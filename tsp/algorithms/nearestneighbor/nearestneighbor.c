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
* OP oi the index of the nearest point to $p in $ps
* OR the distance to the nearest point
*/
double nearestPoint(int p, int n, const int* ps, const TSPInstance* inst, int* oi){
	
	int i;
	double mini, mind, temp;
	/* mind := minimum distance so far */
	/* mini := minimum distance point index so far */

	mini = ps[0];
	mind = getDist(p, mini, inst);

	for(i = 1; i < n; i++)
		if((temp = getDist(p, ps[i], inst)) < mind){
			mini = ps[i];
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
		
		int curr = (*sol).succ[i - 1];

		(*sol).val += nearestPoint(curr, (*inst).dimension - i, &((*sol).succ[i]), inst, &((*sol).succ[i]));

	}/* for */

	(*sol).val += getDist((*sol).succ[(*inst).dimension - 2], (*sol).succ[(*inst).dimension - 1], inst);
	(*sol).val += getDist((*sol).succ[(*inst).dimension - 1], (*sol).succ[0], inst);

}/* NN_solverV2 */

/*
* IP set settings
* IP inst tsp instance
* IOP sol solution
* OP false if found a valid solution, true otherwise.
*/
void nearestNeighborV2(const Settings* set, const TSPInstance* inst, TSPSolution* sol){
    
	int i;

	i = readIntRange(1, inst->dimension, "Insert starting node: ") - 1; /* i-th node in position i - 1 */

	NN_solverV2(i, inst, sol);

}/* nearestNeighborV2 */
