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
    fprintf(gnuplotPipe, "set terminal '%s'\n", "x11");
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
        p = &(inst->points[sol->path[i]]);
        fprintf(gnuplotPipe, "%f %f\n", p->x, p->y);
    }
    
    p = &(inst->points[sol->path[0]]);

    fprintf(gnuplotPipe, "%f %f\n", p->x, p->y); /* connect last to first */

    fprintf(gnuplotPipe, "e\n");/* To close the list of nodes for gnuplot */
	fprintf(gnuplotPipe, "pause mouse close\n");

}/* plotNodes */

/*
* IP inst tsp instance
* IP sol solution to plot
* IP title plot title
* OV hamiltonian path
*/
void plotSolutionTitle(const TSPInstance* inst, const TSPSolution* sol, const char title[]){
    
    FILE *gnuplotPipe = popen("gnuplot -persist > ./gnuplot_out/gnuplot.out 2> ./gnuplot_out/gnuplot_err.out", "w");
	
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
    
	printf("Solution cost: %lf\n\n", (*sol).val);
    plotSolutionTitle(inst, sol, DEFAULT_TITLE);

}/* plotSolution */

/*
* OP gnuplotPipe pipe pointer
* IP title plot title
*/
void costPlotSettings(FILE* gnuplotPipe, const char title[]){
    
    fprintf(gnuplotPipe, "set term qt font \"Arial\"\n");
	fprintf(gnuplotPipe, "set title '%s'\n", title);
    fprintf(gnuplotPipe, "set xlabel 'Iteration'\n");
    fprintf(gnuplotPipe, "set ylabel 'Cost'\n");
	fprintf(gnuplotPipe, "plot '-' using 1:2 with linespoints pointtype 7 pointsize 1 linewidth 2 notitle\n");

}/* costPlotSettings */

/*
* Initializes pipe which plots solutions costs in real time.
* IP title title of the plot
* OP gnuplotPipe pipe pointer
* OP gnuplotPipe gnuplot pipe opened
*/
void initCostPlotPipe(const char title[], FILE** gnuplotPipe){

    *gnuplotPipe = popen("gnuplot -persist > ./gnuplot_out/gnuplot_cost.out 2> ./gnuplot_out/gnuplot_cost_err.out", "w");

    if (gnuplotPipe == NULL){
        fprintf(stderr, "Error opening gnuplot pipe\n");
		return;
	}

    costPlotSettings(*gnuplotPipe, title);

}/* initCostPlotPipe */

/*
* Adds current sol cost in real time plot.
* IP pipe gnuplot pipe that reads costs from $costFile
* IP iter iteration at which cost is found
* IP cost cost of current solution
*/
void addCost(FILE* gnuplotPipe, int iter, double cost){

    fprintf(gnuplotPipe, "%d %lf\n", iter, cost);

}/* plotCost */

/*
* IP gnuplotPipe pipe to be closed
*/
void closeGnuplotPipe(FILE* gnuplotPipe){
    fprintf(gnuplotPipe, "e\n");
    fflush(gnuplotPipe);
    pclose(gnuplotPipe);
}/* closePipe */
