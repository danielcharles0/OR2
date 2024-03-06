/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tsp.h"

/*
* IP x
* OR $x rounded to the nearest integer
* Reference:
*	- http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/
*	- Page 6: http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp95.pdf
*/
int nint(double x){

    return (int)(x + .5);

}/* nint */

/*
* IOP inst instance to initialize $inst->dist
*/
void initDist(TSPInstance* inst){

    int i;

    (*inst).dist = malloc(((*inst).dimension - 1) * sizeof(double*));
    assert((*inst).dist != NULL);

    for (i = 0; i < (*inst).dimension - 1; i++){

        (*inst).dist[i] = malloc((i + 1) * sizeof(double));
        assert((*inst).dist[i] != NULL);

    }/* for */

}/* initDist */

/*
* IOP inst, compute and store into $inst->dist all the distances
*/
void computeDistances(TSPInstance* inst){
	
	int i, j;

	for(i = 1; i < (*inst).dimension; i++)
		for(j = 0; j < i; j++)
	        (*inst).dist[i - 1][j] = distance(&((*inst).points[j]), &((*inst).points[i]));

}/* computeDistances */

/*
* IP n number of nodes of the instance
* IP inst instance to initialize
*/
void allocInst(int n, TSPInstance* inst){

    (*inst).dimension = n;
    (*inst).points = malloc((*inst).dimension * sizeof(Point2D));
    
	assert((*inst).points != NULL);

	initDist(inst);

}/* allocInst */

/*
* IOP inst instance to free $inst->dist
*/
void freeDist(TSPInstance* inst){

    int i;

    if((*inst).dimension > 1){

        for (i = 0; i < (*inst).dimension - 1; i++)
            free((*inst).dist[i]);

        free((*inst).dist);

    }/* if */

}/* freeDist */

/*
* IOP inst instance to free memory
*/
void freeInst(TSPInstance* inst){
    free(inst->points);
	freeDist(inst);
}/* freeInst */

/*
* IP n number of nodes of the instance
* IP sol solution to initialize
*/
void allocSol(int n, TSPSolution* sol){

    sol->succ = malloc(n * sizeof(int));
	assert(sol->succ != NULL);

}/* allocSol */

/*
* IOP sol solution to free memory
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
    for(i = 0; i < inst->dimension; i++)
        if(i < (POINTS_TO_PRINT/2) || i >= (inst->dimension - POINTS_TO_PRINT/2))
            printf("\t\t%d %lf %lf\n", i + 1, inst->points[i].x, inst->points[i].y);
        else if ( i == (inst->dimension - POINTS_TO_PRINT/2 - 1))
            printf("\t\t\t...\n");
    printf("\n");

}/* printInst */

/*
* IP sol solution to plot
* OV hamiltonian path
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

    for(i = 0; i < inst->dimension; i++){
        int idx = i; /* just for test purposes */
        /*int idx = sol->succ[i];*/
        fprintf(gnuplotPipe, "%f %f\n", (inst->points[idx]).x, (inst->points[idx]).y);
    }
    fprintf(gnuplotPipe, "%f %f\n", (inst->points[0]).x, (inst->points[0]).y); /* connect last to first */

    fprintf(gnuplotPipe, "e\n");

    pclose(gnuplotPipe);

}/* plotSolution */
