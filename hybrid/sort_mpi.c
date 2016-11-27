#include <stdio.h>
#include "mpich/mpi.h"
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int cmpfunc (const void * a, const void * b);
void print_vector_matrix(int *m, int l_size, int v_size);
void validade_job(int *m, int l_size, int v_size);
void bsort(int n, int * vetor);

int main(int argc, char** argv){
    
    int my_rank;        // Identificador do processo.
    int proc_n;         // Número de processo.
    int tasks_num;      // Number of tasks that we have.
    int task_size;      // Size of each task.
    int num_threads;    // Number of threads.
    int source;         // Identificador do proc.origem.
    int dest;           // Identificador do proc. destino.
    int tag = 50;       // Tag para as mensagens.
    int *message;
    int i = 0;          // Acumulador para loops.
    int sort_type = 0;  // Tipo de ordenação. (0) quicksort (1) bubblesort. 
    MPI_Status status;  // Status de retorno.

    // Organiza parametrizacao.
    if(argc == 5){
        // params(numero_de_tarefas, tamanho_da_tarefa, quantidade_de_threads)
        tasks_num = atoi(argv[1]);
        task_size = atoi(argv[2]);
        num_threads = atoi(argv[3]);
        
        int ret = strcmp(argv[4], "bsort");

        if(!ret)
           sort_type = 1;

    }else{
        return (-1);
    }

    // Inicializa o MPI.
    MPI_Init (&argc , & argv);
    // Pega o numero do proc. atual.
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    // Pega a informacao  da quantidade total de processos.
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
    
    // Inicializa mensagem.
    message = (int*)malloc(sizeof(int) * task_size * num_threads);

    // printf("My rank %d!!!", my_rank);

    // Se eu sou o mestre.
    if(my_rank == 0){

        // Inicia contador de tempo.
        double init_time = MPI_Wtime();

        // Id da proxima tarefa a ser realizada. 
        int next_task = 0;

        // Index para controle de localizacao das tarefas dentro do saco.                
        int *index = (int*)malloc(task_size * sizeof(int));

        int bag_size = tasks_num * task_size;       // Tamanho do saco de tarefas.
        int r_m = 0, s_m = 0;                       // Contadores de mensagens enviadas e recebidas. 
        int task_block = task_size * num_threads;   // Tamanho do bloco a ser enviado para os escravos.
        
        // Total de tarefas a ser recebida.
        int exit_counter = bag_size;

        // Alocacao dinamica e inicializacao randomica da matrix - bag of tasks.
        int *bag = (int*)malloc(bag_size * sizeof(int));

        // Inicializa matriz.
        for(i=0; i<bag_size; i++)
            bag[i] = bag_size - i;

        // Mostra matriz inicializada.
        // print_vector_matrix(bag, task_size, bag_size);
                    
        // Envia primeira rajada. Consideremos que ha trabalho suficiente para isso.
        int proc_rank = 1;
        // Quantidade de tarefas a serem enviadas. 
        int task_blocks = num_threads;             
        while(proc_rank < proc_n){
                MPI_Send (&bag[next_task], task_block, MPI_INT, proc_rank, task_blocks, MPI_COMM_WORLD);
                // Associa tarefa ao processo destino.
                index[proc_rank] = next_task;       
                next_task += task_block;
                proc_rank++;
        }

        // Tamanho do do buffer para mensagem.
        int msg_size = num_threads * task_size;
        
        // Mestre faz gerencia dos pedidos dos processos enquanto existem tarefas.
        while(1){
            
            // Recebe mensagem verificando seu tamanho.
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &msg_size);
            MPI_Recv (message, msg_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            memcpy(&bag[index[status.MPI_SOURCE]], message, msg_size * sizeof(int));

            exit_counter -= msg_size;

            // Se todos os vetores já foram ordenados.            
            if(!exit_counter)
                break;
            
            // Se não há trabalho para todos os nodos. 
            if(next_task + task_block > bag_size && exit_counter > 0){
                // Agora cada nodo recebe apenas uma unidade de trabalho.
                task_blocks = 1;  
                task_block = task_size;    
            }

            // Envia trabalho para o nodo.
            if(next_task + task_block <= bag_size){
                MPI_Send(&bag[next_task], task_block, MPI_INT, status.MPI_SOURCE, task_blocks,MPI_COMM_WORLD);
                index[status.MPI_SOURCE] = next_task;
                next_task += task_block;
            }
            
        }

        // Envia mensagem de suicio para todos os escravos.
        for(i=1; i<proc_n; i++)
                MPI_Send (&i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        // Mostra saco de tarefas ordenado.
        // print_vector_matrix(bag, task_size, task_size * tasks_num);
        	
        printf("Time %f\n", MPI_Wtime()-init_time);

	validade_job(bag, task_size, task_size * tasks_num);

    // Caso contrario sou um elemento intermediario.
    }else{
        int task_n;
        // Recebeu primeiro trabalho, agora só fica esperando mensagem de suicidio.
        while(1){
            task_n = num_threads;

            // Recebe mensagem.
            MPI_Recv(message, task_size * num_threads, MPI_INT , 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // (MPI_TAG)  (-1) = suicidio (>0) quantidade de tarefas.
            if(!status.MPI_TAG)
                break;
            else
                task_n = status.MPI_TAG;			

            // Configura a quantidade de threads.
            // omp_set_num_threads(task_n);
            // Divide o processamento em diferentes threads.
            //#pragma omp parallel for schedule(static)
            for(i=0; i<task_n; i++){
		// int thread_id = omp_get_thread_num();
                // printf("thread_id = %d\n", thread_id);
		if(sort_type == 0)
                    qsort(&message[i*task_size], task_size, sizeof(int), cmpfunc);
                else
                    bsort(task_size, &message[i*task_size]);
            }
            // Envia resultado e pede mais trabalho.
            MPI_Send (message, task_size * task_n, MPI_INT, 0, tag, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return (0);
}

void print_vector_matrix(int *m, int l_size, int v_size){
    int i = 0, j = l_size, size = v_size; 
    printf("[0] ");
    int l = 0;
    for(i=0; i<size; i++){
        printf("%d ", m[i]);
        j--;
        if(j == 0){
            printf("\n");
            j = l_size;
            l++;
            if(i<size-1){
                printf("[%d] ", l);
            }
        }
    }
}

void validade_job(int *m, int l_size, int v_size){
    int j, i = l_size; 
    for(j=0; j<v_size-1; j++){
        if(m[j] > m[j+1] && i != 1){
            printf("[Error]  %d > %d \n", m[j], m[j+1]);
            return;
        }
        i--;
        if(!i){
            i = l_size;
        }
    }
}

void bsort(int n, int * vetor){
    int c =0, d, troca, trocou =1;
    while ((c < (n-1)) & trocou ){
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1]){
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
            }
        c++;
    }
}

int cmpfunc (const void * a, const void * b){
   return ( *(int*)a - *(int*)b );
}
