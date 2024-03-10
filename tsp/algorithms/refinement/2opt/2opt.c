/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : 2opt.c
*/

#include <stdio.h>
#include "2opt.h"
#include "../../../utility/utility.h"

/*
* IP alg refinement algorithm to run
* IP inst tsp instance
* IOP sol refined solution
* OP error true if an error occurred, false otherwise.
*/
bool runRefAlg(REFINEMENT_ALGORITHM alg, const TSPInstance* inst, TSPSolution* sol){
    
    switch (alg){
	    case OPT2:
	        /*randomSol(inst, sol);*/
	        break;
	    default:
	        printf("Error: Algorithm code not found.\n\n");
			return true;
    }/* switch */

	return false;

}/* run */

/*
* Print refinement algorithm legend.
*/
void refinementaAlgorithmLegend(void){

	printf("Available refinement algorithms:\n");
    printf("Code: %d, Algorithm: Just a random solution\n", OPT2);
    printf("\n");

}/* refinementaAlgorithmLegend */

/*
* OR the selected algorithm
*/
REFINEMENT_ALGORITHM getRefAlg(void){
	
	refinementaAlgorithmLegend();

    return readInt("Insert the code of the refinement algorithm you want to run: ");

}/* getRefinementAlg */

/*
* IP inst tsp instance
* IOP sol solution to optimize
*/
void runRefinement(const TSPInstance* inst, TSPSolution* sol){

    runRefAlg(getRefAlg(), inst, sol);

}/* runRefinement */
