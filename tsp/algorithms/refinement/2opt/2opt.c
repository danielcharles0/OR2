/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : 2opt.c
*/

#include <stdio.h>
#include "2opt.h"
#include "../../../utility/utility.h"
#include "../tabu/tabu.h"

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
	
	int i = 0, j = 2;
	double optdelta;

	*opti = i;
	*optj = j;
	optdelta = delta2OptMoveCost(i, j, inst, sol);

	for(i = 0; i < (*inst).dimension - 2; i++)	/* Note that j = i + 2 avoids to take b = a1 */
												/* Note that j < (*inst).dimension => i < (*inst).dimension - 2 */
		for(j = i + 2; j < (*inst).dimension; j++)
			if(i != 0 || j < (*inst).dimension - 1){
				double temp = delta2OptMoveCost(i, j, inst, sol);
				if(temp < optdelta){
					*opti = i;
					*optj = j;
					optdelta = temp;
				}/* if */
			}/* if */
	
	return optdelta;

}/* getOpt2OptMove */

/*
* IP inst tsp instance
* IOP sol refined solution
* OR int execution seconds
* NB: this method will perform the best move, not just the first one that improve the solution cost
*/
int opt2(const TSPInstance* inst, TSPSolution* sol){

	clock_t start = clock();
	int opti, optj; /* opti and optj are indexes in the sol->succ array */
	
	while(getOpt2OptMove(inst, sol, &opti, &optj) < 0)
		opt2move(opti, optj, inst, sol);
	
	return getSeconds(start);

}/* opt2 */

/*
* Computes delta between new edges and old edges.
* IP i index of a node
* IP j index of a node that can have a crossing with $start
* IP inst tsp instance
* IP sol solution to be improved
* OP delta difference between current cost and cost without crossing
*/
double computeDelta(int i, int j, const TSPInstance* inst, const TSPSolution* sol){
	
	int n = inst->dimension;

	int a = sol->succ[i];
	int b = sol->succ[j];
	int a_next = sol->succ[(i+1) % n];
	int b_next = sol->succ[(j+1) % n];

	if(b_next == a || a_next == b || a_next == b_next)
		return 0;

	double old1 = getDist(a, a_next, inst);
	double old2 = getDist(b, b_next, inst);
	double new1 = getDist(a, b, inst);
	double new2 = getDist(a_next, b_next, inst);

	double delta = (new1 + new2) - (old1 + old2);

	return delta;

}/* computeDelta */

/*
* IP inst tsp instance
* IP sol solution to be improved
* IOP start index of starting point of $(sol->succ)'s sublist to invert
* IOP end index of ending point of $(sol->succ)'s sublist to invert
* OP min_delta minimum delta between initial cost of $sol and its cost after the move.
*/
double bestMove(const TSPInstance* inst, TSPSolution* sol, int* start, int* end){
    
    int i, j;
    double min_delta = 0;

    for(i=0; i<inst->dimension; i++){

        for(j=i+2; j<inst->dimension; j++){

			double delta = computeDelta(i, j, inst, sol);
            
            if(delta < min_delta){
                min_delta = delta;
                *start = (i+1) % inst->dimension;
                *end = j;
            }
			
        }

    }

    return min_delta;

}/* bestMove */

/*
* IP inst tsp instance
* IOP sol solution to be improved
* OR true if $sol has been improved, false otherwise.
*/
bool refinement(const TSPInstance* inst, TSPSolution* sol){
    
    double min_delta;
    int start, end;

    min_delta = bestMove(inst, sol, &start, &end);

    if(min_delta < 0){  /* if there is at least a change that improves the solution */
        
        invertList(start, end, sol->succ); /* apply index choices that give the best improvement */
        
        sol->val += min_delta;

        return true;
    }

    return false;

}/* refinement */

/*
* Refines solution removing crossings or other bad connections.
* IP inst tsp instance
* IOP sol solution to be refined
* OR int execution in seconds
*/
int opt2_v2(const TSPInstance* inst, TSPSolution* sol){
    
	clock_t start = clock();
	bool improvement;

    do{
        improvement = refinement(inst, sol);
    } while(improvement);

	return getSeconds(start);
        
}/* opt2_v2 */
