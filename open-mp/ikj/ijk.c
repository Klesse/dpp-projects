#include <stdio.h>

#define TAM 1024

float matrix_a[TAM][TAM];
float matrix_b[TAM][TAM];
float matrix_c[TAM][TAM];

int main(int argc, char **argv)
{
  int i,j,k;

  for (i = 0; i < TAM; i++) {        // inicia matrizes
    for (j = 0; j < TAM; j++) {
      matrix_a[i][j] = 0.1f;
      matrix_b[i][j] = 0.2f;
      matrix_c[i][j] = 0.0f;     // pode ser substituído por memset (0...)
    }
  }

  for (i = 0; i < TAM; i++)      // percorre as linhas de C
    for (j = 0; j < TAM; j++)
      for (k = 0; k < TAM; k++)  // percorre colunas da linha de A e linhas da coluna de B
        // percorre colunas de C
        matrix_c[i][j] += matrix_a[i][k] * matrix_b[j][k];


  for (i = 0; i < TAM; i++) {
    for (j = 0; j < TAM; j++) {
      printf("%f ", matrix_c[i][j]);
    }
    printf("\n");
  }

  return 0;
 }