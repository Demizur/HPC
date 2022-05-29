
#include "lodepng.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>

#define WINDOW_SIZE 3
#define MEDIAN_DIMENSION 3    
#define MEDIAN_LENGTH 9   
#define BLOCK_WIDTH 16  
#define BLOCK_HEIGHT 16

#define R 0
#define G 1
#define B 2
#define A 3

__global__ void median( unsigned char *image, int width,int height, int num_channel, int channel, int copy_A){

    unsigned char out[BLOCK_WIDTH*BLOCK_HEIGHT][MEDIAN_LENGTH];

    int iterator;
    int Half_Of_MEDIAN_LENGTH =(MEDIAN_LENGTH/2)+1;
    int start=MEDIAN_DIMENSION/2;
    int end=start+1;

    int x = blockDim.x * blockIdx.x + threadIdx.x;
    int y = blockDim.y * blockIdx.y + threadIdx.y;
    int tid = threadIdx.y*blockDim.y+threadIdx.x;   

      if(x>=width || y>=height)
        return;
        if (copy_A)
        *(image+(y*width * num_channel)+x * num_channel + A) = image[y*width*num_channel+x*num_channel+A];
            if (x == 0 || x == width - start || y == 0
                || y == height - start) {
            } else {             
                iterator = 0;
                for (int r = x - start; r < x + (end); r++) {
                    for (int c = y - start; c < y + (end); c++) {
                        out[tid][iterator] =*(image+(c*width*num_channel)+r * num_channel + channel);
                        iterator++;
                    }
                }
                      int t,j,i;
                    for ( i = 1 ; i< MEDIAN_LENGTH ; i++) {
                        j = i;
                        while ( j > 0 && out[tid][j] < out[tid][j-1]) {
                            t= out[tid][j];
                            out[tid][j]= out[tid][j-1];
                            out[tid][j-1] = t;
                            j--;
                        }
                    }

                    *(image+(y*width * num_channel)+x * num_channel + channel)= out[tid][Half_Of_MEDIAN_LENGTH-1];
            }  
}

int main() {

  int width, height,n;
  unsigned char *image = stbi_load("image.png",&width,&height,&n,0);

  unsigned char* Input_Image = NULL;
  unsigned char* Output_Image = NULL;
  cudaMalloc((void**)&Input_Image, sizeof(unsigned char)* height * width * n);
  cudaMalloc((void**)&Output_Image, sizeof(unsigned char)* height * width * n);

  cudaMemcpy(Input_Image, image, sizeof(unsigned char) * height * width * n, cudaMemcpyHostToDevice);

  int BlocksPerThread = 16;
  dim3 blockSize(BlocksPerThread, BlocksPerThread, 1);
  dim3 gridSize(width/blockSize.x, height/blockSize.y,1);

  //median filtering
  median <<<gridSize, blockSize>>>(Input_Image, width, height,n,R,0);
  median <<<gridSize, blockSize>>>(Input_Image, width, height,n,G,0);
  median <<<gridSize, blockSize>>>(Input_Image, width, height,n,B,1);
  
  cudaDeviceSynchronize();

  cudaMemcpy(image, Input_Image, sizeof(unsigned char) * height * width * n, cudaMemcpyDeviceToHost);
  cudaFree(Input_Image);
  cudaFree(Output_Image);
  stbi_write_png("median.png", width, height, n, image, width * n);

  return 0;
}