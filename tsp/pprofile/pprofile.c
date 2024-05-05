/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : pprofile.c
*/

#include "pprofile.h"
#include "../utility/utility.h"
#include <stdio.h>

/*
* OV help section
*/
void pp_help(void){

    printf("Performance profile options:\n");

    printf("\t-s, --seed  <seed_value>\tseed used for random generation (integer value)\n");
    printf("\t-tl         <timelimit_value>\tset the execution time limit (in seconds) for an instance\n");
	printf("\t-n, --nodes <number_of_nodes>\tnumber of nodes for the random instances\n");
    printf("\t-h  --help\t\t\tto reach this section\n");
    printf("\n");

    printf("Usage:\n");
    printf("\tYou must specify the number of nodes for a random generation.\n");
    printf("\tParameter <timelimit_value> accepts only strictly positive integer values\n");
    printf("\tParameter <seed_value> accepts only positive integer values\n");
    printf("\tParameter <number_of_nodes> accepts only integer values strictly greater than 2\n");
	printf("\tOption help must be specified itself\n\n");

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
* IP code of the algorithm
* OR true if $code is a valid algorithm code, false otherwise
*/
bool validateAlgCode(int code){

	if(code < 1 || code == PP___END_HEURISTIC || PP___END_EXACT <= code){
		printf("\nError: Invalid algorithm code!\n");
		return false;
	}/* if */
	
	return true;

}/* validateAlgCode */

/*
* OV the random legend
*/
void pp_random_legend(){

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
void pp_nearest_neighbor_first_node_legend(){

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
void pp_nearest_neighbor_random_node_legend(){

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
void pp_nearest_neighbor_best_start_legend(){

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
* OV the legend
*/
void pp_legend(){
	
	printf("Available algorithms:\n");

	pp_random_legend();
	
	pp_nearest_neighbor_first_node_legend();

	pp_nearest_neighbor_random_node_legend();

	pp_nearest_neighbor_best_start_legend();

}/* pp_legend */

/*
* OP conf configuration for the program execution
*/
bool readAlgorithms(PP_CONF* conf){
	
	pp_legend();

	return readArrayDinaIntValidate("Insert the numer of algorithms you want to compare and their codes.\n\n", &((*conf).algs), (intvalidatorfunc)validateAlgCode);

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
bool readConfiguration(int argc, char* const* argv, PP_CONF* conf){
	
	if(pp_validate(&((*conf).set), parseCMDLine(argc, argv, &((*conf).set))))
		return true;

	return !readAlgorithms(conf);
	
}/* readConfiguration */

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

	readConfiguration(argc, argv, &conf);
	
    printf("\nPerformance Profile progran ended.\n\n");
    
    return 0;

}/* main */
