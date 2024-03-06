/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : point.h
*/

#pragma once
#define MAX_RANDOM_VALUE 1e4

/* Bidimensional point on the cartesian plane */
typedef struct {
    double x, y;
} Point2D;

void randomPoint(Point2D*);

double distance(const Point2D*, const Point2D*);
