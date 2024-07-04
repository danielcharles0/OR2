/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : localbranching.h
*/

//#include <ilcplex/cplex.h>
#include "../../../../tsp.h"
#include "../../../../input/settings/settings.h"

int local_branching(const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr, TSPSolution*, double*);
