/*
 * Project  : Travelling Salesman Problem implementations
 * Author   : Matteo Fischetti
 * File     : fischetti.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "fischetti.h"
// #include "../../algorithms/cplex/cplex.h"

void print_error_fischetti(const char *err) 
{ 
	printf("\n\n ERROR: %s \n\n", err); 
	fflush(NULL); 
	exit(1); 
}  

int xpos_fischetti(int i, int j, instance *inst) // to be verified
{
	if (i == j)
		print_error_fischetti(" i == j in xpos_fischetti");
	if (i > j)
		return xpos_fischetti(j, i, inst);
	int pos = i * inst->nnodes + j - ((i + 1) * (i + 2)) / 2;
	return pos;
}

void build_sol_fischetti(const double *xstar, instance *inst, int *succ, int *comp, int *ncomp)
{
	*ncomp = 0;
	for (int i = 0; i < inst->nnodes; i++)
	{
		succ[i] = -1;
		comp[i] = -1;
	}

	for (int start = 0; start < inst->nnodes; start++)
	{
		if (comp[start] >= 0)
			continue; // node "start" was already visited, just skip it

		// a new component is found
		(*ncomp)++;
		int i = start;
		int done = 0;
		while (!done) // go and visit the current component
		{
			comp[i] = *ncomp;
			done = 1;
			for (int j = 0; j < inst->nnodes; j++)
			{
				if (i != j && xstar[xpos_fischetti(i, j, inst)] > 0.5 && comp[j] == -1) // the edge [i,j] is selected in xstar and j was not visited before
				{
					succ[i] = j;
					i = j;
					done = 0;
					break;
				}
			}
		}
		succ[i] = start; // last arc to close the cycle

		// go to the next component...
	}
}
