//takes around 23 seconds to calculate Pi
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main()
{
    const size_t N = 1000000000;
    int i, count = 0;
    double x,y, pi;
    unsigned int tid;
    clock_t start, end;
    start = clock();

    #pragma omp parallel private(i,x,y,tid) shared(count)
    {
      tid = omp_get_thread_num();
      #pragma omp for reduction(+:count)
      for (i = 0; i < N; i++)
      {
          
          x = (double)rand_r(&tid)/RAND_MAX;      // random x coordinate
          y = (double)rand_r(&tid)/RAND_MAX;      // random y coordinate
          
          if ((x*x)+(y*y)<=1)
          {
                  count++;           
          }
      }
    }

    pi = ((double)count/(double)N)*4.0;
    end = clock();

    printf("Time elapsed: %f seconds.\n", (double)(end - start) / CLOCKS_PER_SEC);
    
    printf("pi = %.16f\n", pi);

    return 0;
}

