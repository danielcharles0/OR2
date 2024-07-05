/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : cplex.c
*/

#include <string.h>
#include <assert.h>

#include "cplex.h"
#include "../../tsp.h"
#include "benders/benders.h"
#include "usercut/usercut.h"
#include "candidate/candidate.h"
#include "matheuristics/hardfixing/hardfixing.h"
#include "matheuristics/localbranching/localbranching.h"

#include "../../utility/utility.h"
#include "../../lib/fischetti/fischetti.h"
#include "../nearestneighbor/nearestneighbor.h"
#include "../refinement/2opt/2opt.h"

#include "../../output/output.h"

#define MODEL_NAME "TSP model version 1"
#define UNKNOWN_ERROR_MESSAGE "Unknown error code."
#define UNKNOWN_STATUS_MESSAGE "Unknown status code."
#define OUTPUT_MODEL_FILE "./tsp/output/cplex/model.lp"
#define OUTPUT_LOG_FILE "./cplex_out/cplex.log"
#define BUFFER_SIZE 4096
#define PARAM_NODELIM 1000
#define MAX_LINE_LENGTH 133 /* 132 characters + '\n' */
#define MAX_OUTPUT_MODEL_FILE_DIMENSION 11
#define MIP_TIMELIMIT_FRACTION 10.

/*
 * IP env CPX environment, can be NULL to be able to translate CPXopenCPLEX routine errors
 * IP ec error code
 * OP buffer on which we write the error message
 */
void getErrorMessage(CPXENVptr env, int ec, char *buffer)
{

	CPXCCHARptr errstr;

	errstr = CPXgeterrorstring(env, ec, buffer);

	if (errstr == NULL)
		strcpy(buffer, UNKNOWN_ERROR_MESSAGE);

} /* getErrorMessage */

/*
 * IP env CPX environment, can be NULL to be able to translate CPXopenCPLEX routine errors
 * IP lp CPLEX linear program
 * OP buffer on which we write the error message
 */
int getStatus(CPXENVptr env, CPXLPptr lp, char *buffer)
{

	CPXCHARptr errstr;
	int s = CPXgetstat(env, lp);

	errstr = CPXgetstatstring(env, s, buffer);

	if (errstr == NULL)
		strcpy(buffer, UNKNOWN_STATUS_MESSAGE);

	return s;

} /* getStatus */

/*
 * IP env CPLEX environment, can be NULL to be able to translate CPXopenCPLEX routine errors
 * IP lp CPLEX linear program
 * OV Error details and flushs
 */
void print_status(CPXENVptr env, CPXLPptr lp)
{
	int s;
	char buffer[BUFFER_SIZE]; /* The buffer must be at least 4096 characters to hold the error string */

	s = getStatus(env, lp, buffer);

	printf("\n\nSTATUS CODE: %d\nMESSAGE: %s\n\n", s, buffer);

	fflush(NULL); 

} /* print_status */

/*
 * IP stre string containing the error informations
 * IP ec error code
 * IP env CPLEX environment, can be NULL to be able to translate CPXopenCPLEX routine errors
 * IP lp CPLEX linear program
 * OV Error details
 */
void print_error(const char *stre, int ec, CPXENVptr env, CPXLPptr lp)
{
	char buffer[BUFFER_SIZE]; /* The buffer must be at least 4096 characters to hold the error string */

	getErrorMessage(env, ec, buffer);

	printf("\n\nERROR: %s\nCODE: %d\nMESSAGE: %s", stre, ec, buffer);

	print_status(env, lp);

} /* print_error */

/*
* IP i index of a node
* IP j index of a node
* IP inst tsp instance
* OP int position of the variable associated to edge (i,j) in the cplex matrix
*/
int xpos(int i, int j, const TSPInstance* inst){ 

	if ( i == j ) 
		return -1;

	if ( i > j ) 
        return xpos(j,i,inst);

	return (i * inst->dimension + j - (( i + 1 ) * ( i + 2 )) / 2);

}/* xpos */

/*
* IP param parameter to set
* IP val integer value of the parameter $param
* OP env CPLEX environment
* OP lp CPLEX linear program
*/
int setintparam(int param, int val, CPXENVptr env, CPXLPptr lp){
	
	int err;
	char errStr[MAX_LINE_LENGTH];
	
	if(!(err = CPXsetintparam(env, param, val)))
		return 0;
	
	sprintf(errStr, "Wrong CPXsetintparam(%d:%d)", param, val);

	print_error(errStr, err, env, lp);
	
	return err;

}/* setintparam */

/*
* IP param parameter to set
* IP val double value of the parameter $param
* OP env CPLEX environment
* OP lp CPLEX linear program
* OR 0 if no error, CPLEX error code otherwise
*/
int setdblparam(int param, double val, CPXENVptr env, CPXLPptr lp){
	
	int err;
	char errStr[MAX_LINE_LENGTH];
	
	if(!(err = CPXsetdblparam(env, param, val)))
		return 0;
	
	sprintf(errStr, "Wrong CPXsetdblparam(%d:%lf)", param, val);

	print_error(errStr, err, env, lp);
	
	return err;

}/* setdblparam */

/*
* OP env CPLEX environment
* OP lp CPLEX linear program
*/
int setlogfilename(CPXENVptr env, CPXLPptr lp){
	
	int err;
	char errStr[MAX_LINE_LENGTH];
	
	if(!(err = CPXsetlogfilename(env, OUTPUT_LOG_FILE, "w")))
		return 0;
	
	sprintf(errStr, "Wrong CPXsetlogfilename(%s:%s)", OUTPUT_LOG_FILE, "w");

	print_error(errStr, err, env, lp);
	
	return err;

}/* setdblparam */

/*
 * IP set settings
 * OP env CPLEX environment
 * OP lp CPLEX linear program
 * OR 0 if no error, error code otherwise
 * OV error message if any
 * Reference: https://www.ibm.com/docs/en/icos/20.1.0?topic=optimizer-terminating-mip-optimization
 */
int setCPXParameters(const Settings *set, CPXENVptr env, CPXLPptr lp)
{
	int err;

	if((err = setintparam(CPX_PARAM_SCRIND, CPX_OFF, env, lp)))
		return err;
	
	if((err = setintparam(CPX_PARAM_CLONELOG, -1, env, lp)))
		return err;

	if ((*set).v)
		if((err = setlogfilename(env, lp)))
			return err;
		/*CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON); TO BE CONVERTED TO setintparam TO ACTIVATE*/ /* CPLEX output on screen */

	if((err = setintparam(CPX_PARAM_RANDOMSEED, (*set).seed, env, lp)))
		return err;
	/*CPXsetintparam(env, CPX_PARAM_NODELIM, PARAM_NODELIM); TO BE CONVERTED TO setintparam TO ACTIVATE*/
	if((err = setdblparam(CPX_PARAM_TILIM, (*set).tl, env, lp)))
		return err;
	
	return 0;

} /* setCPXParameters */

/*
 * IP inst input instance
 * IP env CPLEX environment
 * OP lp CPLEX linear program
 * OR 0 if no error, error code otherwise
 * OV error message if any
 * The method adds the column to che CPLEX model
 */
int build_columns(const TSPInstance *inst, CPXENVptr env, CPXLPptr lp)
{
	/* edge variable name */
	char **en = malloc(sizeof(char *));
	assert(en != NULL);

	*en = malloc(MAX_LINE_LENGTH * sizeof(char));
	assert(*en != NULL);

	for (int i = 0; i < (*inst).dimension; i++)
		for (int j = i + 1; j < (*inst).dimension; j++)
		{
			int err;

			const double lb = 0.0, ub = 1.0;
			const char binary = 'B';
			double val;

			sprintf(*en, "(%d,%d)", i + 1, j + 1);

			/* coefficient of the variable in the objective function (here we are building the c_t array) */
			val = getDist(j, i, inst);

			if ((err = CPXnewcols(env, lp, 1, &val, &lb, &ub, &binary, en)))
			{
				print_error("Wrong CPXnewcols(..)", err, env, lp);
				return err;
			} /* if */

		} /* for */

	free(*en);
	free(en);

	return 0;

} /* build_columns */

/*
 * IP inst input instance
 * IP env CPLEX environment
 * OP lp CPLEX linear program
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int build_constraints(const TSPInstance *inst, CPXENVptr env, CPXLPptr lp)
{
	int *idxs;
	double *vls;
	/* constraint name */
	char **en;

	en = malloc(sizeof(char *));
	assert(en != NULL);

	*en = malloc(MAX_LINE_LENGTH * sizeof(char));
	assert(*en != NULL);

	idxs = malloc(((*inst).dimension - 1) * sizeof(int));
	assert(idxs != NULL);

	vls = malloc(((*inst).dimension - 1) * sizeof(double));
	assert(vls != NULL);

	for (int h = 0; h < (*inst).dimension; h++)
	{
		int nnz = 0, i, err;
		const int zero = 0;
		const double rhs = 2.0;
		const char sense = 'E';

		for(i = 0; i < (*inst).dimension; i++)
			if(i != h){
	        	idxs[nnz] = xpos(i, h, inst);
				vls[nnz] = 1.0;
				nnz++;
			}/* if */

		sprintf(*en, "degree(%d)", h + 1);

		if ((err = CPXaddrows(env, lp, 0, 1, (*inst).dimension - 1, &rhs, &sense, &zero, idxs, vls, NULL, en)))
		{
			print_error("Wrong CPXaddrows(..)", err, env, lp);
			return err;
		} /* if */

	} /* for */

	free(vls);
	free(idxs);
	free(*en);
	free(en);

	return 0;

} /* build_constraints */

/*
 * IP inst input instance
 * IP env CPLEX environment
 * OP lp CPLEX linear program
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int build(const TSPInstance *inst, CPXENVptr env, CPXLPptr lp)
{
	int err;

	if ((err = build_columns(inst, env, lp)))
		return err;

	return build_constraints(inst, env, lp);

	return 0;

} /* build */

/*
 * IP set settings
 * IP inst input instance
 * OP env CPLEX environment
 * OP lp CPLEX linear program
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int build_model(const Settings *set, const TSPInstance *inst, CPXENVptr env, CPXLPptr lp)
{
	int err = 0;

	setCPXParameters(set, env, lp);

	if ((err = build(inst, env, lp)))
		return err;

	if ((*set).v && (*inst).dimension < MAX_OUTPUT_MODEL_FILE_DIMENSION)
		if((err = CPXwriteprob(env, lp, OUTPUT_MODEL_FILE, NULL)))
			print_error("Wrong CPXwriteprob(..)", err, env, lp);

	return err;

} /* build_model */

/*
* Prints exact algorithms legend.
*/
void exactAlgorithmLegend(){

	printf("Available exact algorithms:\n");
    printf("\t- Code: %d, Algorithm: Benders' loop\n", BENDERS);
	printf("\t- Code: %d, Algorithm: Benders' loop with patch\n", BENDERS_PATCH);
	printf("\t- Code: %d, Algorithm: Branch and Cut with Candidate Callback\n", CANDIDATE_CALLBACK);
	printf("\t- Code: %d, Algorithm: Branch and Cut with User-cut Callback\n", USERCUT_CALLBACK);
    printf("\n");

}/* exactAlgorithmLegend */

/*
* Installs the callbacks and runs the solver.
*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* IP callback_installer
* IOP sol solution to be updated
* IP warm_start true if MIPSTART is required
* OP et execution time in seconds
* OR 0 if no error, error code otherwise
*/
int callback_solver(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, callback_installer ci, TSPSolution* sol, bool warm_start, double* et){

    int err = 0;
    clock_t start = clock();

    COMP comp;
	TSPSSolution temp;
    CPXInstance cpx_inst;

    if(warm_start){
        if((err = mip_start(set, inst, env, lp)))
            return err;
        update_time_limit(set, start, env, lp);
    }/* if */

	ci(env, lp, &cpx_inst);

    allocCPXInstance(&cpx_inst, set, inst, env, lp);
	allocComp(inst->dimension, &comp);
	allocSSol(inst->dimension, &temp);

    optimize_model(inst, env, lp, &temp, &comp);

    convertSSol(inst, &temp, sol);
    
	freeSSol(&temp);
    freeComp(&comp);
	freeCPXInstance(&cpx_inst);

	*et = getSeconds(start);

    return 0;

}/* callback_solver */

/*
* OR the Hard fixing fef hyperparamether
*/
double readHFFef(){

	double fef;
	
	do{

		fef = readDouble("Insert the fixed edges fraction (0, 1): ");
		
		if(fef <= 0 || fef >= 1)
			printf("Error: Please insert a valid value!\n\n");
	
	}while(fef <= 0 || fef >= 1);

	return fef;

}/* readHFFnf */

/*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* IP start boolean indicating whether to use mipstart
* IP alg algorithm to run
* IP fef fixed edges fraction, just for hard fixing, ignored in the other cases
* IOP sol solution to be updated
* OP et execution time
* OR 0 if no error, error code otherwise
*/
int run_exact_offline(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, bool start, EXACTS alg, double fef, TSPSolution* sol, double* et){
	
	switch (alg){
	    case BENDERS:
	        return benders(set, inst, env, lp, sol, (patchfunc)dummypatch, start, et);
		case BENDERS_PATCH:
			return benders(set, inst, env, lp, sol, (patchfunc)patch, start, et);
		case CANDIDATE_CALLBACK:
			return callback_solver(set, inst, env, lp, (callback_installer)candidate, sol, start, et);
		case USERCUT_CALLBACK:
			return callback_solver(set, inst, env, lp, (callback_installer)usercut, sol, start, et);
		case _HARD_FIXING:
			return hard_fixing(set, inst, fef, env, lp, sol, et);
		case _LOCAL_BRANCHING:
			return local_branching(set, inst, env, lp, sol, et);
	    default:
	        printf("Error: Exact algorithm code not found.\n\n");
	        return 1;
    }/* switch */

}/* run_exact_offline */

/*
 * IP set settings
 * IP start execution time
 * OP env CPLEX environment
 * IP lp CPLEX linear program
 */
void update_time_limit(const Settings *set, clock_t start, CPXENVptr env, CPXLPptr lp)
{
	double ntl = step((*set).tl - getSeconds(start));

	if(setdblparam(CPX_PARAM_TILIM, ntl, env, lp))
		exit(1);

} /* update_time_limit */

/*
* IP n number of nodes of the instance
* IP comp components array to initialize
*/
void allocComp(int n, COMP* comp){

	(*comp).nc = n;

	(*comp).map = malloc(n * sizeof(int));
	assert((*comp).map != NULL);

}/* allocComp */

/*
* IOP comp components array to free memory
*/
void freeComp(COMP *comp){
    free((*comp).map);
}/* freeComp */

/*
 * This function will build the TSP successors solution starting from the given CPLEX xstar solution
 * 
 * IP inst input instance
 * IP xstar solution found by CPLEX
 * OP sol solution to evaluate ( assume that the solution was already initialized )
 * OP comp array of components assumed to be initialized. It can be NULL if we don't need to compute the components.
 */
void build_sol_xstar(const TSPInstance *inst, const double *xstar, TSPSSolution *sol, COMP *comp){
	
	instance finst = { .nnodes = (*inst).dimension };
	
	build_sol_fischetti(xstar, &finst, (*sol).succ, (*comp).map, &((*comp).nc));
	
	(*sol).val = getSSolCost(inst, sol);

}/* build_sol_xstar */

/*
 * IP inst input instance
 * IP env CPLEX environment
 * IP lp CPLEX linear program
 * OP sol solution to evaluate ( assume that the solution was already initialized )
 * OP comp array of components assumed to be initialized. It can be NULL if we don't need to compute the components.
 * OR false if no error, true otherwise
 */
int build_sol(const TSPInstance *inst, CPXENVptr env, CPXLPptr lp, TSPSSolution *sol, COMP *comp){

	int err, ncols = CPXgetnumcols(env, lp);
	double *xstar;

	xstar = malloc(ncols * sizeof(double));
	assert(xstar != NULL);

	if ((err = CPXgetx(env, lp, xstar, 0, ncols - 1)))
	{
		print_error("CPXgetx() error", err, env, lp);
		return err;
	} /* if */

	build_sol_xstar(inst, xstar, sol, comp);

	free(xstar);

	return 0;

}/* build_sol */

/*
* IP sol solution
* IP lb lower bound
* OR the % gap between the solution cost and the lower bound
*/
double solGap(const TSPSolution* sol, double lb){

	return ((sol->val - lb) / sol->val) * 100;

}/* solGap */

/*
 * IP inst input instance
 * IP env CPLEX environment
 * IP lp CPLEX linear program
 * OP sol solution to evaluate ( assume that the solution was already initialized )
 * OP comp array of components assumed to be initialized. It can be NULL if we don't need to compute the components.
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int optimize_model(const TSPInstance *inst, CPXENVptr env, CPXLPptr lp, TSPSSolution *sol, COMP *comp)
{

	int err;

	if ((err = CPXmipopt(env, lp) /* optimization */))
		print_error("CPXmipopt() error", err, env, lp);
	else
		err = build_sol(inst, env, lp, sol, comp);

	return err;

} /* optimize_model */

/*
 * IP set settings
 * IP inst input instance
 * IP start boolean indicating whether to use mipstart
 * IP alg algorithm to run
 * IP fef fixed edges fraction, just for hard fixing, ignored in the other cases
 * OP sol solution to evaluate ( assume that the solution was already initialized )
 * OP et execution time
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int optimize_offline(const Settings *set, const TSPInstance *inst, bool start, EXACTS alg, double fef, TSPSolution *sol, double* et)
{

	int err;
	CPXENVptr env;

	env = CPXopenCPLEX(&err);

	if (err)
	{ /* err is 0 if no error for the CPLEX lib */
		print_error("CPXopenCPLEX() error", err, env, NULL);
		return err;
	}
	else
	{
		CPXLPptr lp = CPXcreateprob(env, &err, MODEL_NAME);

		if (err)
		{
			print_error("CPXcreateprob() error", err, env, lp);
			CPXcloseCPLEX(&env);
			return err;
		}
		else
		{
			if (!(err = build_model(set, inst, env, lp)))
				if(!(err = run_exact_offline(set, inst, env, lp, start, alg, fef, sol, et)))
					if((*set).v)
						print_status(env, lp);

			CPXfreeprob(env, &lp);
			CPXcloseCPLEX(&env);

			return err;

		} /* else */

	} /* else */

} /* optimize_offline */

/*
 * IP set settings
 * IP inst input instance
 * OP sol solution to evaluate ( assume that the solution was already initialized )
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int optimize(const Settings *set, const TSPInstance *inst, TSPSolution *sol)
{
	double et;

	bool start = readBool("Would you like a MIP start? (yes/no): ");
	printf("\n");

	exactAlgorithmLegend();

	return optimize_offline(set, inst, start, (EXACTS)readInt("Insert the code of the exact algorithm you want to run: "), 0, sol, &et);

} /* optimize */

/*
* IP cc concurrency degree
* IP n length of the array
* OP arr int array to allocate
*/
void allocCPXInstanceIntArray(int cc, int n, int*** arr){
	
	int i;

	*arr = malloc(cc * sizeof(int*));
	assert(*arr != NULL);

	for(i = 0; i < cc; i++){
		(*arr)[i] = malloc(n * sizeof(int));  
    	assert((*arr)[i] != NULL);
	}/* for */

}/* allocCPXInstanceIntArray */

/*
* IP cc concurrency degree
* IP n length of the array
* OP arr double array to allocate
*/
void allocCPXInstanceDoubleArray(int cc, int n, double*** arr){
	
	int i;

	*arr = malloc(cc * sizeof(double*));
	assert(*arr != NULL);

	for(i = 0; i < cc; i++){
		(*arr)[i] = malloc(n * sizeof(double));  
    	assert((*arr)[i] != NULL);
	}/* for */

}/* allocCPXInstanceIntArray */

/*
* IP cc concurrency degree
* IP n size of the instance
* OP ssols array of solutions to allocate
*/
void allocCPXInstanceSSol(int cc, int n, TSPSSolution** ssols){
	
	int i;

	*ssols = malloc(cc * sizeof(TSPSSolution));
	assert(*ssols != NULL);

	for(i = 0; i < cc; i++)
		allocSSol(n, &((*ssols)[i]));

}/* allocCPXInstanceSSol */

/*
* IP cc concurrency degree
* IP n size of the instance
* OP sols array of solutions to allocate
*/
void allocCPXInstanceSol(int cc, int n, TSPSolution** sols){
	
	int i;

	*sols = malloc(cc * sizeof(TSPSSolution));
	assert(*sols != NULL);

	for(i = 0; i < cc; i++)
		allocSol(n, &((*sols)[i]));

}/* allocCPXInstanceSol */

/*
* IP cc concurrency degree
* IP n size of the instance
* OP comps array of components to allocate
*/
void allocCPXInstanceComp(int cc, int n, COMP** comps){
	
	int i;

	*comps = malloc(cc * sizeof(COMP));
	assert(*comps != NULL);

	for(i = 0; i < cc; i++)
		allocComp(n, &((*comps)[i]));

}/* allocCPXInstanceComp */

/*
* Initializes CPXinstance passed in input
*
* OP cpx_inst cplex instance to be initialized
* IP tsp_inst tsp instance to be passed to cplex instance
* IP temp temporary succ solution to be used in callbacks
* IP env cplex environment
* IP lp cplex lp
*/
void allocCPXInstance(CPXInstance* cpx_inst, const Settings* set, const TSPInstance* tsp_inst, CPXENVptr env, CPXLPptr lp){

	int i, cc = get_hardware_concurrency();
	
	cpx_inst->ncols = CPXgetnumcols(env, lp);
	cpx_inst->inst = tsp_inst;
	cpx_inst->env = env;
	cpx_inst->lp = lp;
	cpx_inst->set = set;
	
	cpx_inst->indices = malloc((*cpx_inst).ncols * sizeof(int));
	assert(cpx_inst->indices != NULL);
	
	for(i = 0; i < (*cpx_inst).ncols; i++)
		cpx_inst->indices[i] = i;
	
	allocCPXInstanceIntArray(cc, cpx_inst->inst->dimension, &(cpx_inst->usercut_comps));
	
	allocCPXInstanceIntArray(cc, 2 * cpx_inst->ncols, &(cpx_inst->elist));
	
	allocCPXInstanceDoubleArray(cc, cpx_inst->ncols, &(cpx_inst->xstars));
	
	allocCPXInstanceIntArray(cc, cpx_inst->ncols, &(cpx_inst->sec_idxs));
	
	allocCPXInstanceDoubleArray(cc, cpx_inst->ncols, &(cpx_inst->sec_vls));
	
	allocCPXInstanceSSol(cc, tsp_inst->dimension, &((*cpx_inst).ssols));
	
	allocCPXInstanceSol(cc, tsp_inst->dimension, &((*cpx_inst).sols));
	
	allocCPXInstanceComp(cc, tsp_inst->dimension, &((*cpx_inst).comps));
	
}/* allocCPXInstance */

/*
* IP cc concurrency degree
* OP arr double array to free
*/
void freeCPXInstanceDoubleArray(int cc, double** arr){
	
	int i;

	for(i = 0; i < cc; i++)
		free(arr[i]);

	free(arr);

}/* freeCPXInstanceDoubleArray */

/*
* IP cc concurrency degree
* OP arr int array to free
*/
void freeCPXInstanceIntArray(int cc, int** arr){
	
	int i;

	for(i = 0; i < cc; i++)
		free(arr[i]);

	free(arr);

}/* freeCPXInstanceIntArray */

/*
* IP cc concurrency degree
* OP comps components array to free
*/
void freeCPXInstanceComp(int cc, COMP* comps){
	
	int i;

	for(i = 0; i < cc; i++)
		freeComp(&(comps[i]));

	free(comps);

}/* freeCPXInstanceComp */

/*
* IP cc concurrency degree
* OP sols solutions array to free
*/
void freeCPXInstanceSol(int cc, TSPSolution* sols){
	
	int i;

	for(i = 0; i < cc; i++)
		freeSol(&(sols[i]));

	free(sols);

}/* freeCPXInstanceSol */

/*
* IP cc concurrency degree
* OP ssols solutions array to free
*/
void freeCPXInstanceSSol(int cc, TSPSSolution* ssols){
	
	int i;

	for(i = 0; i < cc; i++)
		freeSSol(&(ssols[i]));

	free(ssols);

}/* freeCPXInstanceSSol */

/*
* IP cpx_inst instance to free
*/
void freeCPXInstance(CPXInstance* cpx_inst){
	
	int cc = get_hardware_concurrency();

	freeCPXInstanceComp(cc, (*cpx_inst).comps);

	freeCPXInstanceSol(cc, (*cpx_inst).sols);

	freeCPXInstanceSSol(cc, (*cpx_inst).ssols);

	freeCPXInstanceDoubleArray(cc, cpx_inst->sec_vls);

	freeCPXInstanceIntArray(cc, cpx_inst->sec_idxs);

	freeCPXInstanceDoubleArray(cc, cpx_inst->xstars);
	
	freeCPXInstanceIntArray(cc, cpx_inst->elist);

	freeCPXInstanceIntArray(cc, cpx_inst->usercut_comps);

	free(cpx_inst->indices);

}/* freeCPXInstance */

/*
* Generates a solution with Nearest Neighbor + 2opt algorithms
*
* IP set settings
* IP inst tsp instance
* IP env CPLEX env
* IP lp CPLEX lp
*/
int mip_start(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp){

	Settings mip_start_set;
	TSPSolution sol;

	cpSet(set, &mip_start_set);

	allocSol(inst->dimension, &sol);

	if((*set).v)
		printf("Running nnbs\n");

	mip_start_set.tl = (set->tl / 10) - best_start(set, inst, &sol);
	
	if((*set).v)
		printf("Running 2opt\n");

	mip_start_set.tl -= opt2(set, inst, &sol);

	if(mip_start_set.tl < 0){
		printf("mip_start exceeded time limit. No starting solution set.\n");
		freeSol(&sol);
		return 0;
	}/* if */

	if(checkSol(inst, &sol)){

		int effortlevel = CPX_MIPSTART_NOCHECK;  
		int beg = 0, nnz = 0, err = 0;
		double* values;
		int* indices;

		indices = (int *) malloc(inst->dimension * sizeof(int));
		assert(indices != NULL);

		values = (double*) malloc(inst->dimension * sizeof(double));
		assert(values != NULL);

		for (int i = 0; i < inst->dimension; i++) {
	        indices[nnz] = xpos(sol.path[i], sol.path[(i + 1) % inst->dimension], inst);
			values[nnz++] = 1.0;
	    }/* for */

		if ((err = CPXaddmipstarts(env, lp, 1, nnz, &beg, indices, values, &effortlevel, NULL))){
			print_error("CPXaddmipstarts() error", err, env, lp);	
			return err;
		}/* if */
		
		free(values);
		free(indices);

		if((*set).v)
			printf("\n\nMIP start added successfully.\n\n");

	} else 
		printf("\n\nMIP start not added: invalid solution.\n\n");
	
	freeSol(&sol);

	return 0;

}/* mip_start */

/*
* IP inst tsp instance
* IP sol TSP path solution to be converted into CPLEX format
* IP cpx_ncols number of columns of the cplex model
* OP cpx_cols values of the CPLEX variables assumed to be initialized
*/
void convertCPXSol(const TSPInstance* inst, const TSPSolution* sol, int cpx_ncols, double* cpx_cols){
	
	int i;

	for(i = 0; i < cpx_ncols; i++)
		cpx_cols[i] = 0.0;

	for(i = 0; i < (*inst).dimension; i++){
		int idx = xpos((*sol).path[i], (*sol).path[(i + 1) % (*inst).dimension], inst);
		cpx_cols[idx] = 1.0;
	}/* for */

}/* convertCPXSol */

/*
* This method will post the solution to CPX
*
* IOP cpx_inst CPLEX instance
* IP ctx CPLEX callback context, handled internally by CPLEX
* IOP sol TSP path solution to be refined and posted
*/
void postSol2CPX(CPXInstance* cpx_inst, CPXCALLBACKCONTEXTptr ctx, TSPSolution* sol){
	
	int err, thread_id;
	double* cpx_cols;

	if(!checkSol((*cpx_inst).inst, sol)){
		if((*(*cpx_inst).set).v)
			printf("\n\nInvalid solution! Exiting at postSol().\n");
		exit(1);
	}/* if */

	CPXcallbackgetinfoint(ctx, CPXCALLBACKINFO_THREADID, &thread_id);

	cpx_cols = cpx_inst->xstars[thread_id];

	convertCPXSol((cpx_inst)->inst, sol, cpx_inst->ncols, cpx_cols);

	if((err = CPXcallbackpostheursoln(ctx, cpx_inst->ncols, cpx_inst->indices, cpx_cols, (*sol).val, CPXCALLBACKSOLUTION_NOCHECK))){
		print_error("CPXcallbackpostheursoln() error", err, cpx_inst->env, cpx_inst->lp);
		exit(1);
	}/* if */

}/* postSol2CPX */

/*
* This method will patch the solution found and post it
*
* IOP cpx_inst CPLEX instance
* IP ctx CPLEX callback context, handled internally by CPLEX
* IP comp components of the $sol
* IOP ssol TSP succ solution to be refined and posted
*/
void postPatchedSSol2CPX(CPXInstance* cpx_inst, CPXCALLBACKCONTEXTptr ctx, COMP* comp, TSPSSolution* ssol){

	int thread_id;
	TSPSolution* sol;

	CPXcallbackgetinfoint(ctx, CPXCALLBACKINFO_THREADID, &thread_id);

	sol = &(cpx_inst->sols[thread_id]);

	patch(cpx_inst->set, cpx_inst->inst, ssol, comp);

	convertSSol(cpx_inst->inst, ssol, sol);

	// opt2(cpx_inst->set, cpx_inst->inst, sol);

	postSol2CPX(cpx_inst, ctx, sol);

}/* postPatchedSSol2CPX */

/*
* Print algorithm configurations.
*/
void matheuristics_legend(){
    
    printf("Available MATHEURISTICS:\n");
    printf("\t- Code: %d, Algorithm: Hard fixing\n", HARD_FIXING);
    printf("\t- Code: %d, Algorithm: Local branching\n", LOCAL_BRANCHING);
    printf("\n");
    
}/* algorithmLegend */

/*
* IP alg matheuristic algorithm to run
* IP set settings
* IP inst tsp instance
* OP sol solution
* OR double execution seconds, -1 if error
*/
double runMatheurConfiguration(MATHEURISTICS alg, const Settings* set, const TSPInstance* inst, TSPSolution* sol){

	int err;
	double et;

	switch (alg){
	    case HARD_FIXING:
			if((err = optimize_offline(set, inst, false, _HARD_FIXING, readHFFef(), sol, &et)))
				return -1;
			return et;
	    case LOCAL_BRANCHING:
	        if((err = optimize_offline(set, inst, false, _LOCAL_BRANCHING, 0, sol, &et)))
				return -1;
			return et;
	    default:
	        printf("Error: Algorithm code not found.\n\n");
	        break;
    }/* switch */
	
	return -1;

}/* runConfiguration */

/*
* IP set settings
* IP inst tsp instance
* IOP sol solution
* OP false if found a valid solution, true otherwise.
* OR int execution seconds, -1 if error
*/
int matheur(const Settings* set, const TSPInstance* inst, TSPSolution* sol){

	matheuristics_legend();

	return runMatheurConfiguration(readInt("Insert the configuration code you want to run: "), set, inst, sol);

}/* matheur */

/*
 * IP set settings
 * IP start execution time
 * OP mipset settings to update timelimit
 * OP env CPLEX environment
 * OP lp CPLEX linear program
 */
void update_solver_time_limit(const Settings* set, clock_t start, Settings* mipset, CPXENVptr env, CPXLPptr lp)
{
	double /* remining time */ rt = step((*set).tl - getSeconds(start));

	(*mipset).tl = min_dbl((*set).tl / MIP_TIMELIMIT_FRACTION, rt);

	if(setdblparam(CPX_PARAM_TILIM, (*mipset).tl, env, lp))
		exit(1);

} /* update_time_limit */
