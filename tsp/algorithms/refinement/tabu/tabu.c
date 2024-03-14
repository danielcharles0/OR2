/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tabu.c
*/

#include "time.h"
#include "tabu.h"
#include "../2opt/2opt.h"
#include "../../../utility/utility.h"

#define MIN_TENURE 10
#define TENURE_DENOMINATOR 10
#define TIMEOUT_WARNING_MESSAGE "Warning: The method exceeded the time limit! the solution that will be returned is the best founded so far\n\n"

/*
* IP iteration index
* IP n size of the instance
* OR tenure value for the current iteration
*/
int defaulttenure(int it, int n){

	int tenure = n / TENURE_DENOMINATOR;

	return tenure < MIN_TENURE ? MIN_TENURE : tenure;

}/* defaulttenure */

/*
* IP inst tsp instance
* IOP tl tabu list to be initialized
*/
void initTabuList(const TSPInstance* inst, TABU_LIST* tl, tenurefunc tf){
	
	int i;

	initArrayDinaInt((*inst).dimension, &((*tl).list));

	(*tl).tf = tf;

	for(i = 0; i < (*tl).list.n; i++)
		(*tl).list.v[i] = 0;

}/* initTabuList */

/*
* IOP tl tabu list to free
*/
void freeTabuList(TABU_LIST* tl){
	freeArrayDinaInt(&((*tl).list));
}/* freeTabuList */

/*
* IP it current iteration
* IP node index of the inst array
* IP tl tabu list
*/
bool isNotTabu(int it, int node, const TABU_LIST* tl){
	return it - (*tl).list.v[node] > (*tl).tf(it, (*tl).list.n);
}/* isNotTabu */

/*
* IP it current iteration
* IP inst tsp instance
* IP sol solution
* IP tl tabu list
* OP i sol->succ first index of the move
* OP j sol->succ second index of the move
* OR bool true if the move is not a tabu, false otherwise
*/
bool isNotTabuMove(int it, const TSPInstance* inst, const TSPSolution* sol, const TABU_LIST* tl, int i, int j){

	int a = (*sol).succ[i], a1 = (*sol).succ[(i + 1) % (*inst).dimension], b = (*sol).succ[j], b1 = (*sol).succ[(j + 1) % (*inst).dimension];	

	return isNotTabu(it, a, tl) && isNotTabu(it, a1, tl) && isNotTabu(it, b, tl) && isNotTabu(it, b1, tl);

}/* isTabu */

/*
* IP it current iteration
* IP inst tsp instance
* IP sol solution
* OP i sol->succ first optimal index of the move
* OP j sol->succ second optimal index of the move
* OR bool true if a not tabu move is founded, false otherwise
* NB: we have to avoid two cases, the ones when one of the two selected nodes is the successor of the other. In this case when we
* 		break the cycle we are no longer able to reconstruct it.
	Note also that the optimal move can increase the solution cost.
*/
bool getOptNotTabu2OptMove(int it, const TSPInstance* inst, const TSPSolution* sol, const TABU_LIST* tl, int* opti, int* optj){

	int i = 0, j = 2;
	double optcost, temp;
	bool notTabu = false;

	if(isNotTabuMove(it, inst, sol, tl, i, j)){
		notTabu = true;
		*opti = i;
		*optj = j;
		optcost = delta2OptMoveCost(i, j, inst, sol);
	}/* if */
	
	for(j = 3; j < (*inst).dimension - 1; j++)
												/* j < (*inst).dimension - 1 avoids to take a = b1 */
		if((temp = delta2OptMoveCost(i, j, inst, sol)) < optcost && isNotTabuMove(it, inst, sol, tl, i, j)){
			notTabu = true;
			*optj = j;
			optcost = temp;
		}/* if */

	for(i = 1; i < (*inst).dimension - 2; i++)	/* Note that j = i + 2 avoids to take b = a1 */
												/* Note that j < (*inst).dimension => i < (*inst).dimension - 2 */
		for(j = i + 2; j < (*inst).dimension; j++)
			if((temp = delta2OptMoveCost(i, j, inst, sol)) < optcost && isNotTabuMove(it, inst, sol, tl, i, j)){
				notTabu = true;
				*opti = i;
				*optj = j;
				optcost = temp;
			}
	
	return notTabu;

}/* getOptNotTabu2OptMove */

/*
* IP i iteration id
* IP inst tsp instance
* IP sol solution
* IP opti sol->succ first optimal index of the move
* IP optj sol->succ second optimal index of the move
* OP tl tabu list to be updated
*/
void updateTabuList(int i, const TSPInstance* inst, const TSPSolution* sol, int opti, int optj, TABU_LIST* tl){

	int a = (*sol).succ[opti], a1 = (*sol).succ[(opti + 1) % (*inst).dimension], b = (*sol).succ[optj], b1 = (*sol).succ[(optj + 1) % (*inst).dimension];

	(*tl).list.v[a] = (*tl).list.v[a1] = (*tl).list.v[b] = (*tl).list.v[b1] = i;

}/* updateTabuList */

/*
* IP set settings
* IP inst tsp instance
* IOP sol refined solution
* NB: this method will perform the best move that is not tabu. It can also encrease the cost.
*/
void tabu(const Settings* set, const TSPInstance* inst, TSPSolution* sol, tenurefunc tf){

	clock_t start = clock();
	int i = 0;
	int opti, optj; /* opti and optj are indexes in the sol->succ array */
	TSPSolution temp;
	TABU_LIST tl; /* TABU list */

	allocSol((*inst).dimension, &temp);
	initTabuList(inst, &tl, tf);

	while(true){

		processBar(getSeconds(start), (*set).tl);

		if(getOptNotTabu2OptMove(i, inst, sol, &tl, &opti, &optj)){
			opt2move(opti, optj, inst, sol);
			updateTabuList(i, inst, sol, opti, optj, &tl);
		}/* if */

		if(isTimeOutWarning(TIMEOUT_WARNING_MESSAGE, start, (*set).tl))
			break;

		i++;

	}/* while */

	freeTabuList(&tl);
	freeSol(&temp);

}/* tabu */
