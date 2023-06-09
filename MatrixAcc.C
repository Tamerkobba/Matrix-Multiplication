#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <openacc.h>

void matrixMul(int *a, int *b, int *c, int N) {
  #pragma acc data copyin(a[0:N*N], b[0:N*N]) copyout(c[0:N*N])
  {
    #pragma acc kernels loop independent
    for (int row = 0; row < N; row++) {
      #pragma acc loop independent
      for (int col = 0; col < N; col++) {
        c[row * N + col] = 0;
        #pragma acc loop seq
        for (int k = 0; k < N; k++) {
          c[row * N + col] += a[row * N + k] * b[k * N + col];
        }
      }
    }
  }
}
double getTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

int main() {
  int N = 2048;
  size_t size = N * N * sizeof(int);

  // host copies of matrices a, b & c
  int *h_a = (int *)malloc(size);
  int *h_b = (int *)malloc(size);
  int *h_c = (int *)malloc(size);

  // Setup input values
  for (int i = 0; i < N * N; i++) {
    h_a[i] = rand() % 100;
    h_b[i] = rand() % 100;
  }

  double start = getTime();
  matrixMul(h_a, h_b, h_c, N);
  double end = getTime();

  printf("Result Matrix:\n");
  // Print a subset of the result matrix
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      printf("%d ", h_c[i * N + j]);
    }
    printf("\n");
  }

  printf("Execution Time: %.6f seconds\n", end - start);

  free(h_a);
  free(h_b);
  free(h_c);

  return 0;
}
