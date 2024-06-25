/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : candidate.c
*/

#include <ilcplex/cplex.h>

#include "../cplex.h"

int candidate(CPXENVptr, CPXLPptr, CPXInstance*);

int add_SEC_candidate(const TSPInstance*, const COMP*, CPXENVptr, CPXLPptr, CPXCALLBACKCONTEXTptr, int);

int CPXPUBLIC checkCandidateSSol(CPXCALLBACKCONTEXTptr, CPXLONG, CPXInstance*);
