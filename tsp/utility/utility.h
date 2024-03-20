/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : utility.h
*/

#include <stdbool.h>
#include <time.h>

#define MAX_STRING_LENGTH 65
#define PRINT_FREQUENCY 1 /* seconds between one print and another */

void printBool(const char[], bool);

int rand0N(int);

void swapInt(int*, int*);

int readInt(const char[]);

bool isEqualPrecision(double, double, double);

bool isEqual(double, double);

int getSeconds(clock_t);

void printSeconds(const char[], int);

int readIntRange(int, int, const char[]);

void invertArray(int, int, int*);

bool isTimeOut(clock_t, int);

bool isTimeOutWarning(const char[], clock_t, int);

void processBar(int ni, int tni);

void timeBar(clock_t, int, int*);

int max(int, int);
