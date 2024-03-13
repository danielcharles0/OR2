/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : output.c
*/

#include <stdio.h>

#include "output.h"

#define DEFAULT_TITLE "Hamiltonian Path"

/*
* IP gnuplotPipe pipe pointer
*/
void plotSettings(FILE* gnuplotPipe, const char title[]){

    fprintf(gnuplotPipe, "set term qt font \"Arial\"\n");
	fprintf(gnuplotPipe, "set title '%s'\n", title);
    fprintf(gnuplotPipe, "set xlabel 'X'\n");
    fprintf(gnuplotPipe, "set ylabel 'Y'\n");
    fprintf(gnuplotPipe, "plot '-' with linespoints pointtype 7 pointsize 1 linewidth 2 notitle\n");

}/* plotSettings */

/*
* IP gnuplotPipe pipe pointer
* IP inst tsp instance
* IP sol solution to bw plotted
*/
void plotNodes(FILE* gnuplotPipe, const TSPInstance* inst, const TSPSolution* sol){
	
	int i;
    Point2D* p;

    for(i = 0; i < inst->dimension; i++){
        p = &(inst->points[sol->succ[i]]);
        fprintf(gnuplotPipe, "%f %f\n", p->x, p->y);
    }
    
    p = &(inst->points[sol->succ[0]]);

    fprintf(gnuplotPipe, "%f %f\n", p->x, p->y); /* connect last to first */

    fprintf(gnuplotPipe, "e\n");

}/* plotNodes */

/*
* IP inst tsp instance
* IP sol solution to plot
* IP title plot title
* OV hamiltonian path
*/
void plotSolutionTitle(const TSPInstance* inst, const TSPSolution* sol, const char title[]){
    
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
	
    if (gnuplotPipe == NULL) {
        fprintf(stderr, "Error opening Gnuplot pipe\n");
        return;
    }

    plotSettings(gnuplotPipe, title);

    plotNodes(gnuplotPipe, inst, sol);

    pclose(gnuplotPipe);

}/* plotSolutionTitle */

/*
* IP inst tsp instance
* IP sol solution to plot
* OV hamiltonian path
*/
void plotSolution(const TSPInstance* inst, const TSPSolution* sol){
    
    plotSolutionTitle(inst, sol, DEFAULT_TITLE);

}/* plotSolution */
