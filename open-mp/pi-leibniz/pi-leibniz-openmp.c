#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

// Função para calcular uma aproximação de pi usando a série de Leibniz
void pi(char* output, const long int n, const long int d) {
    long int digits[d + 11]; // Array para armazenar os dígitos calculados
    long int digit, i;
    int signal;


    // Inicializa o array digits com zeros
    for (digit = 0; digit < d + 11; ++digit) {
        digits[digit] = 0;
    }

    signal = 1;
    // Loop para calcular os dígitos de pi usando a série de Leibniz
    #pragma omp parallel for private(signal, digit)
    for (i = 0; i <= n; ++i) {
    long unsigned int remainder, div, mod;
      if (i % 2 == 0){
        signal = 1;
      } else {
        signal = -1;
      }
        remainder = 4;
        for (digit = 0; digit < d + 11 && remainder; ++digit) {
            div = remainder / (2 * i + 1);
            mod = remainder % (2 * i + 1);

            #pragma omp atomic
            digits[digit] += (signal * div);

            remainder = mod * 10;
        }
        // signal *= -1; // Não pode paralelizar o loop externo por conta desta variável
    }

    // Ajusta os dígitos para obter a aproximação final de pi
    for (i = d + 11 - 1; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
        if (digits[i] < 0) {
            digits[i - 1]--;
            digits[i] += 10;
        }
    }

    // Arredonda o último dígito conforme necessário
    for (i = d + 1; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    // Converte os dígitos em uma string e armazena em output
    output[0] = digits[0] + '0';
    output[1] = '.';
    for (i = 1; i <= d; i++)
        output[i + 1] = digits[i] + '0';
    output[d + 2] = 0;
}

int main(int argc, char* argv[]) {
    int n, d;

    FILE* in, *out;

    // Abre o arquivo de entrada "pi.in" para leitura
    in = fopen("pi.in", "r");
    // Abre o arquivo de saída "pi.out" para escrita
    out = fopen("pi.out", "w");

    // Lê os valores de n e d a partir do arquivo de entrada
    fscanf(in, "%d%d", &n, &d);

    char output[d + 4];

    // Chama a função pi para calcular a aproximação de pi
    pi(output, n, d);

    // Escreve o resultado no arquivo de saída
    fprintf(out, "%s\n", output);
    fflush(out);

    // Fecha os arquivos
    fclose(in);
    fclose(out);

    return EXIT_SUCCESS;
}
