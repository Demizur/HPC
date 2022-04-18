#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>    // include omp header file here


#define RGB_COMPONENT_COLOR 255

struct PPMPixel {
    int red;
    int green;
    int blue;
};

typedef struct{
    int x, y, all;
    PPMPixel * data;
} PPMImage;

void readPPM(const char *filename, PPMImage& img){
    std::ifstream file (filename);
    if (file){
        std::string s;
        int rgb_comp_color;
        file >> s;
        if (s!="P3") {std::cout<< "error in format"<<std::endl; exit(9);}
        file >> img.x >>img.y;
        file >>rgb_comp_color;
        img.all = img.x*img.y;
        std::cout << s << std::endl;
        std::cout << "x=" << img.x << " y=" << img.y << " all=" <<img.all << std::endl;
        img.data = new PPMPixel[img.all];
        for (int i=0; i<img.all; i++){
            file >> img.data[i].red >>img.data[i].green >> img.data[i].blue;
        }

    }else{
        std::cout << "the file:" << filename << "was not found" << std::endl;
    }
    file.close();
}

void writePPM(const char *filename, PPMImage & img){
    std::ofstream file (filename, std::ofstream::out);
    file << "P3"<<std::endl;
    file << img.x << " " << img.y << " "<< std::endl;
    file << RGB_COMPONENT_COLOR << std::endl;

    for(int i=0; i<img.all; i++){
        file << img.data[i].red << " " << img.data[i].green << " " << img.data[i].blue << (((i+1)%img.x ==0)? "\n" : " ");
    }
    file.close();
}

//
//write the function for shifting
//

void shiftPPM(PPMImage &image)
{
    PPMImage new_image;
	new_image.data = new PPMPixel[image.all];
	 
	int i, j, delta = 1;

    #pragma omp parallel for collapse(2)
	for (i = 0; i < image.x-1; i++) 
    {
		for (j = 0; j < image.y; j++) 
    {
			new_image.data[i+delta+j*image.x].green = image.data[i+j*image.x].green;
			new_image.data[i+delta+j*image.x].red = image.data[i+j*image.x].red;
			new_image.data[i+delta+j*image.x].blue = image.data[i+j*image.x].blue;
		}
	}

    #pragma omp parallel for 
    for (i = 0; i < image.y; i++) 
    {
		new_image.data[i*image.x].green = image.data[image.x-1+i*image.x].green;
		new_image.data[i*image.x].red = image.data[image.x - 1+i*image.x].red;
		new_image.data[i*image.x].blue = image.data[image.x -1+i*image.x].blue;
	}

    #pragma omp parallel for 
    for (i = 0; i<image.all; i++)
    {
		image.data[i].green = new_image.data[i].green;
		image.data[i].red = new_image.data[i].red;
		image.data[i].blue = new_image.data[i].blue;
	}
 
}

int main(int argc, char *argv[]){
    PPMImage image;
    readPPM("car.ppm", image);
    
    //
    //write code here
    //
    double start = omp_get_wtime();
    shiftPPM(image);
    double end = omp_get_wtime();
    writePPM("new_car.ppm", image);
    std::cout << "Time elapsed(seconds): " << end - start << std::endl;
    return 0;
}
