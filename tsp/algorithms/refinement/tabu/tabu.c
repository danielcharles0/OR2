/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tabu.c
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "tabu.h"
#include "../2opt/2opt.h"
#include "../../../utility/utility.h"

#define MIN_TENURE 10

/*
* IP n tsp instance dimension
* IOP tabuList list of lists to be allocated
* IOP tenure dimension of the tabuList
*/
void allocTabuList(int n, int* tabuList, int* tenure){

    *tenure = max(MIN_TENURE, n/100);

    tabuList = (int*) malloc((*tenure) * sizeof(int*));

    for(int i=0; i<(*tenure); i++)
        tabuList[i] = -1;

}/* allocTabuList */

/*
* IP tabuList list to be freed
* IP tenure dimension of the list
*/
void freeTabuList(int* tabuList, int tenure){

    free(tabuList);

}/* freeTabuList */

/*
* IOP tenure value to be modified
* IOP next_tabu index to be adapted to $tenure
*/
void changeTenure(int* current_tenure, int* next_tabu, int tenure, int* tabuList){

    int rand = rand0N(4);

    if(rand == 3) /* upper 0.25-th quantile */
        *current_tenure = (*current_tenure) / 2;
    else
        *current_tenure = max(MIN_TENURE, (*current_tenure) * 2);

    if(*next_tabu > *current_tenure)
        *next_tabu = 0;
    
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
bool isTabu(int i, int j, const TSPInstance* inst, const TSPSolution* sol, int* tabuList, int tenure){

    int a = sol->succ[i];
    int b = sol->succ[j];
    int c = sol->succ[(i+1) % inst->dimension];
    int d = sol->succ[(j+1) % inst->dimension];
    
    for(int k=0; k<tenure; k++){

        if(a == tabuList[k]){
            /*if(iter - tenure < tabuList[k][1])*/
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

    *next_tabu = (*next_tabu) + 1;

}/* setTabu */

/*
* Computes delta between new 
* IP i index of a node
* IP j index of a node that can have a crossing with $start
* IP inst tsp instance
* IP sol solution to be improved
* OP delta difference between current cost and cost without crossing
*/
double T_computeDelta(int i, int j, const TSPInstance* inst, const TSPSolution* sol){
	
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
* IP sol solution to be modified
* IP tabuList list of tabu moves
* IP tenure dimension available of $tabuList
*/
double move(const TSPInstance* inst, TSPSolution* sol, int* tabuList, int tenure, int* next_tabu){
    
    int i, j, start, end;
    double min_delta = 0;
    bool first_valid = true;

    for(i=0; i<inst->dimension; i++){

        for(j=i+2; j<inst->dimension; j++){

			double delta = T_computeDelta(i, j, inst, sol);
            
            if((delta < min_delta || first_valid) && !isTabu(i, j, inst, sol, tabuList, tenure)){ 
                min_delta = delta;
                start = (i+1) % inst->dimension;
                end = j;

                first_valid = false;
            }
			
        }

    }

    invertList(start, end, sol->succ);
    
    setTabu(sol->succ[end], tabuList, next_tabu, tenure); 

    return min_delta;

}/* move */

/*
* IP inst tsp instance
* IOP sol solution to be improved
* OR int execution in seconds
*/
int tabu(const TSPInstance* inst, TSPSolution* sol){
    
    clock_t start = clock();
    int tenure, current_tenure;
    int* tabuList = NULL;
    int iter = 0;
    int next_tabu = 0;

    allocTabuList(inst->dimension, tabuList, &tenure);

    do{
        if((iter % tenure) == 0)    
            changeTenure(&current_tenure, &next_tabu, tenure, tabuList);
        sol->val += move(inst, sol, tabuList, current_tenure, &next_tabu);
        next_tabu = next_tabu % tenure;
        iter++;
    }while(iter<200);

    freeTabuList(tabuList, tenure);

    return getSeconds(start, clock());

}/* tabu */
