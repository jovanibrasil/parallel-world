/* Basic approach
    1. Find compute intensive loops.
    2. Make the loop iterations independent, so they can safely execute in any order without loop-carried dependencies.
    3. Place the appropriate OpenMP.
*/

#include <stdio.h> 
#include "omp.h"

static long num_steps = 1000000;
double step;

void main(){
    
    omp_set_num_threads(10);
    
    int i;
    double pi, sum = 0.0;

    step = 1.0/(double) num_steps;

    #pragma omp parallel for
    for(i=0; i<num_steps; i++){
        double x = (i + 0.5) * step;
        #pragma omp critical
        sum = sum + 4.0 / (1.0 + x * x);
    }
   
    pi = step * sum;
    printf("%f\n", pi);

    pi = 0;
    sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for(i=0; i<num_steps; i++){
        double x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
    }   

    pi = step * sum;
    printf("(Reduction) %f\n", pi);
}
