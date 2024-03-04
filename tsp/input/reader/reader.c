/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : reader.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"

/*
* IP set settings
* IF File $(set->input_file_name) to elaborate.
* OP inst tsp instance
* OR Result code (
   0: success;
   1: error;
)
*/
int readInstance(Settings* set, TSPInstance* inst){
    int i;
    char line[MAX_LINE_LENGTH];

    FILE *file = fopen(set->input_file_name, "r");

    if (file == NULL){
        printf("Error opening file: %s\n\n", set->input_file_name);
        return 1;
    }

    while(fgets(line, MAX_LINE_LENGTH, file)){

        if(!strncmp(line, "EOF", 3)){
            break;
        }

        if(strstr(line, "NAME: ") != NULL){
            int len = strlen(line);
            line[len-1] = '\0';
            strcpy(inst->name, &line[sizeof("NAME: ") - 1]);
        }
        else if(strstr(line, "DIMENSION: ") != NULL){
            inst->dimension = strtol(&line[sizeof("DIMENSION: ") - 1], NULL, 10);
            if(inst->dimension <= 2){
                printf("Error: too few nodes in the file.\n\n");
                fclose(file);
                return 1;
            }
            set->nodes = inst->dimension;
            
            allocInst(inst);
        }
        else if(strstr(line, "NODE_COORD_SECTION") != NULL){
            for(i=0; i<inst->dimension; i++)
                fscanf(file, "%d %lf %lf", &((inst->points)[i].id), &((inst->points[i]).x), &((inst->points[i]).y));
        }
    }

    fclose(file); 

    return 0;  
    
}/* readInstance */
