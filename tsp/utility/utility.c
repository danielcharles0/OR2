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

#define EPSILON 1e-7
#define PROCESS_BAR_PRECISION 20
#define PROCESS_BAR_COMPLETED_CHARACTER '#'
#define PROCESS_BAR_TO_COMPLETE_CHARACTER '-'
#define TIMEOUT_WARNING_MESSAGE "Time limit reached! Returning the best solution.\n\n"

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
void swapInt(int* a, int* b){
    
	int temp;

    temp = *a;
    *a = *b;
    *b = temp;

}/* swapInt */

/*
* Read integer in input.
* IP lab label for the output
* OR integer passed in input
*/
int readInt(const char lab[]){
    
	int num;

    printf("%s", lab);
    scanf("%d", &num);
    printf("\n");

    return num;

}/* readInt */

/*
* IP start starting value of the range
* IP end end value of the range
* IP lab label for the output
* OR integer value in [$start, $end]
*/
int readIntRange(int start, int end, const char lab[]){
	
	int val;
    char str[MAX_STRING_LENGTH];
    
    sprintf(str, "%s in the range [%d, %d]: ", lab, start, end);

	do{
        val = readInt(str);

        if(val < start || val > end)
            printf("Invalid value.\n");

    }while(val < start || val > end);

	return val;

}/* readIntRange */

/*
* IP a double number
* IP b double number to check equality with $a
* IP epsilon precision of equality
* OR true if $a dists less then $epsilon to $b, false otherwise
*/
bool isEqualPrecision(double a, double b, double epsilon){
    
    return fabs(a - b) < epsilon;

}/* isEqualPrecision */

/*
* IP a double number
* IP b double number to check equality with $a
* OR true if $a dists less then EPSILON to $b, false otherwise
*/
bool isEqual(double a, double b){
    
    return isEqualPrecision(a, b, EPSILON);

}/* isEqual */

/*
* IP start starting execution time
* IP end ending execution time
* OR execution time in seconds
*/
int getSeconds(clock_t start){

    return (clock() - start) / CLOCKS_PER_SEC;

}/* getSeconds */

/*
* IP start starting point of arr's sublist to invert
* IP end ending point of arr's sublist to invert
* IOP arr list to be inverted
*/
void invertArray(int start, int end, int* arr){
    
    int delta = end - start;

    while(delta > 0){

        swapInt(&(arr[start]), &(arr[end]));

        delta = (--end) - (++start);

    }
    
}/* invertArray */

/*
* IP n dividend
* IP d divisor
* OP r = n % d
* OR q = n / d
* reference: https://stackoverflow.com/questions/63750121/is-it-possible-to-get-both-the-modulus-and-quotient-of-division-in-a-single-oper
*/
int division(int n, int d, int* r){
	*r = n % d;
	return n / d;
}/* div */

/*
* IP lab label for the output
* IP sec seconds to print
* OV the label $lab concatenated with the formatted time stamp
*/
void printSeconds(const char lab[], int sec){

	int fsec, fmin, fhour;

	fhour = division(division(sec, 60, &fsec), 60, &fmin);

	printf("%s%2dh %2dm %2ds", lab, fhour, fmin, fsec);

	fflush(stdout);

}/* printSeconds */

/*
* IP start starting execution time
* IP tl time limit in seconds
* OR true if the execution time exceeded the time limit, false otherwise
*/
bool isTimeOut(clock_t start, int tl){

    return getSeconds(start) > tl;

}/* isTimeOut */

/*
* IP war warning to print if timelimit exceeded
* IP start starting execution time
* IP tl time limit in seconds
* OR true if the execution time exceeded the time limit, false otherwise
*/
bool isTimeOutWarning(const char war[], clock_t start, int tl){

	if(!isTimeOut(start, tl))
		return false;

	printf("\n%s", war);
	return true;

}/* isTimeOutWarning */

/*
* IP set settings
* IP start starting time of processing
* IOP ls last stamp second from the execution start
*/
bool checkTimeLimit(const Settings* set, int start, int* ls){

    if(isTimeOutWarning(TIMEOUT_WARNING_MESSAGE, start, (*set).tl))
        return true;
    else if((*set).v)
        timeBar(start, (*set).tl, ls);
    
    return false;

}/* checkTimeLimit */

/*
* IP n number of repetitions
* IP c char to repeat
* OV this function prints to the screen $n times the character $c
*/
void repeat(int n, char c){

    int i;

    for(i = 0; i < n; i++)
        printf("%c", c);

}/* repeat */

/*
* IP ni number of iteration processed
* IP tni total number of iterations to process
* OV prints a process bar with precision PROCESS_BAR_PRECISION
*/
void processBar(int ni, int tni){

    int n;
    double frac;

    frac = (double) ni / tni;

    n = (frac * PROCESS_BAR_PRECISION);

    printf("\rProcess: [");
    repeat(n, PROCESS_BAR_COMPLETED_CHARACTER);
    repeat(PROCESS_BAR_PRECISION - n, PROCESS_BAR_TO_COMPLETE_CHARACTER);
    printf("] %.2f%% ", frac * 100);

    /*if(tni <= ni)
        printf("\n\n");*/

	/* resource: https://www.educative.io/answers/what-is-fflush-in-c */
    fflush(stdout);

}/* processBar */

/*
* IP start executing time start
* IP tl time limit
* IP freq printing frequency
* IOP ls last stamp second from the execution time start
*/
void timeBarPrecision(clock_t start, int tl, int freq, int* ls){
	
	int s = getSeconds(start);

	if(s - *ls >= freq){
		processBar(s, tl);
		printSeconds("Running time: ", s);
		*ls = s;
	}/* if */

}/* timeBarPrecision */

/*
* IP start executing time start
* IP tl time limit
* IOP ls last stamp second from the execution time start
*/
void timeBar(clock_t start, int tl, int* ls){
	
    timeBarPrecision(start, tl, PRINT_FREQUENCY, ls);

}/* timeBar */

/*
* IP a first integer 
* IP b second integer to be compared with $a
* OP max between $a and $b
*/
int max(int a, int b){
    
    if(a > b)
        return a;
    
    return b;

}/* max */

/*
* IOP a pointer to int
* IOP b pointer to int
* IOP c pointer to int
*/
void sortInt(int* a, int* b, int* c){

    if(*a > *b){
        swapInt(a, b);

        if(*b > *c){
            swapInt(b, c);

            if(*a > *b)
                swapInt(a, b);

        }
    }
    else{

        if(*b > *c){
            swapInt(b, c);
        
            if(*a > *b)
                swapInt(a,b);
        }

    }

}/* sortInt */

/*
* IP a first value 
* IP b second value
*/
int compareInt(const void* a, const void* b){

    return ( *(int*)a - *(int*)b );

}/* compareInt */

/*
* IP n array dimension
* IOP arr array to be sorted
*/
void sortIntArray(int n, int* arr){

    qsort(arr, n, sizeof(int), compareInt);

}/* sortIntArray */

/*
* IP start process starting time
* IP freq frequency to plot 
* IOP ls last stamp second from the execution time start
*/
bool timeToPlot(clock_t start, double freq, double* ls){

    double s = getSeconds(start);

	if(s - *ls >= freq){
		*ls = s;
        return true;
    }

    return false;

}/* timeToPlot */
