/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : benders.h
*/

#include <ilcplex/cplex.h>

#include "../cplex.h"

/*
 * IP set settings
 * IP inst input instance
 * IOP sol solution to patch
 * IOP comp array of components
 */
typedef void (*patchfunc)(const Settings*, const TSPInstance*, TSPSSolution*, COMP*);

void dummypatch(const Settings*, const TSPInstance*, TSPSSolution*, COMP*);

void patch(const Settings*, const TSPInstance*, TSPSSolution*, COMP*);

int benders(const Settings*, const TSPInstance*, CPXENVptr, CPXLPptr, TSPSolution*, patchfunc, bool);
