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
* Removes space before ':' to homogenize TSPLIB file reading.
* IOP line line of input file to read
*/
void removeSpace(char* line){
    
    int i;
    
    for(i=0; i<strlen(line); i++){

        if(line[i] == ':')
            break;
        else if(line[i] == ' '){
            strcpy(&line[i], &line[i+1]);
            i--;
        }

    }

}/* removeSpace */

/*
* IP set settings
* IF File $(set->input_file_name) to elaborate.
* OP inst tsp instance
* OR Result code (
   0: success;
   1: error;
)
* resource: http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp/
*/
int readInstance(const Settings* set, TSPInstance* inst){
    
	int i, dummy;
    char line[MAX_LINE_LENGTH];

    FILE *file = fopen(set->input_file_name, "r");

    if (file == NULL)
        return -1;

    while(fgets(line, MAX_LINE_LENGTH, file)){
        
        int n;
        
        removeSpace(line);

        if(!strncmp(line, "EOF", 3)){
            break;
        }

        if(strstr(line, "NAME: ") != NULL){
            int len = strlen(line);
            line[len-1] = '\0';
            strcpy(inst->name, &line[sizeof("NAME: ") - 1]);
        }
        else if(strstr(line, "DIMENSION: ") != NULL){
            n = strtol(&line[sizeof("DIMENSION: ") - 1], NULL, 10);
            if(n <= 2){
                fclose(file);
                return -2;
            }
            
            allocInst(n, inst);
        }
        else if(strstr(line, "NODE_COORD_SECTION") != NULL){
            for(i=0; i<inst->dimension; i++)
                fscanf(file, "%d %lf %lf", &dummy, &((inst->points[i]).x), &((inst->points[i]).y));
        }
    }	

    fclose(file);

	computeDistances(inst);

    return 0;  

}/* readInstance */
