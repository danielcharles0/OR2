/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : 2opt.c
*/

#include "2opt.h"
#include "../../../utility/utility.h"

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
void opt2(const TSPInstance* inst, TSPSolution* sol){
    bool improvement;

    do{
        improvement = refinement(inst, sol);
    } while(improvement);
        
}/* opt2 */
