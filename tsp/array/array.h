/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : array.h
*/

#pragma once

#include <stdbool.h>

/*
* IP integer to validate
* OR true if the input is valid, false otherwise
*/
typedef bool (*intvalidatorfunc)(int);

typedef struct {
    int n;
    int* v;
} ArrayDinaInt;

void initArrayDinaInt(int, ArrayDinaInt*);

bool readArrayDinaIntValidate(const char[], ArrayDinaInt*, intvalidatorfunc);

void freeArrayDinaInt(ArrayDinaInt*);
