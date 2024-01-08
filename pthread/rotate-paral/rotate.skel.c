/* 
 ** Universidade Federal de São Carlos
 ** Departamento de Computação
 ** Prof. Hélio Crestana Guardia
 ** Programação Paralela e Distribuída
 */

/*
 ** Programa : 
 ** Objetivo:
 ** Obs: para compilar, é preciso "linkar" a biblioteca matemática (gcc ... -lm)
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
// #include <sys/resource.h>


#define WIDTH  3840
#define HEIGHT 2160


unsigned int img_orig [HEIGHT][WIDTH];
unsigned int img_rot [HEIGHT][WIDTH];

int 
main(int argc, char **argv)
{
	int nlin = HEIGHT;
	int ncol = WIDTH;
	int i,j,x,y,xc,yc;
	float angle = 90;
	float s,c;
	int fd;
	char fname[128];


	if(argc<3) {
		printf("Uso: %s nome_arquivo angulo  // (sem extensão .rgba)\n",argv[0]);
		exit(0);
	}

	sprintf(fname,"%s.rgba", argv[1]);	
	if((fd = open(fname,O_RDONLY))==-1) {
		printf("Erro na abertura do arquivo %s\n",argv[1]);
		exit(0);
	}
   angle = atof(argv[2]);
   if(angle <= -360 || angle >= 360) {
     	printf("Erro:  -360 < angulo < 360\n");
     	exit(0);
   }   


	// trabalhar com 2 imagens, a original e a transformada...

	// Lê arquivo de imagem
	// Neste caso, vamos apenas mudar pixel de local, mantendo seus componentes RGBA
	for(i=0; i < nlin; i++) 
		for(j=0; j < ncol; j++) 
			read(fd,&img_orig[i][j], 4);

	// Leitura em bloco (nlin * ncol * 4) pode gerar melhor desempenho?
	// Dividir em blocos de 4k? (tamanho do bloco do sistema de arquivos)
	
	// Fecha arquivo lido
	close(fd);


	// Aplicar rotação em torno de um ponto
	// x' = (x-x0) cosΘ - (y-y0) sinΘ + x0;
	// y' = (x-x0) sinΘ + (y-y0) cosΘ + y0;

	// Considerando rotação em torno do ponto central
	xc = ncol/2;
	yc = nlin/2;

	// Convertendo angulo em radianos
    // M_PI: https://www.gnu.org/software/libc/manual/html_node/Mathematical-Constants.html
	angle = angle * M_PI / 180.0;
	s = sin(angle);
	c = cos(angle);

	for(i=0; i < nlin; i++)
		for(j=0; j < ncol; j++) {

			// faz a rotação dos pixels no sentido horario, em torno do ponto central...

			x = (j-xc)*c -(i-yc)*s + xc;
			y = (j-xc)*s +(i-yc)*c + yc;

			// verifica se o ponto está dentro da área da imagem antes de atribuir `a matriz
         if(x >= 0 && x < ncol && y >= 0 && y < nlin)
            img_rot[y][x] = img_orig[i][j];
		}

	// gravar imagem resultante
	sprintf(fname,"%s.%s.rgba", argv[1], argv[2]);	
	fd=open(fname,O_WRONLY|O_CREAT,0644);

	// write(fd,&ncol,2); write(fd,&nlin,2);
	for(i=0; i < nlin; i++) 
		for(j=0; j < ncol; j++) 
			write(fd, &img_rot[i][j], 4);
	
	close(fd);
	
	return 0;
}


