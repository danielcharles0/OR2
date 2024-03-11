/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : utility.h
*/

#include <stdbool.h>
#include <time.h>

#define MAX_STRING_LENGTH 65

void printBool(const char[], bool);

int rand0N(int);

void swapInt(int*, int*);

int readInt(const char[]);

bool isEqualPrecision(double, double, double);

bool isEqual(double, double);

int getSeconds(clock_t, clock_t);

int readIntRange(int, int, const char[]);

void invertList(int, int, int*);
