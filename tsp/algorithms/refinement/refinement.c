/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : refinement.c
*/

#include <stdio.h>

#include "../../utility/utility.h"
#include "refinement.h"
#include "2opt/2opt.h"
#include "tabu/tabu.h"
#include "vns/vns.h"

/*
* IP alg refinement algorithm to run
* IP set settings
* IP inst tsp instance
* IOP sol refined solution
* OP error true if an error occurred, false otherwise.
*/
bool runRefAlg(REFINEMENT_ALGORITHM alg, const Settings* set, const TSPInstance* inst, TSPSolution* sol){
    
    switch (alg){
		case SKIP:
			break;
	    case OPT2:
	        opt2(inst, sol);
	        break;
		case TABU:
			tabu(set, inst, sol, (tenurefunc)defaulttenure);
			break;
		case TABU_TRIANG:
			tabu(set, inst, sol, (tenurefunc)triangulartenure);
			break;
		case TABU_SQUARE:
			tabu(set, inst, sol, (tenurefunc)squaretenure);
			break;
		case TABU_SAWTOO:
			tabu(set, inst, sol, (tenurefunc)sawtoothtenure);
			break;
		case VNS:
	        vns(set, inst, sol);
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
	printf("\t- Code: %d to skip refinement\n", SKIP);
    printf("\t- Code: %d, Algorithm: 2opt refinement method\n", OPT2);
	printf("\t- Code: %d, Algorithm: TABU refinement method with constant tenure\n", TABU);
	printf("\t- Code: %d, Algorithm: TABU refinement method with triangular tenure\n", TABU_TRIANG);
	printf("\t- Code: %d, Algorithm: TABU refinement method with square tenure\n", TABU_SQUARE);
	printf("\t- Code: %d, Algorithm: TABU refinement method with sawtooth tenure\n", TABU_SAWTOO);
	printf("\t- Code: %d, Algorithm: VNS refinement method\n", VNS);
    printf("\n");

}/* refinementaAlgorithmLegend */

/*
* IP set settings
* IP inst tsp instance
* IOP sol solution to optimize
*/
bool runRefinement(const Settings* set, const TSPInstance* inst, TSPSolution* sol){
	
	refinementaAlgorithmLegend();

    return runRefAlg(readInt("Insert the code of the refinement algorithm you want to run: "), set, inst, sol);

}/* runRefinement */
