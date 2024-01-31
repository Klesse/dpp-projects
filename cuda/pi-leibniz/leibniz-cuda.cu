#include <stdlib.h>
#include <stdio.h>
#include <cuda_runtime.h>

// Função de kernel CUDA para o cálculo da série de Leibniz
__global__
void iter(int n, int* digits, const long int d){
    // Cálculo do índice da thread
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    // Verifica se o índice da thread está dentro do intervalo
    if (i < n){
        int div, mod, remainder, signal, digit;

        // Determina o sinal para os termos alternados na série
        if (i % 2 == 0){
            signal = 1;
        } else {
            signal = -1;
        }

        remainder = 4;

        // Realiza o cálculo da série de Leibniz para cada thread
        for(digit = 0; digit < d + 11 && remainder; ++digit) {
            div = remainder / (2 * i + 1);
            mod = remainder % (2 * i + 1);

            // Usa atomicAdd para atualizar o array compartilhado de forma paralela
            atomicAdd(&digits[digit], signal * div);
            remainder = mod * 10;
        }
    }
}

// Função para calcular pi usando a série de Leibniz com CUDA
void pi(char* output, const long int n, const long int d, int num_threads, int num_blocks) {
    // Aloca memória para o array que armazenará os dígitos
    int *digits = (int *)malloc((d+11)*sizeof(int));
    int *d_digits;
    long int digit, i;

    // Inicializa o array de dígitos com zeros
    for(digit = 0; digit < d + 11; ++digit) {
        digits[digit] = 0;
    }

    // Aloca memória na GPU e copia os dados para a GPU
    cudaMalloc(&d_digits, (d+11)*sizeof(int));
    cudaMemcpy(d_digits, digits, (d+11)*sizeof(int), cudaMemcpyHostToDevice);

    // Inicia o kernel CUDA para o cálculo em paralelo
    iter<<<num_blocks, num_threads>>>(n, d_digits, d);

    // Copia os resultados de volta da GPU para a CPU
    cudaMemcpy(digits, d_digits, (d+11)*sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(d_digits);

    // Pós-processamento dos resultados para obter o valor final de pi
    for(i = d + 11 - 1; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
        if(digits[i] < 0) {
            digits[i - 1]--;
            digits[i] += 10;
        }
    }

    // Ajuste de arredondamento
    if(digits[d + 1] >= 5) {
        ++digits[d];
    }

    // Realiza arredondamento adicional e formatação da saída
    for(i = d; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    // Formata a string de saída
    output[0] = digits[0] + '0';
    output[1] = '.';
    for(i = 1; i <= d; i++)
        output[i + 1] = digits[i] + '0';
    output[d + 2] = 0;

    // Libera a memória alocada
    free(digits);
}

// Função principal
int main(int argc, char* argv[]) {
    int n, d, num_threads, num_blocks;

    FILE *in, *out;

    // Abre os arquivos de entrada e saída
    in = fopen("pi.in", "r");
    out = fopen("pi.out", "w");

    // Lê os valores de entrada do arquivo
    fscanf(in, "%d%d%d%d", &n, &d, &num_threads, &num_blocks);

    char output[d + 4];

    // Chama a função pi para calcular e formatar a aproximação de pi
    pi(output, n, d, num_threads, num_blocks);

    // Escreve o resultado no arquivo de saída
    fprintf(out, "%s\n", output);
    fflush(out);

    // Fecha os arquivos
    fclose(in);
    fclose(out);

    return EXIT_SUCCESS;
}
