/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : point.h
*/

#define MAX_RANDOM_VALUE 1e4

typedef struct {
    int id;
    double x;
    double y;
} Point;

double rand01(void);

int rand0N(int);

void randomPoint(int, Point*);
