#include <stdio.h>

__global__
void mul(float *d_A, float *d_B, float *d_C, int n_elem){
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  int j = blockIdx.y * blockDim.y + threadIdx.y;
  if (i < n_elem && j < n_elem){
      d_C[i*n_elem+j]=0;
      for(int k=0; k<n_elem; k++)
        d_C[i*n_elem+j] += d_A[i*n_elem+k] * d_B[k*n_elem+j];
  }
}


int main(void){
  float *A, *B, *C;
  float *d_A, *d_B, *d_C;

  int n_elem = 10000;

  size_t tam_mat = n_elem * n_elem * sizeof(float);

  A = (float *)malloc(tam_mat);
  B = (float *)malloc(tam_mat);
  C = (float *)malloc(tam_mat);

  for (int i=0; i< n_elem*n_elem; i++){
    A[i] = 0.1;
    B[i] = 0.2;
  }

  cudaMalloc((void **)&d_A, tam_mat);
  cudaMalloc((void **)&d_B, tam_mat);
  cudaMalloc((void **)&d_C, tam_mat);

  cudaMemcpy(d_A, A, tam_mat, cudaMemcpyHostToDevice);
  cudaMemcpy(d_B, B, tam_mat, cudaMemcpyHostToDevice);

  dim3 block;

  block.x = 16;
  block.y = 16;
  block.z = 1;

  dim3 grid;

  grid.x = (n_elem + block.x -1) / block.x;
  grid.y = (n_elem + block.y -1) / block.y;
  grid.z = 1;

  mul<<<grid, block>>>(d_A, d_B, d_C, n_elem);

  cudaDeviceSynchronize();

  cudaMemcpy(C, d_C, tam_mat, cudaMemcpyDeviceToHost);

  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);

  free(A);
  free(B);
  free(C);

  return 0;
  }