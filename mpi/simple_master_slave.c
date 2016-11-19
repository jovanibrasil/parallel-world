#include <stdio.h>
#include "mpi.h"
#define TAREFAS 4 

main(int argc, char** argv){

	int my_rank;  /* Identificador do processo */
	int proc_n;   /* Número de processos */
	int source;   /* Identificador do proc.origem */
	int dest;     /* Identificador do proc. destino */
	int tag = 50; /* Tag para as mensagens */
	 
	//char message[100]; /* Buffer para as mensagens */
	MPI_Status status; /* Status de retorno */
	
	int message;	 
	int saco[TAREFAS]; // Saco de trabalho

	MPI_Init (&argc , & argv);
	 
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /* Pega o numero do proc. atual */
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n); /* Pega a informacao  da quantidade total de processos */
	
	// Se eu sou o mestre
	if(my_rank == 0){
		// Crio mensagem e envio para o proximo processo
		//int cont_tarefas = TAREFAS;
		// Pre-condicao: numero de tarefas igual ao numero de processos
		int i = 0;
		for(i; i < TAREFAS; i++){
			message = saco[i];
			MPI_Send (&message, 1, MPI_INT, i+1, tag, MPI_COMM_WORLD);
		}

		for(i = 0; i < TAREFAS; i++){
			MPI_Recv (&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			saco[status.MPI_SOURCE-1] = message;
			printf("saco[%d] tem valor %d.\n", status.MPI_SOURCE, saco[status.MPI_SOURCE-1]);		
		}	
		
	// Caso contrario sou um elemento intermediario
	}else{
		// Recebo a mensagem, processo e mando para o proximo processo
	   	MPI_Recv (&message, 1, MPI_INT , 0, tag, MPI_COMM_WORLD, &status);

		//sprintf(message, "Recebendo msg do processo %d!", my_rank);
		message = my_rank;
		//printf("My rank is %d\n", my_rank);
		MPI_Send (&message, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
