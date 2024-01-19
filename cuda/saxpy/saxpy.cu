
#include <stdio.h>

__global__
void saxpy(int n, int a, float *d_x, float *d_y){
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n)
    d_y[i] = d_x[i] * a + d_y[i];
}


int main(void){
  int N = 10000;
  float *x, *y, *d_x, *d_y;

  x = (float *)malloc(N*sizeof(float));
  y = (float *)malloc(N*sizeof(float));

  cudaMalloc(&d_x, N*sizeof(float));
  cudaMalloc(&d_y, N*sizeof(float));

  for(int i=0;i<N;i++){
    x[i] = 2;
    y[i] = 3;
  }

  cudaMemcpy(d_x, x, N*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_y, y, N*sizeof(float), cudaMemcpyHostToDevice);

  int blockSize = 1024;
  int numBlocks = (N+blockSize-1)/blockSize;

  saxpy<<<numBlocks,blockSize>>>(N,10,d_x,d_y);


  cudaMemcpy(x, d_x, N*sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(y, d_y, N*sizeof(float), cudaMemcpyDeviceToHost);

  printf("First element of y: %f",y[0]);

  cudaFree(d_x);
  cudaFree(d_y);

}
