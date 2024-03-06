/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : nearestneighbor.c
*/

#include <stdio.h> 
#include <time.h>

#include "nearestneighbor.h"
#include "../../utility/utility.h"

/*
* Returns index of next node to visit (nearest neighbor).
*
*/
static int minDistNode(int len, const TSPSolution* sol, const TSPInstance* inst){
    int i, curr;

    int last = sol->succ[len-1] - 1;
    int next = len;

    double min_dist = inst->dist[last][next];
    double curr_dist;
    
    for(i=len+1; i<inst->dimension; i++){
        
        curr = sol->succ[i] - 1; /* index in inst->distances of next unvisited node */

        curr_dist = inst->dist[last][curr]; /* get distance from last visited node and the current unvisited node in succ */

        if(curr_dist < min_dist){
            min_dist = curr_dist;
            next = i;
        }
    }

    return next;

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
        i = read_int("Insert starting node: ");

        if(i < 0 || i >= inst->dimension)
            printf("Invalid starting node.\n");
        else   
            break;
    }
    
    swap_int(&(sol->succ[0]), &(sol->succ[i-1])); /* i-th node in position i-1 */

}/* initSol */

/*
* IP inst tsp instance to solve
* IOP sol tsp solution
* OP time at which solution was found
*/
int NN_solver(const TSPInstance* inst, TSPSolution* sol){
    int len=1;
    int next;
    time_t curr_time;
    
    while(len < inst->dimension){

        next = minDistNode(len, sol, inst);

        swap_int(&(sol->succ[len]), &(sol->succ[next]));

        len++;
    }

    time(&curr_time); /* store current time in curr_time */

    return (int)curr_time;

}/* NN_solver */

/*
* 
*/
void NN_controller(void){

}/* NN_controller */
