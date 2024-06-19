/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : utility.h
*/

#pragma once

#include <stdbool.h>
#include <time.h>

#include "../array/array.h"
#include "../input/settings/settings.h"

#define MAX_STRING_LENGTH 65
#define PRINT_FREQUENCY 1 /* seconds between one print and another */
#define COST_SAMPLING_FREQUENCY 0.1 /* We print everi COST_SAMPLING_FREQUENCY seconds and so 1 / COST_SAMPLING_FREQUENCY times per second */

void printBool(const char[], bool);

int rand0N(int);

void swapInt(int*, int*);

int readInt(const char[]);

bool isEqualPrecision(double, double, double);

bool isEqual(double, double);

double getSeconds(clock_t);

void printSeconds(const char[], int);

int readIntRange(int, int, const char[]);

void invertArray(int, int, int*);

bool isTimeOut(clock_t, int);

bool isTimeOutWarning(const char[], clock_t, int, bool);

bool checkTimeLimit(const Settings*, clock_t, double*);

void processBar(int ni, int tni);

void timeBar(clock_t, int, double*);

int max(int, int);

void sortInt(int*, int*, int*);

void sortIntArray(int, int*);

bool timeToPlot(clock_t, double, double*);

void printError(const char[]);

double step(double);

bool readBool(const char[]);
