#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

float dotprod(float * a, float * b, size_t N)
{
    int i, tid;
    float sum;

#pragma omp parallel for shared(a, b, N) private(i, tid) reduction(+:sum) 
    for (i = 0; i < N; ++i)
    {
        tid = omp_get_thread_num();
        sum += a[i] * b[i];
        printf("tid = %d i = %d\n", tid, i);
    }

    return sum;
}

int main (int argc, char *argv[])
{
    const size_t N = 100;
    int i;
    float sum = 0.0;
    float a[N], b[N];

    for (i = 0; i < N; ++i)
    {
        a[i] = b[i] = (double)i;
    }

    sum = dotprod(a, b, N);

    printf("Sum = %f\n",sum);

    return 0;
}
