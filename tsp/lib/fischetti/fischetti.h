/*
* Project  : Travelling Salesman Problem implementations
* Author   : Matteo Fischetti
* File     : fischetti.h
*/

typedef struct
{
	// input data
	int nnodes;
	double *xcoord;
	double *ycoord;
	int integer_costs; // = 1 for integer costs (rounded distances), 0 otherwise

	// parameters
	int model_type;
	double timelimit;	   // overall time limit, in sec.s
	char input_file[1000]; // input file

	// global data
	double zbest; // value of the best sol. available
} instance;

void build_sol_fischetti(const double*, instance*, int*, int*, int*);
