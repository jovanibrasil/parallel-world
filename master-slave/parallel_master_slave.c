#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

#define BAG_ROWS 100000
#define BAG_COLS 1000

void print_matrix(int **bag);
int cmpfunc (const void * a, const void * b);

main(int argc, char** argv){

	int my_rank;  /* Identificador do processo */
	int proc_n;   /* Número de processos */
	int source;   /* Identificador do proc.origem */
	int dest;     /* Identificador do proc. destino */
	int tag = 50; /* Tag para as mensagens */
	 
	MPI_Status status; /* Status de retorno */
	
	int *message = (int*)malloc(sizeof(int) * BAG_COLS);	 

	MPI_Init (&argc , & argv);
	 
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /* Pega o numero do proc. atual */
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n); /* Pega a informacao  da quantidade total de processos */

	//printf("There are %d procs!!\n", proc_n);
	
	// Se eu sou o mestre
	if(my_rank == 0){
		
		// Inicia contador de tempo
		double init_time = MPI_Wtime();

		// Id da proxima tarefa a ser realizada 
		int next_task = 0;
		
		// Total de tarefas a ser recebida
		int count_messages = BAG_ROWS;

		// Index para controle de localizacao das tarefas dentro do saco		
		int *index = (int*)malloc(BAG_COLS * sizeof(int));	

		// Alocacao dinamica e inicializacao randomica da matrix - bag of tasks
		int **bag = (int**)malloc(BAG_ROWS * sizeof(int*));

		int i, j;
		for(i=0; i<BAG_ROWS; i++){
			bag[i] = (int*)malloc(BAG_COLS * sizeof(int)); 
			int v = BAG_COLS;
			for(j=0; j<BAG_COLS; j++, v--){
				bag[i][j] = v; 
			}
		}

		// Print da matriz
		//print_matrix(bag);
	
		// Envia primeira rajada;
		for(i=0; i<proc_n-1 && i<BAG_ROWS; i++){
			//printf("%d\n", i+1);
			MPI_Send (bag[i], BAG_COLS, MPI_INT, i+1, tag, MPI_COMM_WORLD);
			index[i+1] = i;
			next_task += 1;	
		}
		//int r_m = 0, s_m = 0;
		// Faz gerencia dos pedidos dos processos enquanto existe tarefas
		while(count_messages){

			// Recebe trabalho
			MPI_Recv (message, BAG_COLS, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			memcpy(bag[index[status.MPI_SOURCE]], message, BAG_COLS * sizeof(int));			
			count_messages--;			
			//r_m +=1;
			// Testo se eu nao tenho mais tarefas
			if(next_task < BAG_ROWS){
				// Envia um novo trabalho para o processo			
				MPI_Send (bag[next_task], BAG_COLS, MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
				index[status.MPI_SOURCE] = next_task;
				next_task += 1;	
				//s_m +=1;
			}
		}  		

		// Mata processos
		for(i=1; i<proc_n; i++){
			MPI_Send (&tag, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			//s_m +=1;
		}

		// Print da matriz (organizada)
		//print_matrix(bag);
		
		//printf("Received messages by master %d\n", r_m);
		//printf("Sended messages by master %d\n", s_m);
		printf("Total time %f\n", MPI_Wtime()-init_time);	
	
	// Caso contrario sou um elemento intermediario
	}else{
		// Recebeu primeiro trabalho, agora só fica esperando mensagem de suicidio
		while(1){
			// Recebo a mensagem, processo e mando para o proximo processo
			MPI_Recv(message, BAG_COLS, MPI_INT , 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			// Se MPI_TAG == 1 entao faz suicidio
			if(!status.MPI_TAG){
				break;
			}
			
			// Faz ordenacao do vetor
			qsort(message, BAG_COLS, sizeof(int), cmpfunc);
			
			// Envia resultado / Pega mais trabalho
			MPI_Send (message, BAG_COLS, MPI_INT, 0, tag, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
}

void print_matrix(int **m){
	int i, j;

	for(i=0; i<BAG_ROWS; i++){
		for(j=0; j<BAG_COLS; j++){
			printf("%d ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int cmpfunc (const void * a, const void * b){
   return ( *(int*)a - *(int*)b );
}
