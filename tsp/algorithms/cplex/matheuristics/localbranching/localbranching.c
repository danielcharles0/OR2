/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : localbranching.c
*/

#include "../../cplex.h"
#include "localbranching.h"

/*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* OP sol solution
* OP et execution time in seconds
* OR 0 if no error, error code otherwise
*/
int local_branching(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, double* et){

	return 1;

}/* local_branching */
