/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tsp.h"
#include "./utility/utility.h"
#include "algorithms/refinement/2opt/2opt.h"
#include "algorithms/nearestneighbor/nearestneighbor.h"
#include "algorithms/random/random.h"
#include "utility/utility.h"

/*
* IP x
* OR $x rounded to the nearest integer
* Reference:
*	- http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/
*	- Page 6: http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp95.pdf
*/
int nint(double x){

    return (int)(x + .5);

}/* nint */

/*
* IP inst instance
* IP sol solution
* OR cost of the solution $sol
*/
double getSolCost(const TSPInstance* inst, const TSPSolution* sol){
	
	int i;
	double cost = 0;

	for(i = 0; i < inst->dimension; i++){
		int s = (*sol).succ[i], t = (*sol).succ[(i + 1) % inst->dimension];
        cost += getDist(s, t, inst);
	}/* for */

	return cost;

}/* getSolCost */

/*
* IP inst instance
* OP sol solution to compute
*/
void ascendentSol(const TSPInstance* inst, TSPSolution* sol){
	
	int i;

	for(i = 0; i < inst->dimension; i++)
        (*sol).succ[i] = i;

	(*sol).val = getSolCost(inst, sol);

}/* ascendentSol */

/*
* IOP inst instance to initialize $inst->dist
*/
void initDist(TSPInstance* inst){

    int i;

    (*inst).dist = malloc(((*inst).dimension - 1) * sizeof(double*));
    assert((*inst).dist != NULL);

    for (i = 0; i < (*inst).dimension - 1; i++){

        (*inst).dist[i] = malloc((i + 1) * sizeof(double));
        assert((*inst).dist[i] != NULL);

    }/* for */

}/* initDist */

/*
* IOP inst, compute and store into $inst->dist all the distances
*/
void computeDistances(TSPInstance* inst){
	
	int i, j;

	for(i = 1; i < (*inst).dimension; i++)
		for(j = 0; j < i; j++)
	        (*inst).dist[i - 1][j] = distance(&((*inst).points[j]), &((*inst).points[i]));

}/* computeDistances */

/*
* IP n number of nodes of the instance
* IP inst instance to initialize
*/
void allocInst(int n, TSPInstance* inst){

    (*inst).dimension = n;
    (*inst).points = malloc((*inst).dimension * sizeof(Point2D));
    
	assert((*inst).points != NULL);

	initDist(inst);

}/* allocInst */

/*
* IOP inst instance to free $inst->dist
*/
void freeDist(TSPInstance* inst){

    int i;

    if((*inst).dimension > 1){

        for (i = 0; i < (*inst).dimension - 1; i++)
            free((*inst).dist[i]);

        free((*inst).dist);

    }/* if */

}/* freeDist */

/*
* IOP inst instance to free memory
*/
void freeInst(TSPInstance* inst){
    free(inst->points);
	freeDist(inst);
}/* freeInst */

/*
* IP n number of nodes of the instance
* IP sol solution to initialize
*/
void allocSol(int n, TSPSolution* sol){

    sol->succ = malloc(n * sizeof(int));
	assert(sol->succ != NULL);

}/* allocSol */

/*
* IOP sol solution to free memory
*/
void freeSol(TSPSolution* sol){
    free(sol->succ);
}/* freeSol */

/*
* IP inst instance to print
*/
void printInst(const TSPInstance* inst){
	
	int i;

    printf("Instance:\n");
    printf("\tName: %s\n", inst->name);
    printf("\tDimension: %d\n", inst->dimension);
    printf("\tNodes:\n");
    for(i = 0; i < inst->dimension; i++)
        if(i < (POINTS_TO_PRINT/2) || i >= (inst->dimension - POINTS_TO_PRINT/2))
            printf("\t\t%.3d %lf %lf\n", i + 1, inst->points[i].x, inst->points[i].y);
        else if ( i == (inst->dimension - POINTS_TO_PRINT/2 - 1))
            printf("\t\t\t...\n");
    printf("\n");

}/* printInst */

/*
* Print algorithm legend.
*/
void algorithmLegend(void){
    
    printf("Available algorithms:\n");
    printf("Code: %d, Algorithm: Just a random solution\n", RANDOM);
    printf("Code: %d, Algorithm: Nearest neighbor search\n", NEAREST_NEIGHBOR);
    printf("\n");
    
}/* algorithmLegend */

/*
* IP alg algorithm
* OR true if it is an exact method, false otherwise
*/
bool isExactMethod(ALGORITHM alg){
	return alg > __END_HEURISTIC;
}/* isExactMethod */

/*
* IP alg algorithm to run
* IP inst tsp instance
* IOP sol solution
* IP set settings
* OP error true if an error occurred, false otherwise.
*/
bool run(ALGORITHM alg, const TSPInstance* inst, TSPSolution* sol, const Settings* set){
    
	bool error = false;
    
    switch (alg){
	    case RANDOM:
	        randomSol(inst, sol);
	        break;
	    case NEAREST_NEIGHBOR:
	        nearestNeighbor(set, inst, sol);
	        break;
	    default:
	        printf("Error: Algorithm code not found.\n\n");
	        return true;
    }/* switch */

	if(!isExactMethod(alg))
		runRefinement(set, inst, sol);

    return error;

}/* run */

/*
* IP first node index
* IP second node index
* IP inst tsp instance
* OR distance between nodes i and j
*/
double getDist(int i, int j, const TSPInstance* inst){

	if(i == j)
		return 0;
	
	return (i < j) ? (*inst).dist[j - 1][i] : (*inst).dist[i - 1][j];

}/* getDist */

/*
* Checks whether the solution is valid.
* IP inst tsp instance
* IP sol solution we want to check validity of
* OP valid true if valid solution, false otherwise.
*/
bool checkSol(const TSPInstance* inst, const TSPSolution* sol){

    if(!isDistinct(inst->dimension, sol->succ))
        return false;

    return isEqual(sol->val, getSolCost(inst, sol));    

}/* checkSol */

/*
* IP inst tsp instance
* IP source solution we want to copy
* OP destination solution that will be a copy of $source
*/
void cpSol(const TSPInstance* inst, const TSPSolution* source, TSPSolution* destination){
	
	int i;

	for(i = 0; i < (*inst).dimension; i++)
		(*destination).succ[i] = (*source).succ[i];
	
	(*destination).val = (*source).val;

}/* cpSol */

/*
* Check that each element of the array appears once.
* IP n dimension of the array
* IP arr array
* OP distinct true if each element appears once, false otherwise.
*/
bool isDistinct(int n, int* arr){
    
	int i;

    int* counters = malloc(n * sizeof(int));
    assert(counters != NULL);

	for(i = 0; i < n; i++)
		counters[i] = 0;
	
	for(i = 0; i < n; i++)
		if(arr[i] < 0 || arr[i] >= n || ++counters[arr[i]] > 1){
			free(counters);
			return false;
		}/* if */

    free(counters);

    return true;

}/* isDistinct */
