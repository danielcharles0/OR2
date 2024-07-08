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
* IP i sol->path first index of the move
* IP j sol->path second index of the move
* IP inst tsp instance
* IP sol refined solution
* OR the cost of the move
*/
double delta2OptMoveCost(int i, int j, const TSPInstance* inst, const TSPSolution* sol){
	
	int a = (*sol).path[i], a1 = (*sol).path[(i + 1) % (*inst).dimension], b = (*sol).path[j], b1 = (*sol).path[(j + 1) % (*inst).dimension];

	return getDist(a, b, inst) + getDist(a1, b1, inst) - (getDist(a, a1, inst) + getDist(b, b1, inst));

}/* delta2OptMoveCost */

/*
* IP i sol->path first index of the move
* IP j sol->path second index of the move
* IP inst tsp instance
* IOP sol refined solution
* NB: it assumes i < j
*/
void opt2move(int i, int j, const TSPInstance* inst, TSPSolution* sol){
	
	int s = i + 1, t = j;

	(*sol).val += delta2OptMoveCost(i, j, inst, sol);

	invertArray(s, t, sol->path);
	
}/* opt2move */

/*
* IP inst tsp instance
* IP sol solution
* OP i sol->path first optimal index of the move
* OP j sol->path second optimal index of the move
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
		for(j = i + 2; j < (*inst).dimension; j++){

			if(i != 0 || j < (*inst).dimension - 1){

				double temp = delta2OptMoveCost(i, j, inst, sol);

				if(temp < optdelta){
					*opti = i;
					*optj = j;
					optdelta = temp;
				}/* if */

			}/* if */
			
		}
	
	return optdelta;

}/* getOpt2OptMove */

/*
* IP inst tsp instance
* IOP sol refined solution
* OR int execution seconds
* NB: this method will perform the best move, not just the first one that improve the solution cost
*/
double opt2(const Settings* set, const TSPInstance* inst, TSPSolution* sol){

	time_t start = time(0);
	double ls = -1;
	int opti, optj; /* opti and optj are indexes in the sol->path array */
	
	while(getOpt2OptMove(inst, sol, &opti, &optj) < 0){
		
		opt2move(opti, optj, inst, sol);

		if(checkTimeLimit(set, start, &ls))
            return getSeconds(start);

	}/* while */

	if((*set).v)
		processBar(1, 1);

	return getSeconds(start);

}/* opt2 */
