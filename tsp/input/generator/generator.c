/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : generator.c
*/

#include <stdlib.h>
#include <string.h>

#include "generator.h"
#include "../../tsp.h"

/*
* IP seed for the random generation
* IP reset boolean indicating if the seed needs to be resetted
* OP inst instance to random generate
*/
void generatePointsResetSeed(int seed, bool reset, TSPInstance* inst){
	
	int i;

	if(reset)
		srand(seed);

    for(i = 0; i < (*inst).dimension; i++)
        randomPoint(&((*inst).points[i]));

}/* generatePoints */

/*
* IP seed for the random generation
* OP inst instance to random generate
*/
void generatePoints(int seed, TSPInstance* inst){

	generatePointsResetSeed(seed, true, inst);

}/* generatePoints */

/*
 * IP set settings
 * IP name of the instance
 * IP reset boolean indicating if the seed needs to be resetted
 * OP inst instance to random generate
 * This method assumes the instance to be already allocated
 */
void generateInstanceName(const Settings* set, bool reset, const char name[], TSPInstance* inst){

	strcpy(inst->name, name);

	generatePointsResetSeed((*set).seed, reset, inst);
	
	computeDistances(inst);

}/* generateInstance */

/*
 * IP set settings
 * OP inst instance to random generate
 */
void generateInstance(const Settings* set, TSPInstance* inst){

    allocInst((*set).n, inst);

	generateInstanceName(set, true, "RANDOM", inst);

}/* generateInstance */
