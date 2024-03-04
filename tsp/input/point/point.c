/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : point.c
*/

#include <stdlib.h>
#include <math.h>

#include "point.h"

double rand01(void){

    return ((double) rand()) / RAND_MAX; 

} /* rand01 */

/*
* IP n
* OR random integer value in [0, $n - 1]
*/
int rand0N(int n){
    
    return round(rand01() * n);  

}/* rand0N */

/*
* IP p point
*/
void randomPoint(int i, Point* p){

    p->id = i;
    p->x = rand0N(MAX_RANDOM_VALUE + 1);
    p->y = rand0N(MAX_RANDOM_VALUE + 1);

}/* randomPoint */
