/*
* Project  : Travelling Salesman Problem implementations
* Authors    : Luigi Frigione, Daniel Carlesso
* IDs       : 2060685, 2088626
* File      : main.c
*/
#include <stdio.h>
#include <stdlib.h>

#include "input/parser/parser.h"

void runConfig(CONF config, Settings* set){
    switch(config){
        case INPUT_FILE:
            //add reader
            if(set->v)
                printSettings(set);
            break;
        case RANDOM_GENERATION:
            //add generator
            if(set->v)
                printSettings(set);
            break;
        case ERROR:
            printf("Error in the configuration.\n\n");
            break;
        case HELP:
            help();
            break;
    }
}

int main(int argc, char* const* argv){

    Settings set;
    CONF config; 

    printf("Program started...\n\n");

    config = parseCMDLine(argc, argv, &set);
    
    runConfig(config, &set);

    printf("Progran ended.\n");

    return 0;
}
