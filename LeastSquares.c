#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void rand_init_vector(double * vector, size_t N) {
    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        vector[i] = 10.0*(double)rand() / RAND_MAX;
     }

}

double * malloc_vector(size_t N)
{
    double * vector = (double *)malloc(N * sizeof(double));

    return vector;
}

void generate_data(double *x, double *y, size_t N, double a, double b) {

  for (int i = 0; i < N; i++) {
    double noise = (double) rand() / RAND_MAX;
    noise += (double) (rand() % 2);
     y[i] = a*x[i] + b + noise;
  }

}

int main() {
    size_t N = 100000;
    double *y, *x;
    double a = 1;
    double b = 2;
    double a_estim, b_estim;
    double start, end;

    x =  malloc_vector(N);
    y =  malloc_vector(N);
    rand_init_vector(x,N);

    generate_data(x,y,N, a,b );

    start = omp_get_wtime();
     double SUMx = 0, SUMy = 0, SUMxx = 0, SUMxy = 0;

    #pragma omp parallel for reduction (+:SUMx,SUMy,SUMxx,SUMxy)
    for (int i = 0; i < N; i++) {
        SUMx += x[i];
        SUMy += y[i];
        SUMxx += x[i]*x[i];
        SUMxy += x[i]*y[i];
    }

    SUMx = SUMx/N;
    SUMy = SUMy/N;
    SUMxx = SUMxx/N;
    SUMxy = SUMxy/N;

    a_estim = (SUMxy - SUMx*SUMy) / (SUMxx - SUMx*SUMx);
    b_estim = SUMy - a_estim * SUMx;
    end = omp_get_wtime();

    printf ("y = %lfx + %lf\n", a_estim, b_estim);

    printf("Time elapsed (seconds): %lf\n", end - start);

    free(x);
    free(y);
    return 0;
}