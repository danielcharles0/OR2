/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.c
*/

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
