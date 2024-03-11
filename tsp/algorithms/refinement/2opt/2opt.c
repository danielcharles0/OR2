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
* IP i sol->succ first index of the move
* IP j sol->succ second index of the move
* IP inst tsp instance
* IP sol refined solution
* OR the cost of the move
*/
double delta2OptMoveCost(int i, int j, const TSPInstance* inst, const TSPSolution* sol){
	
	int a = (*sol).succ[i], a1 = (*sol).succ[(i + 1) % (*inst).dimension], b = (*sol).succ[j], b1 = (*sol).succ[(j + 1) % (*inst).dimension];

	return getDist(a, b, inst) + getDist(a1, b1, inst) - (getDist(a, a1, inst) + getDist(b, b1, inst));

}/* delta2OptMoveCost */

/*
* IP i sol->succ first index of the move
* IP j sol->succ second index of the move
* IP inst tsp instance
* IOP sol refined solution
* NB: it assumes i < j
*/
void opt2move(int i, int j, const TSPInstance* inst, TSPSolution* sol){
	
	int s = i + 1, t = j;

	(*sol).val += delta2OptMoveCost(i, j, inst, sol);

	/* Note that the number of swaps performed is equal to floor((j - i - 1) / 2) */
	while(s < t){
		swapInt(&((*sol).succ[s]), &((*sol).succ[t]));
		s++;
		t--;
	}/* while */

}/* opt2move */

/*
* IP inst tsp instance
* IP sol solution
* OP i sol->succ first optimal index of the move
* OP j sol->succ second optimal index of the move
* OR the cost of the move
* NB: we have to avoid two cases, the ones when one of the two selected nodes is the successor of the other. In this case when we
* 		break the cycle we are no longer able to reconstruct it.
*/
double getOpt2OptMove(const TSPInstance* inst, const TSPSolution* sol, int* opti, int* optj){
	
	int i = 0, j = 2, optcost, temp;

	*opti = i;
	*optj = j;
	optcost = delta2OptMoveCost(i, j, inst, sol);

	for(j = 3; j < (*inst).dimension - 1; j++)
												/* j < (*inst).dimension - 1 avoids to take a = b1 */
		if((temp = delta2OptMoveCost(i, j, inst, sol)) < optcost){
			*optj = j;
			optcost = temp;
		}/* if */

	for(i = 1; i < (*inst).dimension - 2; i++)	/* Note that j = i + 2 avoids to take b = a1 */
												/* Note that j < (*inst).dimension => i < (*inst).dimension - 2 */
		for(j = i + 2; j < (*inst).dimension; j++)
			if((temp = delta2OptMoveCost(i, j, inst, sol)) < optcost){
				*opti = i;
				*optj = j;
				optcost = temp;
			}
	
	return optcost;

}/* getOpt2OptMove */

/*
* IP inst tsp instance
* IOP sol refined solution
*/
void opt2(const TSPInstance* inst, TSPSolution* sol){

	int opti, optj; /* opti and optj are indexes in the sol->succ array */
	double delta_cost;

	while((delta_cost = getOpt2OptMove(inst, sol, &opti, &optj)) < 0)
		opt2move(opti, optj, inst, sol);

}/* opt2 */

/*
* IP alg refinement algorithm to run
* IP inst tsp instance
* IOP sol refined solution
* OP error true if an error occurred, false otherwise.
*/
bool runRefAlg(REFINEMENT_ALGORITHM alg, const TSPInstance* inst, TSPSolution* sol){
    
    switch (alg){
		case SKIP:
			break;
	    case OPT2:
	        opt2(inst, sol);
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
	printf("Code: %d to skip refinement\n", SKIP);
    printf("Code: %d, Algorithm: 2opt refinement method\n", OPT2);
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

/*
* IP inst tsp instance
* IP sol solution to be improved
* IOP start index of starting point of $(sol->succ)'s sublist to invert
* IOP end index of ending point of $(sol->succ)'s sublist to invert
* OP min_delta minimum delta between initial cost of $sol and its cost after changes.
*/
double testAllChanges(const TSPInstance* inst, TSPSolution* sol, int* start, int* end){
    
    int i, j;
    double min_delta = 0;
    double cost_before_change = getSolCost(inst, sol);

    for(i=0; i<inst->dimension; i++){
        
        for(j=i+1; j<inst->dimension; j++){
            double delta;

            invertList(i, j, sol->succ);
            
            delta = getSolCost(inst, sol) - cost_before_change;
            
            if(delta < min_delta){
                min_delta = delta;
                *start = i;
                *end = j;
            }

            invertList(i, j, sol->succ); /* list back to initial state */
        }

    }

    return min_delta;

}


/*
* IP inst tsp instance
* IOP sol solution to be improved
* OR true if $sol has been improved, false otherwise.
*/
bool refinement(const TSPInstance* inst, TSPSolution* sol){
    
    double min_delta;
    int start, end;

    min_delta = testAllChanges(inst, sol, &start, &end);

    if(min_delta < 0){  /* if there is at least a change that improves the solution */
        
        invertList(start, end, sol->succ); /* apply index choices that give the best improvement */
        
        sol->val = getSolCost(inst, sol);
        
        return true;
    }

    return false;

}/* refinement */

/*
* Refines solution removing crossings or other bad connections.
* IP inst tsp instance
* IOP sol solution to be refined
*/
void opt2_v2(const TSPInstance* inst, TSPSolution* sol){
    bool improvement;

    do{
        improvement = refinement(inst, sol);
    } while(improvement);
        
}/* opt2 */
