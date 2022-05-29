
#include "lodepng.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>

#define BLUR_SIZE 7
#define R 0
#define G 1
#define B 2
#define A 3

__global__ void blur(unsigned char* in, unsigned char* out, int width, int height, int num_channel, int channel, int copy_A) {

  int col = blockIdx.x * blockDim.x + threadIdx.x;
  int row = blockIdx.y * blockDim.y + threadIdx.y;

  if(col < width && row < height) {
    int pixVal = 0;
    int pixels = 0;
    if (copy_A)
      out[row*width*num_channel+col*num_channel+A] = in[row*width*num_channel+col*num_channel+A];
    for(int blurRow = -BLUR_SIZE; blurRow < BLUR_SIZE + 1; ++blurRow) {
      for(int blurCol = -BLUR_SIZE; blurCol < BLUR_SIZE + 1; ++blurCol) {
        int curRow = row + blurRow;
        int curCol = col + blurCol;
        if(curRow > -1 && curRow < height && curCol > -1 && curCol < width) {
          pixVal += in[curRow * width * num_channel + curCol * num_channel + channel];
          pixels++;
        }
      }
    }
    out[row * width * num_channel + col * num_channel + channel] = (unsigned char)(pixVal/pixels);
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

  //blurring
  blur <<<gridSize, blockSize>>>(Input_Image, Output_Image, width, height,n,R,0);
  blur <<<gridSize, blockSize>>>(Input_Image, Output_Image, width, height,n,G,0);
  blur <<<gridSize, blockSize>>>(Input_Image, Output_Image, width, height,n,B,1);
  
  cudaDeviceSynchronize();

  cudaMemcpy(image, Output_Image, sizeof(unsigned char) * height * width * n, cudaMemcpyDeviceToHost);
  cudaFree(Input_Image);
  cudaFree(Output_Image);
  stbi_write_png("blurred.png", width, height, n, image, width * n);

  return 0;
}