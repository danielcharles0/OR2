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
* IP i sol->succ first node index of the move
* IP j sol->succ second node index of the move
* IP inst tsp instance
* IP sol refined solution
* OR the cost of the move
*/
double delta(int i, int j, const TSPInstance* inst, const TSPSolution* sol){

	int a = (*sol).succ[i], a1 = (*sol).succ[(i + 1) % (*inst).dimension], b = (*sol).succ[j], b1 = (*sol).succ[(j + 1) % (*inst).dimension];

	return getDist(a, b, inst) + getDist(a1, b1, inst) - (getDist(a, a1, inst) + getDist(b, b1, inst));

}/* delta */

/*
* IP inst tsp instance
* IOP sol refined solution
* NB: we have to avoid two cases, the ones when one of the two selected nodes is the successor of the other. In this case when we
* 		break the cycle we are no longer able to reconstruct it.
*/
void opt2(const TSPInstance* inst, TSPSolution* sol){

	int i;
	int j;
	double delta_cost;


}/* opt2 */

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
