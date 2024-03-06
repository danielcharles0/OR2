/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : point.c
*/

#include <stdlib.h>
#include <math.h>

#include "point.h"
#include "../utility/utility.h"

/*
* OP p random point in the square [0, MAX_RANDOM_VALUE]^2
*/
void randomPoint(Point2D* p){

    p->x = rand0N(MAX_RANDOM_VALUE + 1);
    p->y = rand0N(MAX_RANDOM_VALUE + 1);

}/* randomPoint */

/*
* IP p point of which to calculate the squared distance from the origin
* OR squared distance from the origin
*/
double originSquaredDistance(const Point2D* p){
	
	return pow((*p).x, 2) + pow((*p).y, 2);

}/* originSquaredDistance */

/*
* IP p point of which to calculate the distance from the origin
* OR distance from the origin
*/
double originDistance(const Point2D* p){

    return sqrt(originSquaredDistance(p));

}/* originDistance */

/*
* IP p1 first point
* IP p2 second point
* OR distance between $p1 and $p2
*/
double distance(const Point2D* p1, const Point2D* p2){
    
    Point2D pdiff;

    pdiff.x = (*p1).x - (*p2).x;
    pdiff.y = (*p1).y - (*p2).y;

    return originDistance(&pdiff);

}/* distance */
