#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define SIZE 100000
#define DELTA 10000

void print(int *vector, int size);
void bs(int *vector, int size);
void validate(int *vector, int size);
int* interleaving(int *vector, int size);

main(int argc, char** argv){

	int my_rank;  /* Identificador do processo */
	int proc_n;   /* Número de processos */
	int source;   /* Identificador do proc.	origem */
	int dest;     /* Identificador do proc. destino */
	int tag = 50; /* Tag para as mensagens */
	int vector_size;
	int f_rank;
	double tx;
	
	int vector[SIZE];
        vector_size = SIZE;
	double t = MPI_Wtime();

	MPI_Status status;

	MPI_Init (&argc , & argv);
	 
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // Pega o numero do proc. atual 
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n); // Pega a informacao  da quantidade total de processos 

	// Se eu nao sou raiz, recebo o vetor.
	if ( my_rank != 0 ){
   		// Nao sou a raiz, tenho pai. Recebo um vetor.
		int er = MPI_Recv(vector, SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		f_rank = status.MPI_SOURCE;
		// Descubro o tamanho da mensagem recebida.
   		MPI_Get_count(&status, MPI_INT, &vector_size);   
	}
	else{
		// Sou o elemento raiz e portanto gero o vetor (ordem reversa).		
		int i;

		for(i=0; i<vector_size; i++){
			vector[i] = vector_size - i;
		}
	}

	int i = 0;
	printf("%d \n", vector_size);	
	// Conquista - ordena vetor com bubble sort.
	if ( vector_size <= DELTA ){
		bs(vector, vector_size);  
	}
	// Divisao - divide o vetor em partes e distribui para os filhos.  
	else{

                // Divide - quebra o vetor em duas partes e manda para os filhos        
                int size = vector_size/2;
                // Localizacao dos filhos - igual a um binary heap  
                int rank = 2*my_rank + 1;
                //printf("rank %d\n", rank);
                MPI_Send(&vector[0], size, MPI_INT, rank, tag, MPI_COMM_WORLD);
                MPI_Send(&vector[size], vector_size-size, MPI_INT, rank+1, tag, MPI_COMM_WORLD);

                // Recebe - recebe partes ordenadas dos filhos
                MPI_Recv(vector, size, MPI_INT, rank, tag, MPI_COMM_WORLD, &status);
                MPI_Recv(&vector[size], size, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &status);
	
		int *new_vector;
	
		// Intercala os vetores.
		new_vector =  interleaving(vector, vector_size);	

		// Retorna valores para pai.
        	if ( my_rank !=0 ){
               		// Retorno vetor ordenado para meu pai.
               		i = 1; 
                	MPI_Send(&new_vector[0], vector_size, MPI_INT, f_rank, tag, MPI_COMM_WORLD); 
       		}
		//print(new_vector, vector_size);
    	}

	// Retorna valores para pai.
	if ( my_rank !=0 && i == 0){
		// Retorno vetor ordenado para meu pai.
		MPI_Send(&vector[0], vector_size, MPI_INT, f_rank, tag, MPI_COMM_WORLD); 
	}
	else if (my_rank == 0){
   		// Sou raiz, mostro o vetor.
		printf("Time %f\n", MPI_Wtime() - t);
	}
	
	MPI_Finalize();

	return 0;
}

void print(int *vector, int size){
	int i;
	for(i=0; i<size; i++){
		printf("%d ", vector[i]);
	}
	printf("\n");
}

void bs(int *vetor, int n){
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou ){
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
   }
}

int *interleaving(int vetor[], int tam)
{
	int *vetor_auxiliar;
	int i1, i2, i_aux;

	vetor_auxiliar = (int *)malloc(sizeof(int) * tam);

	i1 = 0;
	i2 = tam / 2;

	for (i_aux = 0; i_aux < tam; i_aux++) {
		if (((vetor[i1] <= vetor[i2]) && (i1 < (tam / 2)))
		    || (i2 == tam))
			vetor_auxiliar[i_aux] = vetor[i1++];
		else
			vetor_auxiliar[i_aux] = vetor[i2++];
	}

	return vetor_auxiliar;
}
