#include <stdio.h>
#include "mpi.h"
 
main(int argc, char** argv){

	int my_rank;  /* Identificador do processo */
	int proc_n;   /* Número de processos */
	int source;   /* Identificador do proc.origem */
	int dest;     /* Identificador do proc. destino */
	int tag = 50; /* Tag para as mensagens */
	 
	//char message[100]; /* Buffer para as mensagens */
	MPI_Status status; /* Status de retorno */
	
	int message;	 

	MPI_Init (&argc , & argv);
	 
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /* Pega o numero do proc. atual */
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n); /* Pega a informacao  da quantidade total de processos */
	
	// Se eu sou o primeiro processo do pipeline
	if(my_rank == 0){
		// Crio mensagem e envio para o proximo processo
		//sprintf(message, "Recebendo msg do processo %d!", my_rank);
		message = 1;
		MPI_Send (&message, 1, MPI_INT, my_rank+1, tag, MPI_COMM_WORLD);
	
	// Se eu sou o ultimo processo do pipeline		
	}else if(my_rank == proc_n-1){
		// Recebo a mensagem e mostro		
	   	MPI_Recv (&message, 1, MPI_INT , my_rank-1, tag, MPI_COMM_WORLD, &status);
		printf("%d\n", message);	

	// Caso contrario sou um elemento intermediario
	}else{
		// Recebo a mensagem, processo e mando para o proximo processo
	   	MPI_Recv (&message, 1, MPI_INT , my_rank-1, tag, MPI_COMM_WORLD, &status);
		printf("%d\n", message);
		//sprintf(message, "Recebendo msg do processo %d!", my_rank);
		message += 1;
		MPI_Send (&message, 1, MPI_INT, my_rank+1, tag, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
