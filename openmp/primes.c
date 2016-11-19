#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

int main (int argc, const char * argv[]) {
    
    const int intervalo = 5000;
    double starttime, stoptime;

    int i,j,k;
    int prime;
    int total;
	
    starttime = omp_get_wtime(); 

    omp_set_num_threads(8);
	
    #pragma omp parallel private ( i, j, k, prime )
    // schedule controla como as itera��es do loop ser�o divididas
    // entre as threads. 
    // Static - quantidade de trabalho � dividida em quantidades mais pr�ximas poss�veis. 
    // Dynamic - existe uma pilha de trabalho que dinamicamente distribuida em tempo de execucao (run-time). 
    // Guided - Igual a dynamic mas a unidade de trabalho come�a grande vai deiminuindo com o tempo. 
    // 
    // No caso do modo dynamic e guided, podemos definir o tamanho do bloco de trabalho (chunk). Por
    // default o tamanho � 1.
    //
    // Mais informa��es: https://software.intel.com/en-us/articles/openmp-loop-scheduling
    #pragma omp for schedule (dynamic)	
    for (k = 1 ; k <= intervalo ; k++){ 
	total = 0;	
	for ( i = 2; i <= k ; i++ ){
	    prime = 1;
	    for ( j = 2; j < i; j++ ){
                if ( i % j == 0 ){
                    prime = 0;
                    break;
                }
            }
            total = total + prime;
        }  
        printf("O n�mero de primos do intervalo [1-%d] � %d\n", k, total);
    }
	
    stoptime = omp_get_wtime();
    printf("Tempo de execu��o: %3.2f segundos\n", stoptime-starttime);
    return(0);
}
