/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : point.c
*/

#include <stdlib.h>

#include "point.h"
#include "../utility/utility.h"

/*
* OP p random point in the square [0, MAX_RANDOM_VALUE]^2
*/
void randomPoint(Point2D* p){

    p->x = rand0N(MAX_RANDOM_VALUE + 1);
    p->y = rand0N(MAX_RANDOM_VALUE + 1);

}/* randomPoint */
