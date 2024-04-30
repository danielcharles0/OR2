/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tabu.h
*/

#include "../../../tsp.h"
#include "../../../array/array.h"

/*
* IP iteration index
* IP n size of the instance
* OR tenure value for the current iteration
*/
typedef int (*tenurefunc)(int, int);

typedef struct{
    
	tenurefunc tf;
	ArrayDinaInt list;

} TABU_LIST;

void tabu(const Settings*, const TSPInstance*, TSPSolution*, tenurefunc);

int defaulttenure(int, int);

int triangulartenure(int, int);

int squaretenure(int, int);

int sawtoothtenure(int, int);
