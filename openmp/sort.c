#include <stdio.h> 
#include <stdlib.h>
#include "omp.h"

int compare (const void * a, const void * b){
  return ( *(int*)a - *(int*)b );
}

void main(){
    int M[5][10];

    // Initializing the matrix

    int i,j;
    for(i=0; i<5; i++){
        for(j=0; j<10;j++)
            M[i][j] = 10-j;
    }

    // Clause to request a certain number os threads
    omp_set_num_threads(5);
    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        qsort (M[ID], 10, sizeof(int), compare); 
    }

    // Showing the matrix
    
    for(i=0; i<5; i++){
        for(j=0; j<10;j++){
            printf("%d ", M[i][j]);
        }
        printf("\n");
    }

}
