/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : cplex.c
*/

#include "cplex.h"
#include "../../tsp.h"
#include <string.h>
#include <assert.h>
#include <ilcplex/cplex.h>

#define MODEL_NAME "TSP model version 1"
#define UNKNOWN_ERROR_MESSAGE "Unknown error code."
#define UNKNOWN_STATUS_MESSAGE "Unknown status code."
#define OUTPUT_MODEL_FILE "./tsp/output/cplex/model.lp"
#define BUFFER_SIZE 4096
#define PARAM_NODELIM 1000
#define MAX_LINE_LENGTH 133 /* 132 characters + '\n' */
#define MAX_OUTPUT_MODEL_FILE_DIMENSION 11

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
 * IP stre string containing the error informations
 * IP ec error code
 * IP env CPX environment, can be NULL to be able to translate CPXopenCPLEX routine errors
 * IP lp CPLEX linear program
 * OV Error details
 */
void print_error(const char *stre, int ec, CPXENVptr env, CPXLPptr lp)
{
	int s;
	char buffer[BUFFER_SIZE]; /* The buffer must be at least 4096 characters to hold the error string */

	getErrorMessage(env, ec, buffer);

	printf("\n\nERROR: %s\nCODE: %d\nMESSAGE: %s", stre, ec, buffer);

	s = getStatus(env, lp, buffer);

	printf("\n\nSTATUS CODE: %d\nMESSAGE: %s\n\n", s, buffer);

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
 * IP set settings
 * OP env CPLEX environment
 * Reference: https://www.ibm.com/docs/en/icos/20.1.0?topic=optimizer-terminating-mip-optimization
 */
void setCPXParameters(const Settings *set, CPXENVptr env)
{

	CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF);

	if ((*set).v)
		CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON); /* CPLEX output on screen */

	CPXsetintparam(env, CPX_PARAM_RANDOMSEED, (*set).seed);
	CPXsetintparam(env, CPX_PARAM_NODELIM, PARAM_NODELIM);
	CPXsetdblparam(env, CPX_PARAM_TILIM, (*set).tl);

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
	int err;

	setCPXParameters(set, env);

	if ((err = build(inst, env, lp)))
		return err;

	if ((*set).v && (*inst).dimension < MAX_OUTPUT_MODEL_FILE_DIMENSION)
		CPXwriteprob(env, lp, OUTPUT_MODEL_FILE, NULL);

	return 0;

} /* build_model */

/*
 * IP set settings
 * IP inst input instance
 * OP sol solution to evaluate ( assume that the solution was already initialized )
 * OR 0 if no error, error code otherwise
 * OV error message if any
 */
int optimize(const Settings *set, const TSPInstance *inst, TSPSolution *sol)
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
				return 0;
				// err = run_exact(set, inst, env, lp, sol);

			CPXfreeprob(env, &lp);
			CPXcloseCPLEX(&env);

			return err;

		} /* else */

	} /* else */

} /* optimize */
