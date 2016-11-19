#include<stdio.h> 
#include"omp.h"

void main(){
    // Clause to request a certain number os threads
    omp_set_num_threads(6);
    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf("Hello(%d)", ID);
        printf("world(%d)\n", ID);
        
    }
}
