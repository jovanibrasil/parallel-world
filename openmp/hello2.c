#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
 
int main (int argc, char *argv[]) {
    int th_id, nthreads;

    // Maneiras de configurar o numero de threads
    // omp_set_num_threads(4);
    // setenv OMP_NUM_THREADS 2
    // export OMP_NUM_THREADS=2

    // Consulta o nro de cores da maquina
    int num_procs = omp_get_num_procs();
    printf("Number of procs = %d \n", num_procs);

    #pragma omp parallel private(th_id, nthreads) num_threads(4)
    {
        th_id = omp_get_thread_num();
        nthreads = omp_get_num_threads();
        printf("Hello World from thread %d of %d threads.\n", th_id, nthreads);
    }

    getchar();
    return EXIT_SUCCESS;
}


