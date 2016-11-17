#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define SIZE 1000000
#define TX 0.15
#define DELTA 130000

void print(int *vector, int size);
void bs(int *vector, int size);
void validate(int *vector, int size);
void interleaving(int *vector, int *new_vector, int b, int c, int size);
int compare (const void * a, const void * b);

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

	//printf("hello\n");

	//vector = (int*)malloc(sizeof(int) * max_size);

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
		qsort(vector, vector_size, sizeof(int), compare);  
	}
	// Divisao - divide o vetor em partes e distribui para os filhos.  
	// Ordena uma parte localmente.
	else{
		/* 
			|------|---------|----------|
			       b         c
		*/

		int b, c;
		// Calcula posicoes no vetor a serem ordenadas localmente.
		//b = (int)(vector_size * (double)TX);
		b = (double)vector_size / proc_n;		

		c = b + (vector_size - b) / 2;
	
		// Localizacao dos filhos - igual a um binary heap. 
		int rank = 2 * my_rank + 1;

		// Manda as partes dos filhos.
		MPI_Send(&vector[b], c-b, MPI_INT, rank, tag, MPI_COMM_WORLD);
		MPI_Send(&vector[c], vector_size-c, MPI_INT, rank+1, tag, MPI_COMM_WORLD); 	

		// Ordena sua parte.
		qsort(vector, b, sizeof(int), compare);

		// Recebe os vetores dos filhos.
		MPI_Recv(&vector[b], c-b, MPI_INT, rank, tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&vector[c], vector_size-c, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &status);	
	 
		//int new_vector[vector_size];
		int *new_vector = (int*)malloc(sizeof(int) * vector_size);	

		// Intercala os vetores.
		interleaving(vector, new_vector, b, c, vector_size);	

		// Retorna valores para pai.
        	if ( my_rank !=0 ){
               		// Retorno vetor ordenado para meu pai.
               		i = 1;
			//printf("%d \n", my_rank); 
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

int compare (const void * a, const void * b){
  return ( *(int*)a - *(int*)b );
}

void interleaving(int *vector, int *v, int b, int c, int size){

	int i1, i2, i3, i_aux;

	i1 = 0;
	i2 = b;
	i3 = c;

	for (i_aux = 0; i_aux < size; i_aux++) {
		// i1 <= i2 ^ (i1 < b v i2 = c) 	
		if(((vector[i1] <= vector[i2]) && (i1 < b)) || (i2 == c)){
			// i1 < i3 ^ i3 = size
			if((vector[i1] <= vector[i3]) || (i3 == size)){
				v[i_aux] = vector[i1++];
			}
			// i1 < i2 ^ i1 > i3 
			else {
				v[i_aux] = vector[i3++];
			}
		}
		else{
			// i2 <= i3 v i3 = size 	
			if((vector[i2] <= vector[i3]) || (i3 == size)){
				v[i_aux] = vector[i2++];
			}
			else {
				v[i_aux] = vector[i3++];
			}	
		}
	}
}
