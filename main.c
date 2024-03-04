/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : main.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "tsp/input/settings/settings.h"
/*#include "tsp/input/reader/reader.h"*/
#include "tsp/input/generator/generator.h"

/*
* IP conf configuration to run
* IP set settings of the configuration
* OR boolean false if there was some error, true otherwise.
*/
bool runConfig(CONF config, const Settings* set){
    
	int error;
    TSPInstance inst;

    switch(config){
        case INPUT_FILE:
			/* TO BE COMPLETED */
            /*error = readInstance(set, &inst);*/
			error = -1;
            if(error){
                if(error == -1)
                    printf("Error while opening the file \"%s\"\n\n", (*set).input_file_name);
                else
                    printf("The instance must contains at least three nodes and the number of nodes must have been specified into the input file.\n\n");
                return 0;
            }/* if */
            break;
        case RANDOM_GENERATION:
            generateInstance(set, &inst);
            break;
		case HELP:
            return 1;
        default: /* ERROR */
            printf("Error in the configuration.\n\n");
            return 0;
    }

	/* TO BE COMPLETED */
	if((*set).v)
		printInst(&inst);

    freeInst(&inst);

	return 1;
    
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

	if(config && config != HELP && set.v){
        
        printf("Result code configuration: %d\n\n", config);

        printSettings(&set);

    }/* if */
    
    runConfig(config, &set);

    printf("Progran ended.\n");

    return 0;

} /* main */
