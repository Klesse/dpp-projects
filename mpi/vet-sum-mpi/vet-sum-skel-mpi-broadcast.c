#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define NELEM 100

int main(int argc, char *argv[]) {
    int i;
    float sum = 0.0;
    float sum_check = 0.0;
    long int nelem;
    int numtasks;
    int rank;
	unsigned int seedp;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc > 1)
        nelem = atoi(argv[1]);
    else
        nelem = NELEM;

    if (rank == 0) {
        
        float *vet;
        float *partial_sum;
        
        // alocação do vetor
        vet = (float *)malloc(nelem * sizeof(float));
        partial_sum = (float *)malloc(numtasks * sizeof(float));

        if (!vet || !partial_sum) {
            perror("Erro na alocacao do vetor de elementos ou partial_sum.");
            return EXIT_FAILURE;
        }

		for(i=0; i < nelem; i++) 
			//vet[i] = (float)((float)rand_r(&seedp) / (float)RAND_MAX);
            vet[i] = 10;

        // Inicializando sum_check
        for (i = 0; i < nelem; i++)
            sum_check += vet[i];

    
        MPI_Bcast(vet, nelem, MPI_FLOAT, 0 , MPI_COMM_WORLD);
        
        for (int i = 1; i < numtasks; i++) {
            MPI_Recv(&partial_sum[i], 1, MPI_FLOAT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            sum += partial_sum[i];
        }

		printf("Sum: %f\n", sum);
		printf("Checksum: %f\n", sum_check);

        free(vet);
        free(partial_sum);

    } else {
        float *vet_worker;
        float *sum_workers;

        vet_worker = (float *)malloc(nelem * sizeof(float));
        sum_workers = (float *)malloc(numtasks * sizeof(float));

        MPI_Bcast(vet_worker, nelem, MPI_FLOAT, 0 , MPI_COMM_WORLD);

        int division;
        int begin, end;
        division = nelem / (numtasks-1);
        begin = (rank-1) * division;
        if(rank == numtasks - 1){
            end = begin + division + (nelem % (numtasks-1));
        }   
        else
            end = begin + division;

        sum_workers[rank] = 0.0;
        for (i = begin; i < end; i++)
            sum_workers[rank] += vet_worker[i];

        MPI_Send(&sum_workers[rank], 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);

        free(vet_worker);
        free(sum_workers);
    }

    MPI_Finalize();



    return 0;
}
