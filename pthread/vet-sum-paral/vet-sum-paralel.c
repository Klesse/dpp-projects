/* 
 ** Universidade Federal de São Carlos
 ** Departamento de Computação
 ** Prof. Hélio Crestana Guardia
 */

/*
 ** Programa : soma dos elementos de um vetor
 ** Comando: 
 ** Objetivo: observar passagem de valores para função da thread.
              Nesse caso, usa-se vetor em que cada thread preenche 
              os resultados da sua operação. 
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 
#include <limits.h> 
#include <time.h>


#define NELEM (1<<25)
#define N_THREADS 8


// Variável atômica. Pode ser incrementada sem mutex, usando opearação específica
double total = 0;
pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

float *vet;


typedef struct args_type{
	int ind;
	int nelem;
} args_type;

// Funcao da thread: o que passar como parâmetro?
void *
soma(void *arg)
{
	int nl, li, lf;
	args_type *args;
	args=(args_type*)arg;
	long int ind = (long int)(args->ind);
	long int nelem_p = (long int)(args->nelem);
	//printf("%ld\n", nelem_p);

	// Como tratar a soma local? Retorna como parâmetro?
	// Salva de maneira atômica em variável global?
	// Usa vetor global de somas parciais?
	

	nl = nelem_p/N_THREADS;
	li = ind*nl;
	lf = li+nl;

	if (ind == N_THREADS - 1)
		lf += nelem_p - lf;

	for(int j=li; j<lf; j++)
	{
		pthread_mutex_lock(&_mutex);
		total += vet[j];
		pthread_mutex_unlock(&_mutex);
	}


	pthread_exit(NULL);

}


int
main(int argc, char *argv[])
{
	long int i;
	long int nelem;
	unsigned int seedp;
	int status;
	args_type args;

	int num_threads = N_THREADS;
	// vetor de pthread_t para salvamento dos identificadores das threads
	pthread_t *threads; 

	if(argc > 1)
		nelem = atoi(argv[1]);
	else
		nelem = NELEM;

	// passando o número de threads como argumanto para a função main...

	if(argc > 2) {

		num_threads = atoi(argv[2]);
		if(num_threads <= 0) {
			printf("Númreo de threads inválido...\n");
			return(EXIT_FAILURE);
		}
	} 
	// aloca memória para o vetor de thread IDs
	threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));	


	// alocação do vetor
	vet = (float *)malloc(nelem * sizeof(float));

	if (!vet) {
		perror("Erro na alocacao do vetor de elementos.");
		return EXIT_FAILURE;
	}

	for(int i=0; i < nelem; i++) 
		vet[i] = (float)((float)rand_r(&seedp) / (float)RAND_MAX);


	// gerar sempre a mesma sequência de valores? 
	// srand(time(NULL));

	// atribuição de valores (0<val<=1) aos elementos do vetor 
	// Atenção para a função de geração de valores aleatórios: rand_r é 'thread safe'

	// Será que vale a pena paralelizar a geração de valores também?
	// Numa aplicação real, provavelmente leria dados de arquivo ou estes seriam gerados no código...



	// Como fazer a soma em paralelo?

	// 1o: Comentar código acima :-)
	// Criar as threads, atribuindo uma parte do vetor para soma por cada uma delas
	// O que passar como parâmetro? Intervalo do vetor cujos valores cada uma vai calcular...

	// Loop de criacao das threads
	for (int i=0; i < num_threads; i++) {
		args.ind = i;
		args.nelem = nelem;
		status = pthread_create(&threads[i], NULL, soma, (void *)&args);

		if (status) {
			printf("Falha da criacao da thread %d (%d)\n", i, status);
			return (EXIT_FAILURE);
		}
	}


	// Juntar as somas parciais? Em qual tarefa?
	// loop de pthread_join...

	// Como coletar as somas parciais calculadas pelas threads?
	// Thread retorna valor?  Usar vetor global?
	// Usar variável global incrementada com exclusão mútua?
	// Usar variável global incrementada com instrução atômica?

	// loop de espera pelo término da execução das threads
	for (int i=0; i < num_threads; i++) {

		// join recebendo a soma parcial de cada thread
		status = pthread_join(threads[i], NULL);

		if (status) {
			printf("Erro em pthread_join (%d)\n",status);
			break;
		}
	}


	printf("Soma: %f\n",total);

	// libera o vetor de ponteiros para as threads
	// free(threads);

	// libera o vetor de valores
	free(vet);

	return(0);
}



