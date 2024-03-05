/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : settings.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "settings.h"
#include "validator/validator.h"
#include "../../utility/utility.h"

#define DEFAULT_TIME_LIMIT 300 /* 60 s/m * 5 m */
#define DEFAULT_SEED 2060685 + 2088626

static struct option long_options[] = {
		/* {name, kind_arg, flag, key} */
        {"seed", required_argument, 0, 's'},
        {"file", required_argument, 0, 'f'},
        {"nodes", required_argument, 0, 'n'},
        {"tl", required_argument, 0, 't'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0} /* End of options */
};

/*
* OV help section
*/
void help(){

    printf("Options:\n");
	                                                            /* remember that the last character contains the special EOS value '\0' */
    printf("\t-f, --file  <file_name>\t\tinput file (no file names with more than %d characters are accepted)\n", MAX_FILE_NAME_SIZE - 1);
    printf("\t-s, --seed  <seed_value>\tseed used for random generation (integer value)\n");
    printf("\t-tl         <timelimit_value>\tset an execution time limit (in seconds)\n");
    printf("\t-v          <\\>\t\t\tset the verbose flag to true\n");
	printf("\t-n, --nodes <number_of_nodes>\tnumber of nodes for the random instance\n");
    printf("\t-h  --help\t\t\tto reach this section\n");
    printf("\n");

    printf("Usage:\n");
    printf("\tYou must specify an input file or the number of nodes for a random generation (you cannot specify both).\n");
    printf("\tParameter <timelimit_value> accepts only strictly positive integer values\n");
    printf("\tParameter <seed_value> accepts only positive integer values\n");
    printf("\tParameter <number_of_nodes> accepts only integer values strictly greater than 2\n");
    printf("\tOption help must be specified itself\n\n");

}/* help */

/*
* OP set the function will initialize the settings with default values
*/
void init(Settings* set){

    set->input_file_name[0] = '\0'; /* The empty string */
    set->n = 0;
    set->seed = DEFAULT_SEED;
    set->tl = DEFAULT_TIME_LIMIT;
    set->v = false;

}/* init */

/*
* IP set settings
* OV settings values
*/
void printSettings(const Settings* set){

    printf("Settings:\n");

    if(!set->n)
        printf("\tinput file name: %s\n", set->input_file_name);
    printf("\tseed: %d\n", set->seed);
	if(set->n)
    	printf("\tnumber of nodes: %d\n", set->n);
    printf("\ttime limit: %d\n", set->tl);
    printBool("\tverbose: ", set->v);
	printf("\n");

}/* printSettings */

/*
* IP opt_val value representing the option id
* OP set settings for the program execution
* OR char representing the readed symbol associated to the setted option in the switch-case list,
*    the value 0 has a special meaning, if zero is returned then there was some problem with
*    the option (missing required or unconsistent value)
*/
char setOption(int opt_val, Settings* set){

    switch (opt_val){

        case 'f':
            /* remember that the last character contains the special EOS value '\0' */
            if(strlen(optarg) >= MAX_FILE_NAME_SIZE){
                printf("The filename exceed the allowed number of characters.\n");
                return 0;
            }/*if*/
            strcpy((*set).input_file_name, optarg);
            return 1;

        case 's':
            (*set).seed = strtol(optarg, NULL, 10);
            if((*set).seed < 0)
                return 0;
            return 2;

        case 't':
            (*set).tl = strtol(optarg, NULL, 10);
            if((*set).tl <= 0)
                return 0;
            return 3;

        case 'n':
            (*set).n = strtol(optarg, NULL, 10);
            if((*set).n < 3)
                return 0;
            return 4;
        
        case 'v':
            (*set).v = true;
            return 5;
        
        case 'h':
            (*set).v = true;
            return 6;

        default:
            printf("Unrecognized option: type 'main -h' or 'main --help' to see the options\n");
            return 0;

    }/* switch */

}/* setOption */

/*
* IP argc number of elements contained in argv
* IP argv[] -f, --file	: input file name
* IP argv[] -s, --seed	: seed used for random generation
* IP argv[] -tl       	: set an execution time limit (in seconds)
* IP argv[] -v        	: set the verbose flag at true
* IP argv[] -n, --nodes	: number of nodes for the random instance
* IP argv[] -h  --help	: to reach the help section
* OP set settings for the program execution
* OR CONF (
    ERROR: error in the configuration
    HELP: help section
    INPUT_FILE: read data from the specified input file
    RANDOM_GENERATION: generate a random instance of the specified size
)
*/
CONF parseCMDLine(int argc, char* const* argv, Settings* set){
    
	int opt_indx, opt_val;
    FSM_STATES curr = START;

    init(set);

    while((opt_val = getopt_long_only(argc, argv, ":f:s:n:hv", long_options, &opt_indx)) != EOF)
        curr = delta(curr, setOption(opt_val, set));

    if(optopt) /* if the library recognize an error stores the option character into the variable optopt */
        printf("\n");

    if(curr <= __VALID_END_STATES){
        help();

        if(curr == H)
            return HELP;
        
        return ERROR;
    }/* if */

    if((*set).n == 0)
        return INPUT_FILE;

    return RANDOM_GENERATION;

}/* parseCMDLine */
