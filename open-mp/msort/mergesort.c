/* 
 ** Universidade Federal de São Carlos
 ** Departamento de Computação
 ** Prof. Hélio Crestana Guardia
 ** Programação Paralela e Distribuída
 */

/*
 ** Programa : ordenação de vetor
 ** Objetivo: mergesort usando paralelização com tasks
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <assert.h> 

// tamanho default para o vetor
#define LSIZE 10000000

// tamanho mínimo do vetor realizar chamadas recursivas
#define LIM 128
// #define LIM 1


float *list;
float *aux;


int 
compare ( const void *e1, const void *e2 ) 
{
	// double *i1 = (double*) e1;
	// double *i2 = (double*) e2;
	float *i1 = (float*) e1;
	float *i2 = (float*) e2;

	return ((*i1 < *i2) ? -1 : (*i1 > *i2) ? +1 : 0);
}


void 
print_list (float * lista, int list_size)
{
	int i;

	for(i=0; i < list_size; i++)
		printf("%f  ",lista[i]);
	printf("\n");
}

void 
troca (float *x, float *y)
{
	float temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

void 
merge (int start, int middle, int end) 
{
	int i;
	int l = start;
	int r = middle+1;

	i=start;
	while (l <= middle && r <= end) { // enquanto ha dados nas 2 partes
			
		if(list[l] < list[r])
			aux[i] = list[l++];
		else 
			aux[i] = list[r++];
		i++;
	}
	while (l <= middle) {
		aux[i] = list[l++];
		i++;
	}
	while (r <= end) {
		aux[i] = list[r++];
		i++;
	}

	// copia segmento ordenado do vetor auxiliar para vetor original
	for(i=start; i <= end; i++) 
		list[i] = aux[i];
	// copiando com memcpy
	// memcpy (&(list[start]), &(aux[start]), (end - start +1) * sizeof(float));
}

void 
msort (int m, int n)
{
	int k;

	// para poucos elementos, talvez valha a pena evitar a recursividade
	if (n-m < LIM)
		qsort((void*)&list[m], n-m+1, sizeof(int), compare);
	else {

		if (m == n)
			return;
		
		if (m+1 == n) {
			if(list[m] > list[n]) 
				troca (&list[m],&list[n]);
		} else {
			// divide ao meio
			k = (m+n)/2;
            merge (m,k,n);

            
            #pragma omp task if (k > LIM)
			msort (m, k);

            #pragma omp task if (k > LIM)
			msort (k+1, n);
	
            #pragma omp taskwait
			merge	(m,k,n);
            
            
		}
	}
}

float 
check_list(float *list, int list_size )
{
	float sum;

	sum = 0;
	for(int i = 0; i < list_size; i++ )
		sum = sum + list[i];

	return sum;
}

float 
kahan_sum(float x[], int N)  
{
	float s = x[0];
	float c = 0.0;

	for (int i = 1; i < N; i++) {
		float y = x[i] - c;
		float t = s + y;
		c = (t - s) - y;
		s = t;
	}   

	return s;
}


int 
main(int argc, char **argv)
{
	int i;
	int list_size = 0;
	int debug = 0;
  // unsigned int seedp; // se for usar rand_r()

	if(argc>1) {
		list_size=atoi(argv[1]);
		assert(list_size >= 0);
	} else
		list_size = LSIZE;

	if(argc>2)
		debug = !strcmp(argv[2],"-d");

	// aloca vetores 
	list=(float *)malloc(list_size*sizeof(float));
	aux=(float *)malloc(list_size*sizeof(float));

	// Gera lista de números aleatórios. Usar rand_r()?
	// Não mudar a semente para ter sempre os mesmos valores (pseudo) aleatórios
	// srand(time(NUL)));
	for( i=0; i < list_size ; i++ )
		list[i] = (float)rand() / (float)RAND_MAX;
		// list[i] = (float)rand_r(&seedp) / (float)RAND_MAX;
    
	// imprime lista inicial
	if(debug)	print_list(list, list_size);

	// if(debug)
  	printf( "Check: sum of %d elements = %f\n", list_size, kahan_sum( list, list_size ) );
   #pragma omp parallel
   {
    #pragma omp single
    msort(0, list_size-1);
   }
	

	// imprime lista ordenada
	if(debug)	print_list(list, list_size);

	// if(debug)
		printf( "Check: sum of %d elements = %f\n", list_size, kahan_sum( list, list_size ) );
		// printf( "Check: sum of %d elements = %f\n", list_size, check_list( list, list_size ) );

	// testa se há elementos fora de ordem
	for(i = 0; i < list_size-1; i++)
		if(list[i]>list[i+1]) {
			printf("Erro...\n");
			break;
		}

	return 0;
}