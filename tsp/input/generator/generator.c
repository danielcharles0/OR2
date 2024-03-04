/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : generator.c
*/

#include <stdlib.h>
#include <string.h>

#include "generator.h"

/*
 * IP set settings
 * OP inst instance to random generate
 */
void generateInstance(const Settings* set, TSPInstance* inst){

	int i;

    initInst((*set).n, inst);

	strcpy(inst->name, "RANDOM");

    srand((*set).seed);

    for(i = 0; i < (*inst).dimension; i++)
        randomPoint(&((*inst).points[i]));

}/* generateInstance */
