/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : utility.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utility.h"

#define EPSILON 1e-9

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

/*
* IP a pointer to index of a node
* IP b pointer to index of a node
*/
void swap_int(int* a, int* b){
    int temp;

    temp = *a;
    *a = *b;
    *b = temp;

}/* swap_int */

/*
* Read integer in input.
* IP lab label for the output
* OR integer passed in input
*/
int read_int(const char lab[]){
    int num;

    printf("%s", lab);
    scanf("%d", &num);
    printf("\n\n");

    return num;
}/* read_int */

/*
* IP a double number
* IP b double number to check equality with $a
* IP epsilon precision of equality
* OR true if $a equal to $b, false otherwise
*/
bool is_equal_prescision(double a, double b, double epsilon){
    
    return fabs(a-b) <= epsilon;

}/* is_equal_precision */

/*
* IP a double number
* IP b double number to check equality with $a
*/
bool is_equal(double a, double b){
    
    return is_equal_prescision(a, b, 0);

}/* is_equal */
