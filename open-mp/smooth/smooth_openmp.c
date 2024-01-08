/* 
 * Universidade Federal de São Carlos
 * Departamento de Computação
 * Prof. Hélio Crestana Guardia
 */

/*
 * Programa : smooth (stencil de suavização pela média)
 * Objetivo: 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define WIDTH  3840
#define HEIGHT 2160

int mr [WIDTH][HEIGHT];
int mg [WIDTH][HEIGHT];
int mb [WIDTH][HEIGHT];
int ma [WIDTH][HEIGHT];
int mr2 [WIDTH][HEIGHT];
int mg2 [WIDTH][HEIGHT];
int mb2 [WIDTH][HEIGHT];
int ma2 [WIDTH][HEIGHT];

int 
main(int argc, char **argv)
{
	int i,j;
	int fdi, fdo;

	int nlin = HEIGHT;
	int ncol = WIDTH;

	if(argc<2) {
		printf("Uso: %s nome_arquivo\n",argv[0]);
		exit(0);
	}
	if((fdi=open(argv[1],O_RDONLY))==-1) {
		printf("Erro na abertura do arquivo %s\n",argv[1]);
		exit(0);
	}

	// lê arquivo de imagem, supondo tamanho 512x512
	
	// (ao menos) 2 abordagens: 
	// - ler pixels byte a byte, colocando-os em matrizes separadas
	//	- ler pixels (32bits) e depois usar máscaras e rotações de bits para o processamento.

	// Ordem de leitura dos bytes (componentes do pixel): RGBA?
	for(i=0; i < nlin; i++) {
			read(fdi,&mr[i],ncol);
			read(fdi,&mg[i],ncol);
			read(fdi,&mb[i],ncol);
			read(fdi,&ma[i],ncol);
	}
	close(fdi);

	// Aplicar filtro (estêncil), separadamente para componentes R, G, B e A
	printf("Começou a processar...\n");
	// Tratar: linhas 0 e n-1; colunas 0 e n-1
	#pragma omp parallel for private(j)
	for(i = 1; i < nlin -1; i++) {
		for(j=1; j < ncol -1; j++) {

			mr2[i][j] = ( mr[i-1][j-1] + mr[i-1][j] + mr[i-1][j+1] +
	      		        mr[i]  [j-1] + mr[i]  [j] + mr[i]  [j+1] +
      			        mr[i+1][j-1] + mr[i+1][j] + mr[i+1][j+1] ) / 9;
		


			mg2[i][j] = ( mg[i-1][j-1] + mg[i-1][j] + mg[i-1][j+1] +
	      		        mg[i]  [j-1] + mg[i]  [j] + mg[i]  [j+1] +
      			        mg[i+1][j-1] + mg[i+1][j] + mg[i+1][j+1] ) / 9;
		

			mb2[i][j] = ( mb[i-1][j-1] + mb[i-1][j] + mb[i-1][j+1] +
	      		        mb[i]  [j-1] + mb[i]  [j] + mb[i]  [j+1] +
      			        mb[i+1][j-1] + mb[i+1][j] + mb[i+1][j+1] ) / 9;
		
			ma2[i][j] = ( ma[i-1][j-1] + ma[i-1][j] + ma[i-1][j+1] +
	      		        ma[i]  [j-1] + ma[i]  [j] + ma[i]  [j+1] +
      			        ma[i+1][j-1] + ma[i+1][j] + ma[i+1][j+1] ) / 9;
		}
		
	}

	printf("Terminou de processar...\n");

	// gravar imagem resultante
	fdo=open("out.rgba",O_WRONLY|O_CREAT,0644);

	for(i=0; i < nlin; i++) {
			write(fdo,&mr2[i],ncol);
			write(fdo,&mg2[i],ncol);
			write(fdo,&mb2[i],ncol);
			write(fdo,&ma2[i],ncol);
		
	}
	close(fdo);
	
	return 0;
}


