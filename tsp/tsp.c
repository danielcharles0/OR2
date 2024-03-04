/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "tsp.h"

/*
* IP inst instance to allocate
*/
void allocInst(TSPInstance* inst){
    inst->points = malloc(inst->dimension * sizeof(Point));
}/* allocInst */

/*
* IP inst instance to free
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
    for(i=0; i<inst->dimension; i++){
        if(i < (POINTS_TO_PRINT/2) || i >= (inst->dimension - POINTS_TO_PRINT/2))
            printf("\t\t%d %lf %lf\n", inst->points[i].id, inst->points[i].x, inst->points[i].y);
        else if ( i == (inst->dimension - POINTS_TO_PRINT/2 - 1))
            printf("\t\t\t...\n");
    }
    printf("\n");

}/* printInst */
