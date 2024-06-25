/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : usercut.c
*/

#include <ilcplex/cplex.h>

#include "../cplex.h"

typedef struct{
    CPXInstance* cpx_inst;
    CPXCALLBACKCONTEXTptr context;
} CCInstance;

int usercut(CPXENVptr, CPXLPptr, CPXInstance*);
