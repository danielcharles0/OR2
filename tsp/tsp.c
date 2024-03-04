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

/*
* IP n number of nodes of the instance
* inst instance to initialize
*/
void initInst(int n, TSPInstance* inst){

    (*inst).dimension = n;
    (*inst).points = malloc((*inst).dimension * sizeof(Point2D));
    
	assert((*inst).points != NULL);

}/* initInst */

/*
* IOP inst instance to free memory
*/
void freeInst(TSPInstance* inst){
    free(inst->points);
}/* freeInst */

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
            printf("\t\t%d %lf %lf\n", i + 1, inst->points[i].x, inst->points[i].y);
        else if ( i == (inst->dimension - POINTS_TO_PRINT/2 - 1))
            printf("\t\t\t...\n");
    printf("\n");

}/* printInst */
