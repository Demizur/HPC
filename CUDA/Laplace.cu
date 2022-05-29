#include <stdio.h>

const int N = 50;
#define max_iter 500

__global__ void Init(float * dA)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;

    if(idx < N*N)
    { 
        if (blockIdx.x != 0) dA[idx] = 0;
        else 
        {
            if ((threadIdx.x == 0) | (threadIdx.x == N-1)) dA[idx] = 0;
            else dA[idx] = 1;
        }
    }  
}

__global__ void Step(float *T_old, float *T_new)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    int up, down, right, left;

    if(idx < N*N)
    { 
        if((blockIdx.x != 0) && (blockIdx.x !=  N-1))
        {
            if((threadIdx.x != 0) && (threadIdx.x !=  N-1))
            {
                right = (blockIdx.x + 1) * blockDim.x + threadIdx.x;
                left = (blockIdx.x - 1) * blockDim.x + threadIdx.x;            
                up = blockIdx.x * blockDim.x + threadIdx.x + 1;
                down = blockIdx.x * blockDim.x + threadIdx.x - 1;
             
                T_new[idx] = 0.25* (T_old[up] + T_old[down] + T_old[right] + T_old[left]);
            }
        }
    }  
}
int main()
{
    float *T_old;
    float *T;

    cudaMallocManaged(&T_old, sizeof(float)*N*N);
    cudaMallocManaged(&T, sizeof(float)*N*N);

    Init<<<N,N>>>(T_old);
    Init<<<N,N>>>(T);
 
    int k = 0;
    while(k< max_iter){
        Step<<<N,N>>>(T_old, T);
        Step<<<N,N>>>(T, T_old);
        k++;
    }

    cudaDeviceSynchronize();

    FILE *fp=NULL;
    fp = fopen("heatmap.txt","w");

    for (int j=0; j < N; j++)
    {
        for (int i=0; i < N; i++)
        {
            fprintf(fp, "%f ", T[i + j * N]);
        }
        fprintf(fp, "\n");
    }

    cudaDeviceSynchronize();
 
    fclose(fp);
    cudaFree(T_old);
    cudaFree(T);

    return 0;
}