/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : pprofile.c
*/

#include "pprofile.h"

#include <stdio.h>
#include "../tsp.h"
#include "../input/settings/settings.h"

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
* IP argc number of elements contained in $argv
* IP argv command line arguments
* OR Result code (
   0: success, nothing unexpected happened;
  ).
*/
int main(int argc, char* const* argv){
    
	Settings set;

    printf("Performance Profile program started...\n\n");

	if(pp_validate(&set, parseCMDLine(argc, argv, &set)))
		return 0;
    
    printf("Performance Profile progran ended.\n");
    
    return 0;

}/* main */
