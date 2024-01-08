#!/bin/bash

rm -r t1.txt 2> /dev/null
rm -r t2.txt 2> /dev/null
rm -r t3.txt 2> /dev/null

echo -e "Informe N linhas de A:\n"
read LIN_A
echo -e "Informe N Colunas de A == N Linhas de B:\n"
read COL_A
echo -e "Informe N Colunas de B:\n"
read COL_B



gcc -Wall t1.c -o t1 -O3 -fopenmp
gcc -Wall t2.c -o t2 -O3 -fopenmp
gcc -Wall t3.c -o t3 -O3 -fopenmp

for i in {1,2,4,8,16,32,64,128}; do
        for j in {1..3}; do
                echo "$i thread(s) / Programa t$j"
                export OMP_NUM_THREADS=$i
                { time ./t$j  $LIN_A $COL_A $COL_B; } 2>> t$j.txt
                echo -e "\n" 2>> $j.txt
        done
        echo
done

python plot.py
