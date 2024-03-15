/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : array.c
*/

#include <stdlib.h>
#include <assert.h>
#include "array.h"

/*
* IP n the size of the array to allocate
* OP arr the allocated array
*/
void initArrayDinaInt(int n, ArrayDinaInt* arr){

	(*arr).v = malloc(n * sizeof(int));
	assert((*arr).v != NULL);

	(*arr).n = n;

}/* initArrayDinaInt */

/*
* IOP arr the array to free
*/
void freeArrayDinaInt(ArrayDinaInt* arr){
	free((*arr).v);
}/* freeArrayDinaInt */
