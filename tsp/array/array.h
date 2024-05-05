/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : array.h
*/

#pragma once

typedef struct {
    int n;
    int* v;
} ArrayDinaInt;

void initArrayDinaInt(int, ArrayDinaInt*);

void freeArrayDinaInt(ArrayDinaInt*);
