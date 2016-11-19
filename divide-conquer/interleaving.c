#include <stdio.h>
#include "stdlib.h"

void print(int *vector, int size){
	int i = 0;
	for(i; i<size; i++){
		printf("%d ", vector[i]);
	}
	printf("\n");
}

// Compara de a é menor do que b e c 
int compare(int a, int b, int c){
	if(a < b && b < c || a < c && c < b){
		return 1;
	}else{
		return 0;
	}

}

int *interleaving3(int vetor[], int b, int c, int tam){

	int *vetor_auxiliar;

	int i1, i2, i3, i;

	vetor_auxiliar = (int *)malloc(sizeof(int) * tam);

	i1 = 0;
	i2 = b;
	i3 = c;

	// Para cada posição do vetor
	for (i = 0; i < tam; i++) {


		if(((vetor[i1] <= vetor[i2]) && (i1 < (b))) || (i2 == (c))){
			
			if((vetor[i1] <= vetor[i3]) || (i3 == tam)){
				vetor_auxiliar[i] = vetor[i1++];
			}
			else {
				vetor_auxiliar[i] = vetor[i3++];
			}
		}
		else{
			
			if((vetor[i2] <= vetor[i3]) || (i3 == tam)){
				vetor_auxiliar[i] = vetor[i2++];
			}
			else {
				vetor_auxiliar[i] = vetor[i3++];
			}	
		}

	}

	return vetor_auxiliar;

}

int *interleaving2(int v[], int l1, int l2, int size){

	int i = 0;
	int j = l1;
	int k = l2;

	int index = 0;

	int *n = (int*)malloc(sizeof(int)*size);

	while(1){	
		if(i < l1 && j < l2 && k < size){
			print(n, size);	
			// Neste caso vamos intercalar os tres vetores ate
			// que haja somente dois.
			
			// Se v[i] é o menor
			if(compare(v[i], v[j], v[k])){
				n[index] = v[i];
				index++; i++;	
			}
			// Se v[j] é o menor
			else if(compare(v[j], v[i], v[k])){
				n[index] = v[j];
				index++; j++;
			}
			// Se v[k] é o menor
			else if(compare(v[k], v[i], v[j])){
				n[index] = v[k];
				index++; k++;
			}
			// Se v[i] == v[j] == v[k]
			else if(v[i] == v[k] && v[k] == v[j]){
				n[index] = v[i]; index++; 
				n[index] = v[j]; index++;
				n[index] = v[k]; index++; 
				i++; j++; k++;		
			}
		}else{
			// Agora temos somente dois vetores	
							
		

			break;
		}
	}
	return n;

}

int *interleaving(int vetor[], int l, int size){

        int *v;
        int i = 0;
        int j = l;
        int k = 0;

	printf("Primeiro elemento do menor vetor = %d. \n", vetor[l]);
	printf("Primeiro elemento do segundo vetor = %d. \n", vetor[0]);

	v = (int*)malloc(sizeof(int)*size);

        while(1){
                // Se os dos dois vetores tem elementos
                if(j < size && i < l){
                        // Se i < j
			
			printf("Comparando %d com %d. \n", vetor[i], vetor[j]);

                        if(vetor[i] == vetor[j]){
                                // adiciona ambos os elementps
                                v[k] = vetor[i];
                                k++;
                                v[k] = vetor[j];
                                i++;
                                j++;
                        }else if(vetor[i] < vetor[j]){
                                // adiciona elemento vetor[i]
                                v[k] = vetor[i];
                                i++;
                        }else{
                                // adiciona elemento vetor[j]
                                v[k] = vetor[j];
                                j++;
                        }
                        k++;
			print(v, size);
		}else{
			// Se apenas um deles tem elementos
			if(i < l){
				printf("Sobrou no vetor maior! \n");
				for(i; i < l; i++, k++){
					v[k] = vetor[i];
				}
				break;
			}
			if(j < size){
				printf("sobrou no vetor menor! \n");
				for(j; j < size; j++, k++){
					v[k] = vetor[j];
				}
				break;
			}
		}
        }
        return v;
}

int main(int argc, char** argv){

	int a[] = { 1, 2, 3, 1, 2, 3, 1, 2, 3 };
	int c[] = { 1, 2, 7, 3, 4, 8, 5, 6, 9 };
	int *b;

	print(c, 9);

	//b = interleaving2(a, 3, 6, 9);
	b = interleaving3(c, 3, 6, 9);

	print(b, 9);

	return 0;
}



