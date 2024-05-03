/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : cplex.c
*/

#include <ilcplex/cplex.h>
#include "../cplex.h"

int candidateCallback(const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr, TSPSolution*);
