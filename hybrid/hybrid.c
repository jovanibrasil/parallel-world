#include <stdio.h>
#include "mpich/mpi.h"
#include "omp.h"
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUM_THREADS 7

int cmpfunc (const void * a, const void * b);
void print_vector_matrix(int *m, int l_size, int v_size);

int main(int argc, char** argv){

        int my_rank;        // Identificador do processo.
        int proc_n;         // Número de processo.
        int tasks_num;      // Number of tasks that we have.
        int task_size;      // Size of each task.
        int source;         // Identificador do proc.origem.
        int dest;           // Identificador do proc. destino.
        int tag = 50;       // Tag para as mensagens.
        int *message;
        //int name_len;
        int i = 0;          // Acumulador para loops.
        MPI_Status status;  // Status de retorno.
        //char proc_name[MPI_MAX_PROCESSOR_NAME];

        if(argc == 3){
            tasks_num = atoi(argv[1]);
            task_size = atoi(argv[2]);
        }else{
            return (-1);
        }

        MPI_Init (&argc , & argv);

        // Pega o numero do proc. atual.
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
        // Pega a informacao  da quantidade total de processos.
        MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

        //MPI_Get_processor_name(proc_name, &name_len);

        //printf("There are %d procs!!\n", proc_n);

        message = (int*)malloc(sizeof(int) * task_size * NUM_THREADS);

        // Se eu sou o mestre
        if(my_rank == 0){

                // Inicia contador de tempo
                double init_time = MPI_Wtime();

                // Id da proxima tarefa a ser realizada 
                int next_task = 0;

                // Total de tarefas a ser recebida
                int count_messages = tasks_num;

                // Index para controle de localizacao das tarefas dentro do saco                
                int *index = (int*)malloc(task_size * sizeof(int));

                // Alocacao dinamica e inicializacao randomica da matrix - bag of tasks
                int *bag = (int*)malloc(tasks_num * task_size * sizeof(int));

                int size = tasks_num * task_size;

                // Inicializa matriz.
                for(i=0; i<size; i++){
                    bag[i] = size - i;
                }

                // Mostra matriz inicializada.
                print_vector_matrix(bag, task_size, task_size *tasks_num);

                int r_m = 0, s_m = 0;
               
                // Envia primeira rajada;
                // TODO O que fazer quando não há trabalho suficiente
                // para distribuir a todos os nodos.
             
                int task_block = task_size * NUM_THREADS;
                
                int proc_rank = 1;
                while(proc_rank < proc_n){
                        MPI_Send (&bag[next_task], task_block, MPI_INT, proc_rank, tag, MPI_COMM_WORLD);
                        //printf("MPI task sended to process %d. \n", proc_rank);        
                        printf("Unidades de tarefas %d-%d enviadas para %d. \n", next_task, next_task+task_block-1, proc_rank);
                        s_m++;
                        // The task bag[next_task] is associated with the processes proc_rank.
                        index[proc_rank] = next_task;
                        next_task += task_block;
                        proc_rank++;
                }

                // TODO distribuir localmente

                // Iniciativa no escravo. Mestre faz gerencia dos pedidos dos processos enquanto existe tarefas.
                while(1){
                        // TODO considerar que a quantidade de tarefas muda no final. Processar o resto paralelamente e 
                        // localmente se necessário.
                        // TODO considerar a opção de realizar o trabalho localmente.
                        // Recebe e guarda o trabalho realizado.
                        MPI_Recv (message, task_size * NUM_THREADS, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                        memcpy(&bag[index[status.MPI_SOURCE]], message, task_size * NUM_THREADS * sizeof(int));
                        //count_messages--;                     
                        r_m++;
                        // Testo se eu nao tenho mais tarefas.
                        printf("Sended messages: %d Received messages: %d \n", s_m, r_m);
                        printf("Next task = %d \n", next_task);
                        if(next_task < size){
                                // Envia um novo trabalho para o processo                       
                                MPI_Send (&bag[next_task], task_block, MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
                                // printf("MPI task sended to process %d. \n", status.MPI_SOURCE);
                                printf("Unidades de tarefas %d-%d enviadas para %d. \n", next_task, next_task+task_block-1, proc_rank);
                                index[status.MPI_SOURCE] = next_task;
                                next_task += task_block;
                                s_m +=1;
                        }else{
                            // Sobraram tarefas, faz a ordenação local.
                            printf("Sobraram %d unidades de tarefa. \n", (size - task_block) / task_size);
                            break;
                        }

                }

                // Envia mensagem de suicio para todos os escravos.
                int p = 0;
                for(i=1; i<proc_n; i++){
                        printf("MPI send to process %d. \n", i);
                        MPI_Send (&p, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                        s_m++;
                }

                print_vector_matrix(bag, task_size, task_size * tasks_num);

                //printf("Received messages by master %d\n", r_m);
                //printf("Sended messages by master %d\n", s_m);
                printf("Total time %f\n", MPI_Wtime()-init_time);

        // Caso contrario sou um elemento intermediario
        }else{
                // Recebeu primeiro trabalho, agora só fica esperando mensagem de suicidio.
                while(1){
                        // Recebe mensagem.
                        MPI_Recv(message, task_size * NUM_THREADS, MPI_INT , 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                        // Se for uma mensagem de suicidio (MPI_TAG == -1), caso contrario é a quantidade de tarefas.
                        // TODO considerar quantidade de tarefas. 
                        if(!status.MPI_TAG)
                                break;

                        // Configura a quantidade de threads.
                        omp_set_num_threads(NUM_THREADS);
                        // Divide o processamento em diferentes threads.
                        #pragma omp parallel for schedule(static)
                        for(i=0; i<NUM_THREADS; i++){
                            qsort(&message[i*task_size], task_size, sizeof(int), cmpfunc);
                            // int id = omp_get_thread_num();
                            // int nt = omp_get_num_threads();
                        }

                        // Envia resultado e pede mais trabalho.
                        MPI_Send (message, task_size * NUM_THREADS, MPI_INT, 0, tag, MPI_COMM_WORLD);
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

void print_matrix(int **m){
    int i, j;
    /*
    for(i=0; i<BAG_ROWS; i++){
            for(j=0; j<BAG_COLS; j++){
                        printf("%d ", m[i][j]);
                    }
            printf("\n");
        }
    printf("\n"); */
}

int cmpfunc (const void * a, const void * b){
   return ( *(int*)a - *(int*)b );
}
