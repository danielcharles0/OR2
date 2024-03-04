/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : generator.c
*/

#include <stdlib.h>
#include <string.h>

#include "generator.h"

void generateInstance(Settings* set, TSPInstance* inst){
    int i;

    strcpy(inst->name, "RANDOM");
    inst->dimension = set->n;
    allocInst(inst);

    srand(set->seed);

    for(i=0; i<inst->dimension; i++)
        randomPoint(i+1, &(inst->points[i]));

}
