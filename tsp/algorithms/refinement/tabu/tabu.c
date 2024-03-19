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

#define MIN_TENURE 10
#define TENURE_DENOMINATOR 10
/*#define TIMEOUT_WARNING_MESSAGE "The method exceeded the time limit! The returned solution is the best found so far\n\n"*/
#define TIMEOUT_WARNING_MESSAGE "Time limit reached! Returning the best solution.\n\n"
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
* IP inst tsp instance
* IP temp current solution found
* IOP sol best solution 
*/
void updateIncumbentSol(const TSPInstance* inst, const TSPSolution* temp, TSPSolution* sol){

    if((*temp).val < (*sol).val && checkSol(inst, temp))
        cpSol(inst, temp, sol);

} /* updateIncumbentSol */

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
		*ls = s;
	}/* if */

}/* tabuBar */

/*
* IP set settings
* IP start starting time of processing
* IOP ls last stamp second from the execution start
*/
bool checkTimeLimit(const Settings* set, int start, int* ls){

    if(isTimeOutWarning(TIMEOUT_WARNING_MESSAGE, start, (*set).tl))
        return true;
    else if((*set).v)
        tabuBar(start, (*set).tl, ls);
    
    return false;

}/* checkTimeLimit */

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

	int i, j;
	double optdelta;
	bool notTabu = false;

	for(i = 0; i < (*inst).dimension - 2; i++)	/* Note that j = i + 2 avoids to take b = a1 */
												/* Note that j < (*inst).dimension => i < (*inst).dimension - 2 */
		for(j = i + 2; j < (*inst).dimension; j++)
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
* NB: this method will perform the best move that is not tabu. It can also encrease the cost.
*/
void tabu(const Settings* set, const TSPInstance* inst, TSPSolution* sol, tenurefunc tf){

	clock_t start = clock();
	TSPSolution temp;
	TABU_LIST tl; /* TABU list */
	int it = 0, ls = -1; /* ls := last stamp, seconds from the start to the last stamp */
	int opti, optj; /* opti and optj are indexes in the sol->path array */

	allocSol((*inst).dimension, &temp);
	initTabuList(inst, &tl, tf);

	cpSol(inst, sol, &temp);

	while(true){

		if(getOptNotTabu2OptMove(it, inst, &temp, &tl, &opti, &optj)){
			
			tabuMove(inst, it, opti, optj, &tl, &temp);

			updateIncumbentSol(inst, &temp, sol);
            
		}/* if */

		if(checkTimeLimit(set, start, &ls))
            break;

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
* OP temp list of int succesfully allocated
*/
int* allocTabuList(int n, int* tenure){
    
    int* temp;

    *tenure = max(MIN_TENURE, n/100);

    temp = (int*) malloc((*tenure) * sizeof(int));
    assert(temp != NULL);

    for(int i=0; i<(*tenure); i++)
        temp[i] = -1;

    return temp;

}/* allocTabuList */

/*
* IP max_tenure maximum tenure available
* IOP current_tenure tenure value to be modified
* IOP next_tabu index to be adapted to $tenure
* IOP tabuList tabu list
*/
void changeTenure(int max_tenure, int* current_tenure, int* next_tabu, int* tabuList){

    int rand = rand0N(4);
    bool enlarged;
    
    if(rand == 3){  /* upper 0.25-th quantile */
        *current_tenure = (*current_tenure) / 2;
        enlarged = false;
    }
    else{
        *current_tenure = (max_tenure < ((*current_tenure) * 2) ? max_tenure : ((*current_tenure) * 2));
        enlarged = true;
    }

    if(*next_tabu > *current_tenure)
        *next_tabu = 0;
    else if (enlarged)
        *next_tabu = *current_tenure / 2;
    
    /* forget about other tabus if list become again of max_tenure size */
    for(int i=(*current_tenure); i<max_tenure; i++)
        tabuList[i] = -1;

}/* changeTenure */

/*
* IP i index
* IP j index
* IP inst tsp instance
* IP sol solution to be explored
* IP tenure current tenure of $tabuList
* IOP tabuList list of tabu indices
* OR bool true if tabu move, false otherwise.
*/
bool isTabu(int i, int j, const TSPInstance* inst, const TSPSolution* sol, int tenure, int* tabuList){

    int a = sol->path[i];
    int b = sol->path[j];
    int c = sol->path[(i+1) % inst->dimension];
    int d = sol->path[(j+1) % inst->dimension];
        
    for(int k=0; k<tenure; k++){
        
        if(a == tabuList[k] || b == tabuList[k] || c == tabuList[k] || d == tabuList[k])
            return true;
        
    }
    
    return false;
    
}/* isTabu */

/*
* IP i index to be set as tabu
* IP tenure current tenure of $tabuList
* IOP tabuList list of tabu indices
* IOP next_tabu next index of $tabuList that has to be overwritten
*/
void setTabu(int i, int tenure, int* tabuList, int* next_tabu){

    tabuList[(*next_tabu) % tenure] = i;

    *next_tabu = ((*next_tabu) + 1) % tenure;

}/* setTabu */

/*
* IP inst tsp instance
* IP sol solution to be modified
* IP tabuList list of tabu moves
* IP tenure dimension available of $tabuList
* IOP next_tabu next index of $tabuList that has to be overwritten
*/
void bestNotTabuMove(const TSPInstance* inst, TSPSolution* sol, int* tabuList, int tenure, int* next_tabu){
    
    int start, end;
    double min_delta = 0;
    bool first_valid = true;

    for(int i=0; i<inst->dimension; i++){

        for(int j=i+2; j<inst->dimension; j++){
            
            if(!isTabu(i, j, inst, sol, tenure, tabuList)){
                
                double delta = computeDelta(i, j, inst, sol);

                if(delta < min_delta || first_valid){ 
                    min_delta = delta;
                    start = (i+1) % inst->dimension;
                    end = j;

                    first_valid = false;
                }

            }

        }

    }

    invertList(start, end, sol->path);
    
    sol->val += min_delta;

    setTabu(sol->path[end], tenure, tabuList, next_tabu); 

}/* bestNotTabuMove */

/*
* IP set settings
* IP inst tsp instance
* IOP sol solution to be improved
* OR int execution time in seconds
*/
int tabu_v2(const Settings* set, const TSPInstance* inst, TSPSolution* sol){
    
    clock_t start = clock();
    TSPSolution temp;
    int max_tenure;
    int next_tabu = 0;
    int iter = 0;
    int ls = -1;

    int* tabuList = allocTabuList(inst->dimension, &max_tenure);

    int current_tenure = max_tenure;

    allocSol(inst->dimension, &temp);

    cpSol(inst, sol, &temp);

    while(true){

        if((iter % max_tenure) == 0 && iter != 0)  
            changeTenure(max_tenure, &current_tenure, &next_tabu, tabuList);
        
        bestNotTabuMove(inst, &temp, tabuList, current_tenure, &next_tabu);

        updateIncumbentSol(inst, &temp, sol);

        if(checkTimeLimit(set, start, &ls))
            break;

        iter++;

    }

    free(tabuList);

    return getSeconds(start);

}/* tabu_v2 */
