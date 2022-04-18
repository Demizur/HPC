#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define ITERATION_LIMIT 10000
#define EPS 0.000001

void zero_init_vector(double * vector, size_t N)
{
    for (int i = 0; i < N; i++)
    {
        vector[i] = 0.0;
        
    }
}

//initialise diagonal matrix
void init_matrix(double ** matrix, size_t N)
{
    srand(time(NULL));

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            matrix[i][j] = rand();
        }
        matrix[i][i] = 1;
    }
}

void rand_init_vector(double * vector, size_t N) {
    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        vector[i] = (double)rand() / RAND_MAX;
     }

}

double ** malloc_matrix(size_t N)
{
    double ** matrix = (double **)malloc(N * sizeof(double *));
    
    for (int i = 0; i < N; ++i)
    {   
        matrix[i] = (double *)malloc(N * sizeof(double));
    }
    
    return matrix;
}

double * malloc_vector(size_t N)
{
    double * vector = (double *)malloc(N * sizeof(double));

    return vector;
}

void free_matrix(double ** matrix, size_t N)
{
    for (int i = 0; i < N; ++i)
    {   
        free(matrix[i]);
    }
    
    free(matrix);
}

void jacobi(double **A, double * b, double * x, size_t N) 
{
    double * x1;
    x1 = malloc_vector(N);
    for (int i = 0; i < N; i++) 
    {
	   x1[i] = 1 ;
    }

	for (int k = 0; k < ITERATION_LIMIT; k++) {   
        x1 = x;
        #pragma omp parallel for
        for (int i = 0; i < N; i++) 
        {
        double  sigma = 0;
        for (int j = 0; j < N; j++) 
        {
            if (j != i) 
            {
            sigma += A[i][j]*x[j];
            } 
        }
        x[i] = (b[i] - sigma)/A[i][i];
        }
    }
}

int main() {

    double start, finish;

    double ** A, *b, *x;

    size_t N = 4; //matrix size

    A = malloc_matrix(N);
    init_matrix(A, N);
    b =  malloc_vector(N);
    rand_init_vector(b,N);

    x = malloc_vector(N);
    rand_init_vector(x,N);

   
    start = clock();

    jacobi(A,b,x,N);

    finish = clock();

    free_matrix(A,N);
    free(b);

    printf("Time elapsed: %f seconds.\n", (double)(finish - start) / CLOCKS_PER_SEC);

    return 0;
}