/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : benders.h
*/

#include <ilcplex/cplex.h>
#include "../cplex.h"

int benders(const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr, TSPSolution*);
