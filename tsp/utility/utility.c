/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : utility.c
*/

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

/*
* Print a boolean.
* IP lab label for the output
* IP b boolean to print
* OV print $b
*/
void printBool(const char lab[], bool b) {
    printf("%s", lab);
    if (b)
        printf("true");
    else
        printf("false");
    printf("\n");
} /* printBool */

/*
* IP n
* OR random integer value in [0, $n - 1]
* Note: rand() returns an integer value in the range [0, RAND_MAX - 1]
*/
int rand0N(int n){

    return rand() % n;

}/* rand0N */
