/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tabu.c
*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "tabu.h"
#include "../2opt/2opt.h"
#include "../../../utility/utility.h"

#define MIN_TENURE 10
#define TENURE_DENOMINATOR 10
#define TIMEOUT_WARNING_MESSAGE "The method exceeded the time limit! The returned solution is the best found so far\n\n"
#define TIMEOUT_WARNING_MESSAGE "The method exceeded the time limit! The returned solution is the best found so far\n\n"
#define PRINT_FREQUENCY 1 /* seconds between one print and another */

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
	
	int i, val;

	initArrayDinaInt((*inst).dimension, &((*tl).list));

	(*tl).tf = tf;
	val = -(*tl).tf(0, (*inst).dimension);

	for(i = 0; i < (*tl).list.n; i++)
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

	int i, j;
	double optdelta;
	bool notTabu = false;

	for(i = 0; i < (*inst).dimension - 2; i++)	/* Note that j = i + 2 avoids to take b = a1 */
												/* Note that j < (*inst).dimension => i < (*inst).dimension - 2 */
		for(j = i + 2; j < (*inst).dimension; j++)
			if(i != 0 || j < (*inst).dimension - 1){
				double temp = delta2OptMoveCost(i, j, inst, sol);
				if((!notTabu || temp < optdelta) && isNotTabuMove(it, inst, sol, tl, i, j)){
					notTabu = true;
					*opti = i;
					*optj = j;
					optdelta = temp;
				}/* if */
			}/* if */
	
	return notTabu;

}/* getOptNotTabu2OptMove */

/*
* IP it iteration id
* IP inst tsp instance
* IP sol solution
* IP opti sol->succ first optimal index of the move
* IP optj sol->succ second optimal index of the move
* OP tl tabu list to be updated
*/
void updateTabuList(int it, const TSPInstance* inst, const TSPSolution* sol, int opti, int optj, TABU_LIST* tl){

	int a = (*sol).succ[opti];/*, a1 = (*sol).succ[(opti + 1) % (*inst).dimension], b = (*sol).succ[optj], b1 = (*sol).succ[(optj + 1) % (*inst).dimension];*/

	/*(*tl).list.v[a] = (*tl).list.v[a1] = (*tl).list.v[b] = (*tl).list.v[b1] = it;*/
	(*tl).list.v[a] = it;

}/* updateTabuList */

/*
* IP start executing time start
* IP tl time limit
* IOP ls last stamp second from the execution time start
*/
void tabuBar(clock_t start, int tl, int* ls){
	
	int s = getSeconds(start);


	if(s - *ls >= PRINT_FREQUENCY){
		processBar(s, tl);
		printSeconds("Running time: ", s);
		printSeconds("Running time: ", s);
		*ls = s;
	}/* if */

}/* tabuBar */

/*
* IP inst tsp instance
* IP it iteration id
* OP i sol->succ first index of the move
* OP j sol->succ second index of the move
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
* NB: this method will perform the best move that is not tabu. It can also encrease the cost.
*/
void tabu(const Settings* set, const TSPInstance* inst, TSPSolution* sol, tenurefunc tf){

	clock_t start = clock();
	TSPSolution temp;
	TABU_LIST tl; /* TABU list */
	int it = 0, ls = -1; /* ls := last stamp, seconds from the start to the last stamp */
	int opti, optj; /* opti and optj are indexes in the sol->succ array */

	allocSol((*inst).dimension, &temp);
	initTabuList(inst, &tl, tf);

	cpSol(inst, sol, &temp);

	while(true){

		if(getOptNotTabu2OptMove(it, inst, &temp, &tl, &opti, &optj)){
			
			tabuMove(inst, it, opti, optj, &tl, &temp);

			if(temp.val < (*sol).val)
				cpSol(inst, &temp, sol);
		}/* if */

		if(isTimeOutWarning(TIMEOUT_WARNING_MESSAGE, start, (*set).tl))
			break;
		else if((*set).v)
			tabuBar(start, (*set).tl, &ls);

		it++;

	}/* while */

	freeTabuList(&tl);
	freeSol(&temp);

}/* tabu */



/* VERSION 2 
      |
      |
      |
     \ /
      V
*/

/*
* IP n tsp instance dimension
* IOP tenure dimension of the tabuList
* OP tabuList list of lists to be allocated
*/
int* allocTabuList(int n, int* tenure){
    
    int* temp;

    *tenure = max(MIN_TENURE, n/100);

    temp = (int*) malloc((*tenure) * sizeof(int));

    for(int i=0; i<(*tenure); i++)
        temp[i] = -1;

    return temp;

}/* allocTabuList */

/*
* IP tabuList list to be freed
* IP tenure dimension of the list
*/
void freeTabuList_v2(int* tabuList, int tenure){

    free(tabuList);

}/* freeTabuList */

/*
* IOP tenure value to be modified
* IOP next_tabu index to be adapted to $tenure
*/
void changeTenure(int* current_tenure, int* next_tabu, int tenure, int* tabuList){

    int rand = rand0N(4);
    bool enlarged;
    
    if(rand == 3){  /* upper 0.25-th quantile */
        *current_tenure = (*current_tenure) / 2;
        enlarged = false;
    }
    else{
        *current_tenure = (tenure < ((*current_tenure) * 2) ? tenure : ((*current_tenure) * 2));
        enlarged = true;
    }

    if(*next_tabu > *current_tenure)
        *next_tabu = 0;
    else if (enlarged)
        *next_tabu = *current_tenure / 2;
    
    /* forget about other tabus if list become again of tenure size */
    for(int i=(*current_tenure); i<tenure; i++)
        tabuList[i] = -1;

}/* changeTenure */

/*
* IP i index
* IP j index
* IP inst tsp instance
* IP sol solution to be explored
* IP tabuList list of tabu indices
* IP tenure current tenure of $tabuList
* IP iter current iteration
* OR bool true if tabu move, false otherwise.
*/
bool isTabu(int i, int j, const TSPInstance* inst, const TSPSolution* sol, int tenure, int* tabuList){

    int a = sol->succ[i];
    int b = sol->succ[j];
    int c = sol->succ[(i+1) % inst->dimension];
    int d = sol->succ[(j+1) % inst->dimension];
        
    for(int k=0; k<tenure; k++){
        
        if(a == tabuList[k]){
            return true;
        }
        else if(b == tabuList[k]){
            return true;
        }
        else if(c == tabuList[k]){
            return true;
        }
        else if(d == tabuList[k]){
            return true;
        }
        
    }
    
    return false;
    
}/* isTabu */

/*
* IP i index to be set as tabu
* IOP tabuList list of tabu indices
* IP iter current iteration
* IOP oldest_tabu oldest index that have been added
*/
void setTabu(int i, int* tabuList, int* next_tabu, int tenure){

    tabuList[(*next_tabu) % tenure] = i;

    *next_tabu = ((*next_tabu) + 1) % tenure;

}/* setTabu */

/*
* IP inst tsp instance
* IP sol solution to be modified
* IP tabuList list of tabu moves
* IP tenure dimension available of $tabuList
*/
void bestNotTabuMove(const TSPInstance* inst, TSPSolution* sol, int* tabuList, int tenure, int* next_tabu){
    
    int i, j, start, end;
    double min_delta = 0;
    bool first_valid = true;

    for(i=0; i<inst->dimension; i++){

        for(j=i+2; j<inst->dimension; j++){
            
			double delta = computeDelta(i, j, inst, sol);

            if((delta < min_delta || first_valid) && !isTabu(i, j, inst, sol, tenure, tabuList)){ 
                min_delta = delta;
                start = (i+1) % inst->dimension;
                end = j;

                first_valid = false;
            }
			
        }

    }

    invertList(start, end, sol->succ);
    
    sol->val += min_delta;

    setTabu(sol->succ[end], tabuList, next_tabu, tenure); 

}/* bestNotTabuMove */

/*
* IP inst tsp instance
* IOP sol solution to be improved
* OR int execution in seconds
*/
int tabu_v2(const Settings* set, const TSPInstance* inst, TSPSolution* sol){
    
    clock_t start = clock();
    TSPSolution temp;
    int tenure;
    int next_tabu = 0;
    int iter = 0;
    int ls = -1;

    int* tabuList = allocTabuList(inst->dimension, &tenure);

    int current_tenure = tenure;

    allocSol(inst->dimension, &temp);

    cpSol(inst, sol, &temp);

    while(true){

        if((iter % tenure) == 0 && iter != 0)  
            changeTenure(&current_tenure, &next_tabu, tenure, tabuList);
        
        bestNotTabuMove(inst, &temp, tabuList, current_tenure, &next_tabu);

        if(temp.val < sol->val)
            cpSol(inst, &temp, sol);

        if(isTimeOutWarning(TIMEOUT_WARNING_MESSAGE, start, (*set).tl))
			break;
        else if((*set).v)
			tabuBar(start, (*set).tl, &ls);

        iter++;

    }

    freeTabuList_v2(tabuList, tenure);

    return getSeconds(start);

}/* tabu_v2 */
