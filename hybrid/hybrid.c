/*
 * TODO Versao (1) somente com MPI.
 * TODO Versao (2) usando MPI e OpenMP 
 * 
*/

#include <stdio.h>
#include "mpich/mpi.h"
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define BAG_ROWS 14
#define BAG_COLS 10
#define NUM_THREADS 7

void print_matrix(int **bag);
int cmpfunc (const void * a, const void * b);
void print_vector_matrix(int *m);

int main(int argc, char** argv){

        // TODO Parametrizar.
        
        if(argc == 3){
            // Quantidade de tarefas
            // Tamanho de cada tarefa
            printf("%s \n", argv[1]);
            printf("%s \n", argv[2]);
        }else{
            return (-1);
        }

        int my_rank;  /* Identificador do processo */
	int proc_n;   /* Número de processos */
	int source;   /* Identificador do proc.origem */
	int dest;     /* Identificador do proc. destino */
	int tag = 50; /* Tag para as mensagens */
	 
	MPI_Status status; /* Status de retorno */
	
	int *message = (int*)malloc(sizeof(int) * BAG_COLS * NUM_THREADS);	 

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
		//int **bag = (int**)malloc(BAG_ROWS * sizeof(int*));

		//int i, j;
		//for(i=0; i<BAG_ROWS; i++){
		//	bag[i] = (int*)malloc(BAG_COLS * sizeof(int)); 
		//	int v = BAG_COLS;
		//	for(j=0; j<BAG_COLS; j++, v--){
		//		bag[i][j] = v; 
		//	}
		//
                int *bag = malloc(BAG_ROWS * BAG_COLS * sizeof(int));
                int size = BAG_ROWS * BAG_COLS;
                int i=0;
                for(i=0; i<size; i++){
	            bag[i] = size - i;
                }

                print_vector_matrix(bag);

		// Print da matriz
		//print_matrix(bag);
	 
		int r_m = 0, s_m = 0;

		// Envia primeira rajada;
                // TODO Quando eu nao tenho trabalho suficiente
                // para distribuir trabalho para todos os cores.
		int proc_rank = 1;
                while(proc_rank < proc_n){
                        MPI_Send (&bag[next_task], BAG_COLS * NUM_THREADS, MPI_INT, proc_rank, tag, MPI_COMM_WORLD);
			printf("MPI task sended to process %d. \n", proc_rank);
                        s_m++;
                        // The task bag[next_task] is associated with the processes proc_rank
                        index[proc_rank] = next_task;
			next_task += NUM_THREADS * BAG_COLS;
                        proc_rank++;        
		}

                // TODO distribuir localmente

		// Iniciativa no escravo. Mestre faz gerencia dos pedidos dos processos enquanto existe tarefas.
		while(1){
                        // TODO Se utilizado HT, o tamanho da mensagem dobra
			// Recebe e guarda o trabalho realizado
			MPI_Recv (message, BAG_COLS * NUM_THREADS, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			memcpy(&bag[index[status.MPI_SOURCE]], message, BAG_COLS * NUM_THREADS * sizeof(int));			
			//count_messages--;			
			r_m++;
			// Testo se eu nao tenho mais tarefas
			printf("Sended messages: %d Received messages: %d \n", s_m, r_m);
                        printf("Next task = %d \n", next_task);                       
                        if(next_task < size){
				// Envia um novo trabalho para o processo			
				MPI_Send (&bag[next_task], BAG_COLS * NUM_THREADS, MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
			        printf("MPI task sended to process %d. \n", status.MPI_SOURCE);
                                index[status.MPI_SOURCE] = next_task;
				next_task += NUM_THREADS * BAG_COLS;	
				s_m +=1;
			}else{
                            // Sobraram tarefas, faz a ordenação local.
                            // TODO calculo errado
                            printf("Sobraram %d tarefas. \n", BAG_ROWS - next_task / NUM_THREADS);
                            break;     
                        }

                }  		

		// Mata processos
                int p = 0;
		// TODO Paralelizar utilizando OpenMP.
                for(i=1; i<proc_n; i++){
                        printf("MPI send to process %d. \n", i);
                        MPI_Send (&p, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			//s_m +=1;
		}

                print_vector_matrix(bag);

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
			MPI_Recv(message, BAG_COLS * NUM_THREADS, MPI_INT , 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			// Se MPI_TAG == 1 entao faz suicidio
			if(!status.MPI_TAG){
                                break;
			}
			
			// Faz ordenacao do vetor
                        int i;
                        for(i=0; i<NUM_THREADS; i++){
                            qsort(&message[i*BAG_COLS], BAG_COLS, sizeof(int), cmpfunc);
                        }
			// Envia resultado / Pega mais trabalho
			MPI_Send (message, BAG_COLS * NUM_THREADS, MPI_INT, 0, tag, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
        return (0);
}

void print_vector_matrix(int *m){
    int i = 0, j = BAG_COLS, size = BAG_COLS * BAG_ROWS; 
    printf("[0] ");
    int l = 0;
    for(i=0; i<size; i++){
        printf("%d ", m[i]);
        j--;
        if(j == 0){
            printf("\n");
            j = BAG_COLS;
            l++;
            if(i<size-1){
                printf("[%d] ", l);
            }
        }
    }

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
