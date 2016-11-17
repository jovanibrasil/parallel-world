#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COLS 10
#define ROWS 20

void print_matrix(int **bag);
int cmpfunc (const void * a, const void * b);

int main(int argc, char **argv){

	clock_t begin = clock();

	// Alocacao dinamica e inicializacao randomica da matriz
	int **bag = (int**)malloc(ROWS * sizeof(int*));

	int i, j;
	for(i=0; i<ROWS; i++){
		bag[i] = (int *)malloc(COLS * sizeof(int)); 
		int v = COLS;
		for(j=0; j<COLS; j++, v--){
			bag[i][j] = v;
		}
	}

	// Mostra resultado
	print_matrix(bag);	

	// Ordena usando quicksort
	for(i=0; i<ROWS; i++){
		qsort(bag[i], COLS, sizeof(int), cmpfunc);
	}

	clock_t end = clock();

      	// Mostra resultado final
	print_matrix(bag);

	printf("Total time %f\n", (double)(end - begin) / CLOCKS_PER_SEC);

}

void print_matrix(int **m){
	int i, j;

	for(i=0; i<ROWS; i++){
		for(j=0; j<COLS; j++){
			printf("%d ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int cmpfunc (const void * a, const void * b){
   return ( *(int*)a - *(int*)b );
}

