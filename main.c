/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : main.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "tsp/input/settings/settings.h"
#include "tsp/input/reader/reader.h"

/*
* IP conf configuration to run
* IP set settings of the configuration
*/
void runConfig(CONF config, Settings* set){
    int error;
    TSPInstance inst;

    switch(config){
        case INPUT_FILE:
            error = readInstance(set, &inst);
            if(error)
                return;
            if(set->v)
                printSettings(set);
            break;
        case RANDOM_GENERATION:
            /* add generator */
            if(set->v)
                printSettings(set);
            break;
        case ERROR:
            printf("Error in the configuration.\n\n");
            return;
        case HELP:
            help();
            return;
    }

    freeInst(&inst);
    
}/* runConfig */

/*
* IP argc number of elements contained in $argv
* IP argv command line arguments
* OR Result code (
   0: success, nothing unexpected happened;
  ).
*/
int main(int argc, char* const* argv){

    Settings set;
    CONF config; 

    printf("Program started...\n\n");

    config = parseCMDLine(argc, argv, &set);
    
    runConfig(config, &set);

    printf("Progran ended.\n");

    return 0;
} /* main */
