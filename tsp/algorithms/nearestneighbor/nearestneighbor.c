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
static void visitNext(int len, const TSPInstance* inst, TSPSolution* sol){
    int i;

    int last = sol->succ[len-1];
    int curr = sol->succ[len];
    int next = len;

    double min_dist = getDist(last, curr);
    double curr_dist;
    
    for(i=len+1; i<inst->dimension; i++){
        
        curr = sol->succ[i]; /* index in inst->distances of next unvisited node */

        curr_dist = getDist(last, curr); /* get distance from last visited node and the current unvisited node in succ */

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

    allocSol(inst->dimension, sol);

    for(i=0; i<inst->dimension; i++)
        sol->succ[i] = i;
    
    while(true){
        i = readInt("Insert starting node: ");

        if(i < 0 || i >= inst->dimension)
            printf("Invalid starting node.\n");
        else   
            break;
    }
    
    swapInt(&(sol->succ[0]), &(sol->succ[i-1])); /* i-th node in position i-1 */

}/* initSol */

/*
* IP inst tsp instance to solve
* IOP sol tsp solution
* OP time at which solution was found
*/
int NN_solver(const TSPInstance* inst, TSPSolution* sol){
    int len=1;
    clock_t start = clock();
    
    while(len < inst->dimension){

        visitNext(len, inst, sol);

        len++;
    }

    sol->val += getDist(sol->succ[0], sol->succ[len-1]); /* add cost of connection of last to first node */

     /* store current time in curr_time */

    return getSeconds(start, clock());

}/* NN_solver */

/*
* Checks whether the solution is valid.
* IP inst tsp instance
* IP sol solution we want to check validity of
* OP false if there is no error, true otherwise.
*/
bool NN_controller(const TSPInstance* inst, const TSPSolution* sol){
    int i;
    bool valid;

    double cost = 0.0;
    int* counters = malloc(inst->dimension * sizeof(int));

    assert(counters != NULL);

    for(i=1; i<inst->dimension; i++){
        
        ++counters[sol->succ[i] - 1];

        cost += inst->dist[sol->succ[i-1] - 1][sol->succ[i] - 1];
    }
    
    cost += inst->dist[sol->succ[i-1] - 1][sol->succ[0] - 1]; /* add cost of connection of last to first node */

    valid = isEqualPrecision(sol->val, cost, EPSILON);

    for(i=0; i<inst->dimension; i++){
        if(counters[i] != 1)
            valid = false;
    }

    free(counters);

    return !valid;

}/* NN_controller */

/*
* IP set settings
* IP inst tsp instance
* IOP sol solution
* OP false if found a valid solution, true otherwise.
*/
bool nearestNeighbor(const Settings* set, const TSPInstance* inst, TSPSolution* sol){
    int time;
    bool error;

    NN_initSol(inst, sol);

    time = NN_solver(inst, sol);

    return NN_controller(inst, sol);

}/* nearestNeighbor */
