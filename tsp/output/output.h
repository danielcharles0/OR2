/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : output.h
*/

#include "../tsp.h"

void plotSolutionTitle(const TSPInstance*, const TSPSolution*, const char[]);

void plotSolution(const TSPInstance*, const TSPSolution*);

FILE* initCostFile(const char[]);

FILE* initCostPlotPipe(const char[]);

void plotCost(FILE*, FILE*, const char[], int, double);

void closeCostFile(FILE*);

void closePipe(FILE*);
