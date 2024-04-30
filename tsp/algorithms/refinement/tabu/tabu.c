/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tabu.c
*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "tabu.h"
#include "../2opt/2opt.h"
#include "../../../utility/utility.h"
#include "../../../output/output.h"

#define MIN_TENURE 10
#define TENURE_DENOMINATOR 10

/*
* IP iteration index
* IP n size of the instance
* OR tenure value for the current iteration
* Costant tenure
*/
int defaulttenure(int it, int n){

	int tenure = n / TENURE_DENOMINATOR;

	return max(tenure, MIN_TENURE);

}/* defaulttenure */

/*
* IP iteration index
* IP n size of the instance
* OR tenure value for the current iteration
* Triangular tenure
*/
int triangulartenure(int it, int n){
	
	int max_tenure = defaulttenure(0, n);
	int h = max_tenure - MIN_TENURE, win = it / h, val = it % h;
	
	return MIN_TENURE + ((win % 2) ? h - val : val);

}/* triangulartenure */

/*
* IP iteration index
* IP n size of the instance
* OR tenure value for the current iteration
* Square tenure
*/
int squaretenure(int it, int n){
	
	int max_tenure = defaulttenure(0, n);
	int h = max_tenure - MIN_TENURE, win = it / h;
	
	return ((win % 2) ? max_tenure : MIN_TENURE);

}/* squaretenure */

/*
* IP iteration index
* IP n size of the instance
* OR tenure value for the current iteration
* Sawtooth tenure
*/
int sawtoothtenure(int it, int n){
	
	int max_tenure = defaulttenure(0, n);
	int h = max_tenure - MIN_TENURE, val = it % (3 * h);
	
	return MIN_TENURE + val / 3;

}/* sawtoothtenure */

/*
* IP inst tsp instance
* IOP tl tabu list to be initialized
*/
void initTabuList(const TSPInstance* inst, TABU_LIST* tl, tenurefunc tf){
	
	int val;

	initArrayDinaInt((*inst).dimension, &((*tl).list));

	(*tl).tf = tf;
	val = -(*tl).tf(0, (*inst).dimension) - 1;

	for(int i = 0; i < (*tl).list.n; i++)
		(*tl).list.v[i] = val;

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
* OP i sol->path first index of the move
* OP j sol->path second index of the move
* OR bool true if the move is not a tabu, false otherwise
*/
bool isNotTabuMove(int it, const TSPInstance* inst, const TSPSolution* sol, const TABU_LIST* tl, int i, int j){

	int a = (*sol).path[i], a1 = (*sol).path[(i + 1) % (*inst).dimension], b = (*sol).path[j], b1 = (*sol).path[(j + 1) % (*inst).dimension];	

	return isNotTabu(it, a, tl) && isNotTabu(it, a1, tl) && isNotTabu(it, b, tl) && isNotTabu(it, b1, tl);

}/* isNotTabuMove */

/*
* IP it current iteration
* IP inst tsp instance
* IP sol solution
* OP i sol->path first optimal index of the move
* OP j sol->path second optimal index of the move
* OR bool true if a not tabu move is founded, false otherwise
* NB: we have to avoid two cases, the ones when one of the two selected nodes is the successor of the other. In this case when we
* 		break the cycle we are no longer able to reconstruct it.
	Note also that the optimal move can increase the solution cost.
*/
bool getOptNotTabu2OptMove(int it, const TSPInstance* inst, const TSPSolution* sol, const TABU_LIST* tl, int* opti, int* optj){

	double optdelta;
	bool notTabu = false;

	for(int i = 0; i < (*inst).dimension - 2; i++)	/* Note that j = i + 2 avoids to take b = a1 */
												    /* Note that j < (*inst).dimension => i < (*inst).dimension - 2 */
		for(int j = i + 2; j < (*inst).dimension; j++){

			if(i != 0 || j < (*inst).dimension - 1){

				if(isNotTabuMove(it, inst, sol, tl, i, j)){

                    double temp = delta2OptMoveCost(i, j, inst, sol);

                    if(!notTabu || temp < optdelta){
                        notTabu = true;
                        *opti = i;
                        *optj = j;
                        optdelta = temp;
                    }/* if */

                }

			}/* if */
        }

	return notTabu;

}/* getOptNotTabu2OptMove */

/*
* IP it iteration id
* IP inst tsp instance
* IP sol solution
* IP opti sol->path first optimal index of the move
* IP optj sol->path second optimal index of the move
* OP tl tabu list to be updated
*/
void updateTabuList(int it, const TSPInstance* inst, const TSPSolution* sol, int opti, int optj, TABU_LIST* tl){

	int a = (*sol).path[opti];/*, a1 = (*sol).path[(opti + 1) % (*inst).dimension], b = (*sol).path[optj], b1 = (*sol).path[(optj + 1) % (*inst).dimension];*/

	/*(*tl).list.v[a] = (*tl).list.v[a1] = (*tl).list.v[b] = (*tl).list.v[b1] = it;*/
	(*tl).list.v[a] = it;

}/* updateTabuList */

/*
* IP inst tsp instance
* IP it iteration id
* OP i sol->path first index of the move
* OP j sol->path second index of the move
* OP tl tabu list to be updated
* IOP sol refined solution
*/
void tabuMove(const TSPInstance* inst, int it, int i, int j, TABU_LIST* tl, TSPSolution* sol){

	opt2move(i, j, inst, sol);

	updateTabuList(it, inst, sol, i, j, tl);

}/* tabuMove */

/*
* IP set settings
* IP inst tsp instance
* IOP sol refined solution
* NB: this method will perform the best move that is not tabu. It can also increase the cost.
*/
void tabu(const Settings* set, const TSPInstance* inst, TSPSolution* sol, tenurefunc tf){

	clock_t start = clock();
	TSPSolution temp;
	TABU_LIST tl; /* TABU list */
	FILE* cost_pipe;
	int it = 0;
	int opti, optj; /* opti and optj are indexes in the sol->path array */
	double ls = -1, lp = -1; /* ls := last stamp, seconds from the start to the last stamp */

	initCostPlotPipe("TABU - Solutions Costs", &cost_pipe);

	allocSol((*inst).dimension, &temp);
	initTabuList(inst, &tl, tf);

	cpSol(inst, sol, &temp);

	while(true){

		if(getOptNotTabu2OptMove(it, inst, &temp, &tl, &opti, &optj)){
			
			tabuMove(inst, it, opti, optj, &tl, &temp);

			updateIncumbentSol(inst, &temp, sol);
            
		}/* if */

		if(timeToPlot(start, COST_SAMPLING_FREQUENCY, &lp))
        	addCost(cost_pipe, it, temp.val);

		if(checkTimeLimit(set, start, &ls))
            break;

		it++;

	}/* while */

	freeTabuList(&tl);
	freeSol(&temp);
	closeGnuplotPipe(cost_pipe);

}/* tabu */
