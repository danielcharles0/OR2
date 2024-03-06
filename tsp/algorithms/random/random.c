/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : random.c
*/

#include "random.h"
#include "../../utility/utility.h"

void randomSol(const TSPInstance* inst, TSPSolution* sol){
	
	int i;

	ascendentSol(inst, sol);

	for(i = 0; i < (*inst).dimension - 1; i++){
		
		int nn = i + rand0N((*inst).dimension - i);

		swapInt(&((*sol).succ[i]), &((*sol).succ[nn]));

	}/* for */

	(*sol).val = getSolCost(inst, sol);

}/* random */
