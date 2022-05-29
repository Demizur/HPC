#include <stdio.h>

__global__ void Init(int n, int *d_hist)
{
    int globalIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if(globalIdx < n) d_hist[globalIdx] = 0;
}

__global__ void Histogram(int *d_hist, int *d_image, long int i)
{
    long int idx = blockIdx.x * blockDim.x + threadIdx.x;
    //printf("hist:%d\n", blockIdx.x);//, blockDim.x, threadIdx.x);
    int tmp;

    tmp = d_image[idx];
    atomicAdd(&(d_hist[tmp]), 1);
    __syncthreads();
}

int main(int ac, char **av)
{
    int height = 441;
    int width = 350;
 
    int *h_image = (int*)malloc(sizeof(int) * height * width);
    int *h_hist = (int *)malloc(sizeof(int)*256);

 
    FILE *fp;
    int ch;
    fp = fopen("greysacaleimage.csv", "r");
    long int i = 0;
    int tmp = 0;
    while (!feof(fp) && !ferror(fp))
    {
        ch = getc(fp);
        if (ch != EOF)
        {
            if (ch < 32)
              tmp = 0;
            else if (ch != 32)
            {
                if (tmp == 0) tmp += ch - 48;
                else if (tmp != 0) tmp = tmp * 10 + ch - 48;
            }
            else if(ch == 32)
            {
                h_image[i] = tmp;
                tmp = 0;
                i++;
            }
        }
    }
    printf("\n");
    fclose(fp);

    int *d_image;
    int *d_hist;
  
    cudaMalloc(&d_image, sizeof(int) * height * width);
    cudaMalloc(&d_hist, sizeof(int)*256);

    cudaMemcpy(d_image, h_image, sizeof(int)*width*height, cudaMemcpyHostToDevice);
    cudaMemcpy(d_hist, h_hist, sizeof(int)*256, cudaMemcpyHostToDevice);
 
    Init<<<1, 256>>>(256, d_hist);
    Histogram<<<1058, 1024>>>(d_hist, d_image, i);
 
    cudaMemcpy(h_image, d_image, sizeof(int)*width*height, cudaMemcpyDeviceToHost);
    cudaMemcpy(h_hist, d_hist, sizeof(float)*256, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();

    FILE *hist;
    hist = fopen("histogram.csv","w");
    for (int i = 0; i < 256; i++) fprintf(hist, "%d ", h_hist[i]);
 
    free(h_image);
    free(h_hist);
    cudaFree(d_image);
    cudaFree(d_hist);

    return(0);
}