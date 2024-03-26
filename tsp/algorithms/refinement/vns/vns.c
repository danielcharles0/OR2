/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : vns.c
*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "vns.h"
#include "../2opt/2opt.h"
#include "../tabu/tabu.h"
#include "../../../utility/utility.h"
#include "../../../output/output.h"

#define MAX_KICKS 7

/*
* IP n integers selected in [0, n-1] 
* OP i minimum index of the three selected
* OP j inintermediate index of the three selected
* OP k maximum index of the three selected
*/
void selectRandomIndexes(int n, int* i, int* j, int* k){

    int a, b, c;
    bool notValidTriplet = true;

    while(notValidTriplet){

        a = rand0N(n);
        b = rand0N(n);
        c = rand0N(n);

        if(abs(a-b) > 1 && abs(a-c) > 1 && abs(b-c) > 1)
            notValidTriplet = false;

    }

    sortInt(&a, &b, &c);

    *i = a;
    *j = b;
    *k = c;

}/* selectRandomIndexes */

/*
* IP n integers selected in [0, n-1] 
* OP i minimum index of the three selected
* OP j intermediate index of the three selected
* OP k maximum index of the three selected
*/
void selectRandomIndexes_v2(int n, int* i, int* j, int* k){

    int arr[3];
    bool notValidTriplet = true;

    while(notValidTriplet){

        for(int l=0; l<3; l++)
            arr[l] = rand0N(n);

        if(abs(arr[0]-arr[1]) > 1 && abs(arr[0]-arr[2]) > 1 && abs(arr[1]-arr[2]) > 1)
            notValidTriplet = false;

    }

    sortIntArray(3, arr);

    *i = arr[0];
    *j = arr[1];
    *k = arr[2];

}/* selectRandomIndexes_v2 */

/*
* IP inst tsp instance
* IOP sol solution to be modified
*/
void randomized3Kick(const TSPInstance* inst, TSPSolution* sol){
    
    int i, j, k;

    selectRandomIndexes_v2(inst->dimension, &i, &j, &k);

    invertArray(i+1, j, sol->path);

    invertArray(j+1, k, sol->path);

    sol->val = getSolCost(inst, sol);

}/* randomized3Opt */

/*
* IP inst tsp instance
* IOP sol solution to be modified
*/
void kickSol(const TSPInstance* inst, TSPSolution* sol){

    int kicks = rand0N(MAX_KICKS) + 1;
    
    for(int i=0; i<kicks; i++)
        randomized3Kick(inst, sol);

}/* kickSol */

/*
* IP set settings
* IP inst tsp instance
* IOP sol refined solution
*/
void vns(const Settings* set, const TSPInstance* inst, TSPSolution* sol){

    clock_t start = clock();
    TSPSolution temp;
    FILE* pipe;
	double ls = -1, lp = -1;
    int iter = 0;

    initCostPlotPipe("VNS - Solutions Costs", &pipe);

    allocSol(inst->dimension, &temp);

    cpSol(inst, sol, &temp);

    while(true){

        opt2(inst, &temp);

        updateIncumbentSol(inst, &temp, sol);

        if(checkTimeLimit(set, start, &ls))
            break;

        if(timeToPlot(start, COST_SAMPLING_FREQUENCY, &lp))
        	addCost(pipe, iter++, temp.val);

        kickSol(inst, &temp);

    }

    freeSol(&temp);
	closeGnuplotPipe(pipe);

}/* vns */
