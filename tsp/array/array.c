/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : array.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "array.h"
#include "../utility/utility.h"

/*
* IP dummy integer
* OR true
*/
bool intTrue(int dummy){
	return true;
}/* intTrue */

/*
* IP n the size of the array to allocate
* OP arr the allocated array
*/
void initArrayDinaInt(int n, ArrayDinaInt* arr){

	(*arr).v = malloc(n * sizeof(int));
	assert((*arr).v != NULL);

	(*arr).n = n;

}/* initArrayDinaInt */

/* Reads and allocate an integer dinamic array from the keyboard.
* IP label for the output
* IK+OP a Array to be allocated and read.
* OR false if error, true otherwise
*/
bool readArrayDinaIntValidate(const char label[], ArrayDinaInt* a, intvalidatorfunc validate) {
    int i;
    printf("%s", label);
    a->n = readInt("Number of elements of the array: ");
    a->v = malloc(a->n * sizeof(int));
    assert(a->v != NULL);
    for (i = 0; i < a->n; i++) {
        printf("Value[%d]: ", i);
        scanf("%d", &a->v[i]);
		if(!(validate(a->v[i]))){
			free(a->v);
			return false;
		}/* if */
    } /* for */
	return true;
} /* readArrayDinaIntValidate */

/* Reads and allocate an integer dinamic array from the keyboard.
* IP label for the output
* IK+OP a Array to be allocated and read.
* OR false if error, true otherwise
*/
void readArrayDinaInt(const char label[], ArrayDinaInt* a) {
    readArrayDinaIntValidate(label, a, (intvalidatorfunc)intTrue);
} /* readArrayDinaInt */

/*
* IOP arr the array to free
*/
void freeArrayDinaInt(ArrayDinaInt* arr){
	free((*arr).v);
}/* freeArrayDinaInt */
