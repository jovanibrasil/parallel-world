#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000000

void bs(int n, int *vetor);
void print(int n, int *vetor);

int main(int argc, char *argv){
	int *v;
	
	clock_t begin = clock();
	
	v = (int*)malloc(sizeof(int) * SIZE);
	
	int i=0;
	for(i; i<SIZE;i++){
		v[i] = SIZE - i;
	}

	//print(SIZE, v);	

	bs(SIZE, v);

	//print(SIZE, v);
	clock_t end = clock();

        printf("Total time %f\n", (double)(end - begin) / CLOCKS_PER_SEC);

}

void print(int n, int *vetor){
	int i =0;
	for(i=0; i<n;i++){
		printf("%d ", vetor[i]);
	}
	printf("\n");
}

void bs(int n, int *vetor){
	
	int c=0, d, troca, trocou =1;

    	while (c < (n-1) & trocou ){
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

