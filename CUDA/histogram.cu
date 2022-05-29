
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

__global__ void Hist(int height, int width,  uint8_t *d_image, unsigned int *d_res)
{
        int globalidx = threadIdx.x + blockDim.x * blockIdx.x;
	long int size = height * width;	
	// uint8_t tid = threadIdx.x;

	if (globalidx < size)
	{

		unsigned char value = d_image[globalidx];
		int bin = value % 256;
		atomicAdd(&d_res[bin], 1);
		//__syncthreads();				
	}
}

int main(int argc, char **argv)
{
	int width, height, bpp, size;
	FILE *fp;
	fp = fopen("hist.txt", "w");

	uint8_t* h_image_init = stbi_load("grey_cat.jpg", &width, &height, &bpp, 3);		
	size = height * width;

	uint8_t* h_image = (uint8_t *) malloc(sizeof(uint8_t) * size);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			h_image[j*width + i] = (h_image_init[j*width*3 + i*3] + \
						h_image_init[j*width*3 + i*3 + 1] + \
						h_image_init[j*width*3 + i*3 + 2]) / 3.;		
		}
	}

	uint8_t *d_image;
	unsigned int *d_res;
	unsigned int *h_res = (unsigned int *) malloc(sizeof(unsigned int) * 256);
	cudaMalloc(&d_image, sizeof(uint8_t) * size);
	cudaMalloc(&d_res, sizeof(unsigned int) * 256);
	cudaMemset(d_res, 0, sizeof(unsigned int) * 256);
	
	cudaMemcpy(d_image, h_image, sizeof(uint8_t) * size, cudaMemcpyHostToDevice);
  int block_size, grid_size;
	block_size = 256;
  grid_size = size / 256;

  dim3 dimBlock(block_size);
  dim3 dimGrid(grid_size);

	Hist<<<dimGrid, dimBlock>>>(height, width, d_image, d_res);
	cudaDeviceSynchronize();	

	cudaMemcpy(h_res, d_res, sizeof(unsigned int) * 256, cudaMemcpyDeviceToHost);
	
	for (int i = 0; i < 256; i++)
	{
		fprintf(fp, "%d\t", h_res[i]);
	}
	fprintf(fp, "\n");

	free(h_image);
	free(h_res);
	cudaFree(d_image);
	cudaFree(d_res);
	fclose(fp);

	return 0;	
}