
#include <stdio.h>

__global__
void saxpy(int n, int a, float *x, float*y){
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  if (index < n)
    y[index] = x[index] * a  + y[index]; 

}


int main(void){
  int N=10000;
  float *x, *y;

  cudaMallocManaged(&x, N*sizeof(float));
  cudaMallocManaged(&y, N*sizeof(float));

    for (int i=0;i<N;i++){
    x[i] = 2.0f;
    y[i] = 3.0f;
  }
  printf("First element of y %f\n",y[0]);

  int blockSize = 1024;
  int numberBlocks = (N+blockSize-1)/blockSize;

  saxpy<<<numberBlocks,blockSize>>>(N,10,x,y);

  cudaDeviceSynchronize();

  printf("First element of y %f\n",y[0]);

  cudaFree(x);
  cudaFree(y);

}
