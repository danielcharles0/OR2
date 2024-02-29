/*
* Project  : Travelling Salesman Problem implementations
* Authors    : Luigi Frigione, Daniel Carlesso
* IDs       : 2060685, 2088626
* File      : parser.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "parser.h"

static struct option long_options[] = {
        {"seed", required_argument, 0, 's'},
        {"file", required_argument, 0, 'f'},
        {"nodes", required_argument, 0, 'n'},
        {"tl", required_argument, 0, 't'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0} // End of options
};

/*
* OV help section
*/
void help(void){
    printf("Options:\n");
    printf("\t-f, --file  <file_name>\t\tinput file (no file names with more than %d characters are accepted)\n", MAX_FILE_NAME_SIZE - 1);
    printf("\t-s, --seed  <seed_value>\t\tseed used for random generation (integer value)\n");
    printf("\t-tl         <timelimit_value>\tset an execution time limit (in seconds)\n");
    printf("\t-n, --nodes <number_of_nodes>\tnumber of nodes for the random instance\n");
    printf("\t-h  --help\t\t\tto reach this section\n");
    printf("\n");
    printf("Usage:\n");
    printf("\tYou must specify an input file or the number of nodes for a random generation (you cannot specify both).\n");
    printf("\tParameters <timelimit_value> accepts only strictly positive integer values\n");
    printf("\tParameter <seed_value> accepts only positive integer values\n");
    printf("\tParameter <number_of_nodes> accepts only integer values strictly greater than 2\n");
    printf("\tIf you want to execute a refinement method you need first to compute a solution with one of the available approaches\n");
    printf("\tOption help must be specified itself\n\n");
}/* help */

/*
* OP set the function will initialize the settings with default values
*/
void init(Settings* set){
    set->input_file_name[0] = '\0';
    set->nodes = 0;
    set->seed = 1;
    set->tl = DEFAULT_TIME_LIMIT;
    set->v = false;
}/* init */

/*
* IP set settings
* OV settings values
*/
void printSettings(Settings* set){
    printf("Settings:\n");
    if(!set->nodes)
        printf("\tinput file name: %s\n", set->input_file_name);
    printf("\tseed: %d\n", set->seed);
    printf("\tnumber of nodes: %d\n", set->nodes);
    printf("\ttime limit: %d\n", set->tl);
    printf("\tverbose: %s\n", set->v ? "true" : "false");
    printf("\n");
}/* printSettings */

/*
* IP argc number of elements contained in argv
* IP argv[] -f, --file: input file name
* IP argv[] -s, --seed: seed used for random generation
* IP argv[] -tl       : set an execution time limit (in seconds)
* IP argv[] -v        : set the verbose flag at true
* IP argv[] -n        : number of nodes for the random instance
* IP argv[] -h  --help: to reach the help section
* OP integer with error value
*/
CONF parseCMDLine(int argc, char* const* argv, Settings* set){
    int opt, opt_index;
    CONF config = RANDOM_GENERATION;

    init(set);

    while((opt = getopt_long_only(argc, argv, ":f:n:s:tl:hv", long_options, &opt_index)) != -1) {
        switch(opt){
            case 'f':
                if(strlen(optarg) >= MAX_FILE_NAME_SIZE){
                    printf("The filename exceed the allowed number of characters.\n");
                    return ERROR;
                }
                strcpy(set->input_file_name, optarg);
                config = INPUT_FILE;
            case 's':
                set->seed = strtol(optarg, NULL, 10);
                if(set->seed < 0){
                    printf("The seed must be a natural number.\n");
                    return ERROR;
                }
                break;
            case 'n':
                set->nodes = strtol(optarg, NULL, 10);
                if(set->nodes <= 2){
                    printf("The number of nodes must be an integer greater than 2.\n");
                    return ERROR;
                }
                break;
            case 't':
                set->tl = strtol(optarg, NULL, 10);
                if(set->tl <= 0){
                    printf("The time limit must be a positive integer.\n");
                    return ERROR;
                }
                break;
            case 'v':
                set->v = true;
                break;
            case 'h':
                return HELP;
            default:
                printf("Unrecognized option. Type -h or --help for usage.\n");
                return ERROR;
        }
    }

    return config;
}/* parseCMDLine */
