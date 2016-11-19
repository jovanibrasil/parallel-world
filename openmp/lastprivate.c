#include <stdio.h>
#include <omp.h>

#define N 20

int main() {
	int i, soma = 0;
	omp_set_num_threads(4);	
	
        // firstprivate - valores carregam as atribuições feitas antes da entrada 
        // na região paralela. 
        // 
        // lastprivate - valores são carregados para as variáveis no término da região.
        //
        // default(shered | none) - define o comportamento das veariaveis de fora do 
        // escopo da região paralela. Por default as variáveis são definidas como shared
        // mesmo quando default não é chamado. Se definirmos default(none), obrigamos a 
        // explícita definição do escopo de todas as variáveis, caso contrário, a cláusula
        // causa um erro. Escopos de variáveis podem ser private, shared, reduction,
        // firstprivate ou lastprivate.
        // 
        // atomic and critical - atomic permite mais otimizações pelo compilador.  

        #pragma omp parallel for firstprivate(soma) lastprivate(soma)
	for(i=0; i<N; i++) {
		soma += i;
		if(i == ((omp_get_thread_num()+1)*(N/omp_get_num_threads())-1))
			printf("%d: %d\n", omp_get_thread_num(), soma);
	}
	printf("%d\n", soma);
	return 0;
}
