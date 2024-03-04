/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "tsp.h"

/*
* IP inst instance to allocate
*/
void allocInst(TSPInstance* inst){
    inst->points = malloc(inst->dimension * sizeof(Point));
}/* allocInst */

/*
* IP inst instance to free
*/
void freeInst(TSPInstance* inst){
    free(inst->points);
}/* freeInst */

/*
* IP inst instance to get dimension
* IP sol solution to allocate
*/
void allocSol(TSPInstance* inst, TSPSolution* sol){
    sol->succ = malloc(inst->dimension * sizeof(Point));
}/* allocSol */

/*
* IP sol solution to free
*/
void freeSol(TSPSolution* sol){
    free(sol->succ);
}/* freeSol */

/*
* IP inst instance to print
*/
void printInst(const TSPInstance* inst){
    int i;

    printf("Instance:\n");
    printf("\tName: %s\n", inst->name);
    printf("\tDimension: %d\n", inst->dimension);
    printf("\tNodes:\n");
    for(i=0; i<inst->dimension; i++){
        if(i < (POINTS_TO_PRINT/2) || i >= (inst->dimension - POINTS_TO_PRINT/2))
            printf("\t\t%d %lf %lf\n", inst->points[i].id, inst->points[i].x, inst->points[i].y);
        else if ( i == (inst->dimension - POINTS_TO_PRINT/2 - 1))
            printf("\t\t\t...\n");
    }
    printf("\n");

}/* printInst */

/*
* IP sol solution to plot
* OV hamiltonian cycle
*/
void plotSolution(const TSPSolution* sol, const TSPInstance* inst){
    int i;
    
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (gnuplotPipe == NULL) {
        fprintf(stderr, "Error opening Gnuplot pipe\n");
        return;
    }

    fprintf(gnuplotPipe, "set title 'Hamiltonian Path'\n");
    fprintf(gnuplotPipe, "set xlabel 'X'\n");
    fprintf(gnuplotPipe, "set ylabel 'Y'\n");
    fprintf(gnuplotPipe, "plot '-' with linespoints pointtype 7 pointsize 2 linewidth 2 notitle\n");

    for (i = 0; i < inst->dimension; i++) {
        int idx = i; /* just for test purposes */
        /*int idx = sol->succ[i];*/
        fprintf(gnuplotPipe, "%f %f\n", (inst->points[idx]).x, (inst->points[idx]).y);
    }
    fprintf(gnuplotPipe, "%f %f\n", (inst->points[0]).x, (inst->points[0]).y); /* connect last to first */

    fprintf(gnuplotPipe, "e\n");

    pclose(gnuplotPipe);

}/* plotSolution */
