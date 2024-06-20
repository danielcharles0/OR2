/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : pprofile.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "pprofile.h"
#include "../utility/utility.h"
#include "../input/generator/generator.h"
#include "../algorithms/cplex/cplex.h"

#define NOF_TEST_INSTANCES 20
#define PPROF_OUT_FILE "./perfprof/pprof.csv"
#define PP_PLOT_EXACT_CMD "python3 ./perfprof/perfprof.py -D , -M 1.2 -T %d ./perfprof/pprof.csv ./perfprof/pprof.pdf -X \"Time Ratio\" -P \"TSP Performance Profile\""
#define PP_PLOT_HEUR_CMD "python3 ./perfprof/perfprof.py -D , -M 1.2 -T %d ./perfprof/pprof.csv ./perfprof/pprof.pdf -X \"Cost Ratio\" -P \"TSP Performance Profile\""
#define EXACT_METHODS_OFFSET PP___END_HEURISTIC

/*
* OV help section
*/
void pp_help(void){

    printf("Performance profile options:\n");

    printf("\t-s, --seed  <seed_value>\tseed used for random generation (integer value)\n");
    printf("\t-tl         <timelimit_value>\tset the execution time limit (in seconds) for an instance - default 30s\n");
	printf("\t-n, --nodes <number_of_nodes>\tnumber of nodes for the random instances\n");
    printf("\t-h  --help\t\t\tto reach this section\n");
    printf("\n");

    printf("Usage:\n");
    printf("\tYou must specify the number of nodes for a random generation.\n");
    printf("\tParameter <timelimit_value> accepts only strictly positive integer values\n");
    printf("\tParameter <seed_value> accepts only positive integer values\n");
    printf("\tParameter <number_of_nodes> accepts only integer values strictly greater than 2\n");
	printf("\tOption help must be specified itself\n");

}/* help */

/*
* IP set settings
* IP config input parameters configuration
* OR true if there is an error, false otherwise
* OV the help section if there is an error
*/
bool pp_validate(const Settings* set, CONF config){

	if(!(config == HELP || config == ERROR || (*set).v || (*set).input_file_name[0] != '\0'))
		return false;
	
	pp_help();

	return true;

}/* pp_validate */

/*
* IP code of the heuristic algorithm
* OR true if $code is a valid algorithm code, false otherwise
*/
bool validateHeurAlgCode(int code){

	if(code < 1 || code >= PP___END_HEURISTIC){
		printf("\nError: Invalid algorithm code!\n");
		return false;
	}/* if */
	
	return true;

}/* validateHeurAlgCode */

/*
* IP partial_code code without offset of the exact algorithm
* OR true if $code is a valid algorithm code, false otherwise
*/
bool validateExactAlgCode(int partial_code){

	int code = partial_code + EXACT_METHODS_OFFSET;

	if(partial_code < 1 || code >= PP___END_EXACT){
		printf("\nError: Invalid algorithm code!\n");
		return false;
	}/* if */
	
	return true;

}/* validateHeurAlgCode */

/*
* OV the random legend
*/
void pp_random_legend(void){

	printf("\t* Random\n");
    printf("\t\t- Code: %d, Algorithm: Just a random solution\n", PP_RANDOM);
    printf("\t\t- Code: %d, Algorithm: Random + 2OPT\n", PP_RANDOM_2OPT);
	printf("\t\t- Code: %d, Algorithm: Random + TABU\n", PP_RANDOM_TABU_CONST);
	printf("\t\t- Code: %d, Algorithm: Random + TABU with triangular tenure\n", PP_RANDOM_TABU_TRIANG);
	printf("\t\t- Code: %d, Algorithm: Random + TABU with square tenure\n", PP_RANDOM_TABU_SQUARE);
	printf("\t\t- Code: %d, Algorithm: Random + TABU with sawtooth tenure\n", PP_RANDOM_TABU_SAWTOO);
	printf("\t\t- Code: %d, Algorithm: Random + VNS\n", PP_RANDOM_VNS);
    printf("\n");

}/* pp_random_legend */

/*
* OV the nearest neighbor (start from first node version) legend
*/
void pp_nearest_neighbor_first_node_legend(void){

	printf("\t* Nearest neighbor starting from the first node\n");
    printf("\t\t- Code: %d, Algorithm: Just nearest neighbor starting from the first node\n", PP_NEAREST_NEIGHBOR_START_FIRST_NODE);
    printf("\t\t- Code: %d, Algorithm: Nearest neighbor + 2OPT\n", PP_NEAREST_NEIGHBOR_START_FIRST_NODE_2OPT);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU\n", PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_CONST);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU with triangular tenure\n", PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_TRIANG);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU with square tenure\n", PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_SQUARE);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU with sawtooth tenure\n", PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_SAWTOO);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + VNS\n", PP_NEAREST_NEIGHBOR_START_FIRST_NODE_VNS);
    printf("\n");

}/* pp_nearest_neighbor_first_node_legend */

/*
* OV the nearest neighbor (start from random node version) legend
*/
void pp_nearest_neighbor_random_node_legend(void){

	printf("\t* Nearest neighbor starting from a random node\n");
    printf("\t\t- Code: %d, Algorithm: Just nearest neighbor starting from a random node\n", PP_NEAREST_NEIGHBOR_START_RANDOM_NODE);
    printf("\t\t- Code: %d, Algorithm: Nearest neighbor + 2OPT\n", PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_2OPT);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU\n", PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_CONST);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU with triangular tenure\n", PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_TRIANG);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU with square tenure\n", PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_SQUARE);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU with sawtooth tenure\n", PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_SAWTOO);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + VNS\n", PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_VNS);
    printf("\n");

}/* pp_nearest_neighbor_random_node_legend */

/*
* OV the nearest neighbor (best start version) legend
*/
void pp_nearest_neighbor_best_start_legend(void){

	printf("\t* Nearest neighbor best start\n");
    printf("\t\t- Code: %d, Algorithm: Just nearest neighbor best start\n", PP_NEAREST_NEIGHBOR_BEST_START);
    printf("\t\t- Code: %d, Algorithm: Nearest neighbor + 2OPT\n", PP_NEAREST_NEIGHBOR_BEST_START_2OPT);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU\n", PP_NEAREST_NEIGHBOR_BEST_START_TABU_CONST);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU with triangular tenure\n", PP_NEAREST_NEIGHBOR_BEST_START_TABU_TRIANG);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU with square tenure\n", PP_NEAREST_NEIGHBOR_BEST_START_TABU_SQUARE);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + TABU with sawtooth tenure\n", PP_NEAREST_NEIGHBOR_BEST_START_TABU_SAWTOO);
	printf("\t\t- Code: %d, Algorithm: Nearest neighbor + VNS\n", PP_NEAREST_NEIGHBOR_BEST_START_VNS);
    printf("\n");

}/* pp_nearest_neighbor_best_start_legend */

/*
* OV the heuristic algorithms legend
*/
void pp_heuristic_legend(void){
	
	printf("Available heuristic algorithms:\n");

	pp_random_legend();
	
	pp_nearest_neighbor_first_node_legend();

	pp_nearest_neighbor_random_node_legend();

	pp_nearest_neighbor_best_start_legend();

}/* pp_heuristic_legend */

/*
* OV the exact methods legend
*/
void pp_exact_legend(void){

	printf("Available exact algorithms:\n");

	printf("\t* BENDERS\n");
    printf("\t\t- Code: %d, Algorithm: Benders loop\n", PP_BENDERS_NO_MIPSTART - EXACT_METHODS_OFFSET);
    printf("\t\t- Code: %d, Algorithm: Benders loop + MIPSTART\n", PP_BENDERS_MIPSTART - EXACT_METHODS_OFFSET);
	
	printf("\t* BENDERS PATCH\n");
    printf("\t\t- Code: %d, Algorithm: Benders patch\n", PP_BENDERS_PATCH_NO_MIPSTART - EXACT_METHODS_OFFSET);
    printf("\t\t- Code: %d, Algorithm: Benders patch + MIPSTART\n", PP_BENDERS_PATCH_MIPSTART - EXACT_METHODS_OFFSET);

	printf("\t* CANDIDATE CALLBACK\n");
    printf("\t\t- Code: %d, Algorithm: Candidate callback\n", PP_CANDIDATE_CALLBACK_NO_MIPSTART - EXACT_METHODS_OFFSET);
    printf("\t\t- Code: %d, Algorithm: Candidate callback + MIPSTART\n", PP_CANDIDATE_CALLBACK_MIPSTART - EXACT_METHODS_OFFSET);

	printf("\t* USERCUT CALLBACK\n");
    printf("\t\t- Code: %d, Algorithm: Usercut callback\n", PP_USERCUT_CALLBACK_NO_MIPSTART - EXACT_METHODS_OFFSET);
    printf("\t\t- Code: %d, Algorithm: Usercut callback + MIPSTART\n", PP_USERCUT_CALLBACK_MIPSTART - EXACT_METHODS_OFFSET);
    
	printf("\n");

}/* pp_exact_legend */

/*
* OV the legend
*/
void pp_legend(const PP_CONF* conf){
	
	if((*conf).isExact)
		pp_exact_legend();
	else
		pp_heuristic_legend();

}/* pp_legend */

/*
* OP conf configuration for the program execution
* OR false if error, true otherwise
*/
bool readAlgorithms(PP_CONF* conf){
	
	bool result;

	pp_legend(conf);

	if((*conf).isExact)
		result = readArrayDinaIntValidate("Insert the numer of algorithms you want to compare and their codes.\n\n", &((*conf).algs), (intvalidatorfunc)validateExactAlgCode);
	else
		result = readArrayDinaIntValidate("Insert the numer of algorithms you want to compare and their codes.\n\n", &((*conf).algs), (intvalidatorfunc)validateHeurAlgCode);

	if(!result)
		return false;

	if((*conf).algs.n == 0){
		printf("Nothing to do..\n");
		return false;
	}/* if */

	return true;

}/* readAlgorithms */

/*
* IP argc number of elements contained in argv
* IP argv[] -s, --seed	: seed used for random generation
* IP argv[] -tl       	: set an execution time limit (in seconds)
* IP argv[] -n, --nodes	: number of nodes for the random instance
* IP argv[] -h  --help	: to reach the help section
* OP conf configuration for the program execution
* OR true if error, false otherwise
*/
bool readPPConfiguration(int argc, char* const* argv, PP_CONF* conf){
	
	if(pp_validate(&((*conf).set), parseCMDLine(argc, argv, &((*conf).set))))
		return true;
	
	(*conf).isExact = readBool("Do you want to compare exact methods? (yes/no): ");

	return !readAlgorithms(conf);
	
}/* readConfiguration */

/*
* IP set settings to run
* IP alg algorithm to run
* OP sol solution
* OR true if error, false otherwise
*/
bool runPPHeurAlg(const Settings* set, PP_ALG alg, TSPInstance* inst, TSPSolution* sol){

	switch (alg){

	    case PP_RANDOM:
	        return offline_run_refinement(O_RANDOM, SKIP, inst, sol, set);
		case PP_RANDOM_2OPT:
	        return offline_run_refinement(O_RANDOM, OPT2, inst, sol, set);
		case PP_RANDOM_TABU_CONST:
	        return offline_run_refinement(O_RANDOM, TABU, inst, sol, set);
		case PP_RANDOM_TABU_TRIANG:
	        return offline_run_refinement(O_RANDOM, TABU_TRIANG, inst, sol, set);
		case PP_RANDOM_TABU_SQUARE:
	        return offline_run_refinement(O_RANDOM, TABU_SQUARE, inst, sol, set);
		case PP_RANDOM_TABU_SAWTOO:
	        return offline_run_refinement(O_RANDOM, TABU_SAWTOO, inst, sol, set);
		case PP_RANDOM_VNS:
	        return offline_run_refinement(O_RANDOM, VNS, inst, sol, set);

	    case PP_NEAREST_NEIGHBOR_START_FIRST_NODE:
	        return offline_run_refinement(O_NEAREST_NEIGHBOR_START_FIRST_NODE, SKIP, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_2OPT:
	        return offline_run_refinement(O_NEAREST_NEIGHBOR_START_FIRST_NODE, OPT2, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_CONST:
	        return offline_run_refinement(O_NEAREST_NEIGHBOR_START_FIRST_NODE, TABU, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_TRIANG:
	        return offline_run_refinement(O_NEAREST_NEIGHBOR_START_FIRST_NODE, TABU_TRIANG, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_SQUARE:
	        return offline_run_refinement(O_NEAREST_NEIGHBOR_START_FIRST_NODE, TABU_SQUARE, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_SAWTOO:
	        return offline_run_refinement(O_NEAREST_NEIGHBOR_START_FIRST_NODE, TABU_SAWTOO, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_VNS:
	        return offline_run_refinement(O_NEAREST_NEIGHBOR_START_FIRST_NODE, VNS, inst, sol, set);

		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_START_RANDOM_NODE, SKIP, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_2OPT:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_START_RANDOM_NODE, OPT2, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_CONST:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_START_RANDOM_NODE, TABU, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_TRIANG:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_START_RANDOM_NODE, TABU_TRIANG, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_SQUARE:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_START_RANDOM_NODE, TABU_SQUARE, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_SAWTOO:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_START_RANDOM_NODE, TABU_SAWTOO, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_VNS:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_START_RANDOM_NODE, VNS, inst, sol, set);

		case PP_NEAREST_NEIGHBOR_BEST_START:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, SKIP, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_BEST_START_2OPT:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, OPT2, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_BEST_START_TABU_CONST:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, TABU, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_BEST_START_TABU_TRIANG:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, TABU_TRIANG, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_BEST_START_TABU_SQUARE:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, TABU_SQUARE, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_BEST_START_TABU_SAWTOO:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, TABU_SAWTOO, inst, sol, set);
		case PP_NEAREST_NEIGHBOR_BEST_START_VNS:
			return offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, VNS, inst, sol, set);
		
	    default:
	        printf("\nError: Algorithm code not found.\n");
	        return true;
    }/* switch */
	
}/* runPPHeurAlg */

/*
* IP set settings to run
* IP alg algorithm to run
* OP sol solution
* OP et execution time in seconds
* OR true if error, false otherwise
*/
bool runPPExactAlg(const Settings* set, PP_ALG alg, TSPInstance* inst, TSPSolution* sol, double* et){

	switch (alg){

		case PP_BENDERS_NO_MIPSTART:
			return optimize_offline(set, inst, false, BENDERS, sol, et);
		case PP_BENDERS_MIPSTART:
			return optimize_offline(set, inst, true, BENDERS, sol, et);

		case PP_BENDERS_PATCH_NO_MIPSTART:
			return optimize_offline(set, inst, false, BENDERS_PATCH, sol, et);
		case PP_BENDERS_PATCH_MIPSTART:
			return optimize_offline(set, inst, true, BENDERS_PATCH, sol, et);

		case PP_CANDIDATE_CALLBACK_NO_MIPSTART:
			return optimize_offline(set, inst, false, CANDIDATE_CALLBACK, sol, et);
		case PP_CANDIDATE_CALLBACK_MIPSTART:
			return optimize_offline(set, inst, true, CANDIDATE_CALLBACK, sol, et);

		case PP_USERCUT_CALLBACK_NO_MIPSTART:
			return optimize_offline(set, inst, false, USERCUT_CALLBACK, sol, et);
		case PP_USERCUT_CALLBACK_MIPSTART:
			return optimize_offline(set, inst, true, USERCUT_CALLBACK, sol, et);

	    default:
	        printf("\nError: Algorithm code not found.\n");
	        return true;
    }/* switch */

}/* runPPExactAlg */

/*
* IP set settings to run
* IP alg algorithm to run
* OP sol solution
* OP et execution time in seconds
* OR true if error, false otherwise
*/
bool runPPAlg(const Settings* set, PP_ALG alg, TSPInstance* inst, TSPSolution* sol, double* et){
	
	if(alg < PP___END_HEURISTIC){
		*et = (*set).tl;
		return runPPHeurAlg(set, alg, inst, sol);
	}/* if */

	return runPPExactAlg(set, alg, inst, sol, et);

}/* runPPAlg */

/*
* IP alg algorithm
* OP name of the algorithm
*/
void getAlgName(PP_ALG alg, char name[]){

	switch (alg){
		
		/* HEURISTICS */

	    case PP_RANDOM:
	        sprintf(name, "rndm");
			break;
		case PP_RANDOM_2OPT:
	        sprintf(name, "rndm_2opt");
			break;
		case PP_RANDOM_TABU_CONST:
	        sprintf(name, "rndm_tabu");
			break;
		case PP_RANDOM_TABU_TRIANG:
	        sprintf(name, "rndm_triang_tabu");
			break;
		case PP_RANDOM_TABU_SQUARE:
	        sprintf(name, "rndm_square_tabu");
			break;
		case PP_RANDOM_TABU_SAWTOO:
	        sprintf(name, "rndm_sawtoo_tabu");
			break;
		case PP_RANDOM_VNS:
	        sprintf(name, "rndm_vns");
			break;

	    case PP_NEAREST_NEIGHBOR_START_FIRST_NODE:
	        sprintf(name, "nnfn");
			break;
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_2OPT:
	        sprintf(name, "nnfn_2opt");
			break;
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_CONST:
	        sprintf(name, "nnfn_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_TRIANG:
	        sprintf(name, "nnfn_triang_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_SQUARE:
	        sprintf(name, "nnfn_square_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_TABU_SAWTOO:
	        sprintf(name, "nnfn_sawtoo_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_START_FIRST_NODE_VNS:
	        sprintf(name, "nnfn_vns");
			break;

		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE:
			sprintf(name, "nnrn");
			break;
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_2OPT:
			sprintf(name, "nnrn_2opt");
			break;
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_CONST:
			sprintf(name, "nnrn_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_TRIANG:
			sprintf(name, "nnrn_triang_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_SQUARE:
			sprintf(name, "nnrn_square_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_TABU_SAWTOO:
			sprintf(name, "nnrn_sawtoo_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_START_RANDOM_NODE_VNS:
			sprintf(name, "nnrn_vns");
			break;

		case PP_NEAREST_NEIGHBOR_BEST_START:
			sprintf(name, "nnbs");
			break;
		case PP_NEAREST_NEIGHBOR_BEST_START_2OPT:
			sprintf(name, "nnbs_2opt");
			break;
		case PP_NEAREST_NEIGHBOR_BEST_START_TABU_CONST:
			sprintf(name, "nnbs_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_BEST_START_TABU_TRIANG:
			sprintf(name, "nnbs_triang_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_BEST_START_TABU_SQUARE:
			sprintf(name, "nnbs_square_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_BEST_START_TABU_SAWTOO:
			sprintf(name, "nnbs_sawtoo_tabu");
			break;
		case PP_NEAREST_NEIGHBOR_BEST_START_VNS:
			sprintf(name, "nnbs_vns");
			break;
		
		/* EXACTS */
		
		case PP_BENDERS_NO_MIPSTART:
			sprintf(name, "benders");
			break;
		case PP_BENDERS_MIPSTART:
			sprintf(name, "benders_mipstart");
			break;

		case PP_BENDERS_PATCH_NO_MIPSTART:
			sprintf(name, "benders_patch");
			break;
		case PP_BENDERS_PATCH_MIPSTART:
			sprintf(name, "benders_patch_mipstart");
			break;

		case PP_CANDIDATE_CALLBACK_NO_MIPSTART:
			sprintf(name, "candidate_callback");
			break;
		case PP_CANDIDATE_CALLBACK_MIPSTART:
			sprintf(name, "candidate_callback_mipstart");
			break;

		case PP_USERCUT_CALLBACK_NO_MIPSTART:
			sprintf(name, "usercut_callback");
			break;
		case PP_USERCUT_CALLBACK_MIPSTART:
			sprintf(name, "usercut_callback_mipstart");
			break;

	    default:
	        printf("\nError: Algorithm code not found.\n");
			sprintf(name, "UNKNOWN");

    }/* switch */

}/* getAlgName */

/*
* IP conf configuration for the program execution
* OF outF already opened output file
*/
void writeHeader(const PP_CONF* conf, FILE *outF){

	int i;
	char name[MAX_FILE_NAME_SIZE];

	fprintf(outF, "%d", (*conf).algs.n);

	for(i = 0; i < (*conf).algs.n; i++){
		
		PP_ALG alg;

		if((*conf).isExact)
			alg = (PP_ALG)((*conf).algs.v[i] + EXACT_METHODS_OFFSET);
		else
			alg = (PP_ALG)(*conf).algs.v[i];

		getAlgName(alg, name);

		fprintf(outF, ", %s", name);
	}/* for */

	fprintf(outF, "\n");

}/* writeHeader */

/*
* IP conf configuration for the program execution
* OR true if error, false otherwise
*/
bool runPPConfiguration(const PP_CONF* conf){

	int i, j;
	double et;
	char nametemp[MAX_FILE_NAME_SIZE];
	TSPInstance inst;
	TSPSolution sol;
	FILE *outF;

    if ((outF = fopen(PPROF_OUT_FILE, "w")) == NULL){
        printf("Error while opening the file: %s\n\n", PPROF_OUT_FILE);
        return -1;
    }/* if */
	
	allocInst((*conf).set.n, &inst);
	allocSol((*conf).set.n, &sol);
	
	writeHeader(conf, outF);

	printf("\n");

	for(i = 0; i < NOF_TEST_INSTANCES; i++){

		sprintf(nametemp, "inst_nnodes%d_seed%d_tl%d_id%d", (*conf).set.n, (*conf).set.seed, (*conf).set.tl, i + 1);
		generateInstanceName(&((*conf).set), (i == 0), nametemp, &inst);

		printf("* Working on instance %s\n", inst.name);

		fprintf(outF, "%s", inst.name);

		for(j = 0; j < (*conf).algs.n; j++){
			
			PP_ALG alg;

			if((*conf).isExact)
				alg = (PP_ALG)((*conf).algs.v[j] + EXACT_METHODS_OFFSET);
			else
				alg = (PP_ALG)(*conf).algs.v[j];

			getAlgName(alg, nametemp);
			printf("\t -> Running algorithm %s...\n", nametemp);

			if(runPPAlg(&((*conf).set), alg, &inst, &sol, &et)){
				fclose(outF);
				freeInst(&inst);
				freeSol(&sol);
				return true;
			}/* if */
			
			if((*conf).isExact)
				fprintf(outF, ", %lf", et);
			else
				fprintf(outF, ", %lf", sol.val);

		}/* for */

		fprintf(outF, "\n");

	}/* for */
	
	fclose(outF);
	freeInst(&inst);
	freeSol(&sol);

	return false;

}/* runConfiguration */

/*
* IP conf configuration
* OF the pdf plot of the performance profile
*/
void ppplot(const PP_CONF* conf){
	
	char cmd[MAX_FILE_NAME_SIZE * 5];
	
	if((*conf).isExact)
		sprintf(cmd, PP_PLOT_EXACT_CMD, (*conf).set.tl);
	else
		sprintf(cmd, PP_PLOT_HEUR_CMD, (*conf).set.tl);

	system(cmd);
	
}/* ppplot */

/*
* IP conf configuration to free the memory
*/
void freePPConf(PP_CONF* conf){
	freeArrayDinaInt(&((*conf).algs));
}/* freePPConf */

/*
* IP argc number of elements contained in $argv
* IP argv command line arguments
* OR Result code (
   0: success, nothing unexpected happened;
  ).
*/
int main(int argc, char* const* argv){
    
	PP_CONF conf;

    printf("\nPerformance Profile program started...\n\n");

	if(readPPConfiguration(argc, argv, &conf)){
		printf("\nPerformance Profile progran ended.\n\n");
		return 0;
	}/* if */

	if(!runPPConfiguration(&conf))
		ppplot(&conf);

	freePPConf(&conf);

    printf("\nPerformance Profile progran ended.\n\n");
    
    return 0;

}/* main */
