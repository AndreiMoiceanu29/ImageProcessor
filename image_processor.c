/*
LodePNG Examples

Copyright (c) 2005-2012 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#include "lodepng.h"
#include "time.h"
#include <stdio.h>
#include <stdlib.h>
//extern void to_gray_asm(unsigned char* , int, int);
extern void horizontal_flip_asm(unsigned char* , int, int);
extern void vertical_flip_asm(unsigned char* , int, int);
/*
3 ways to encode a PNG from RGBA pixel data to a file (and 2 in-memory ways).
NOTE: these samples overwrite the file or test.png without warning!
*/
unsigned char* decodeOneStep(const char* filename, unsigned* w, unsigned* h) {
  unsigned error;
  unsigned char* image = 0;
  unsigned width, height;

  error = lodepng_decode32_file(&image, &width, &height, filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
  printf("width: %u, height: %u\n", width, height);
  /*use image here*/
  *w = width;
  *h = height;
  return image;
}
/*
Example 1
Encode from raw pixels to disk with a single function call
The image argument has width * height RGBA pixels or width * height * 4 bytes
*/
void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
  /*Encode the image*/
  unsigned error = lodepng_encode32_file(filename, image, width, height);

  /*if there's an error, display it*/
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

/*
Example 2
Encode from raw pixels to an in-memory PNG file first, then write it to disk
The image argument has width * height RGBA pixels or width * height * 4 bytes
*/
void encodeTwoSteps(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
  unsigned char* png;
  size_t pngsize;

  unsigned error = lodepng_encode32(&png, &pngsize, image, width, height);
  if(!error) lodepng_save_file(png, pngsize, filename);

  /*if there's an error, display it*/
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  free(png);
}

/*
Example 3
Save a PNG file to disk using a State, normally needed for more advanced usage.
The image argument has width * height RGBA pixels or width * height * 4 bytes
*/
void encodeWithState(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
  unsigned error;
  unsigned char* png;
  size_t pngsize;
  LodePNGState state;

  lodepng_state_init(&state);
  /*optionally customize the state*/

  error = lodepng_encode(&png, &pngsize, image, width, height, &state);
  if(!error) lodepng_save_file(png, pngsize, filename);

  /*if there's an error, display it*/
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  lodepng_state_cleanup(&state);
  free(png);
}
void to_gray(unsigned char * image, int width, int height)
{
      for (int y = 0; y < height; y++)
      {
            for (int x = 0; x < width; x++)
            {
                unsigned r = image[4 * width * y + 4 * x + 0];
                unsigned g = image[4 * width * y + 4 * x + 1];
                unsigned b = image[4 * width * y + 4 * x + 2];
                unsigned gray = (r + g + b) / 3;
                image[4 * width * y + 4 * x + 0] =
                image[4 * width * y + 4 * x + 1] =
                image[4 * width * y + 4 * x + 2] = gray;
            } 
      }
}
void copy(unsigned char *src, unsigned char *dst, int width, int height){
  // Copy the image from src to dst
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      dst[4 * width * y + 4 * x + 0] = src[4 * width * y + 4 * x + 0];
      dst[4 * width * y + 4 * x + 1] = src[4 * width * y + 4 * x + 1];
      dst[4 * width * y + 4 * x + 2] = src[4 * width * y + 4 * x + 2];
    }
  }
}
float computeFloatingPointPixelValue(unsigned char pixel){
  float f = pixel / 255.0;


  return f;
}

unsigned char computeIntegerPixelValue(float  pixel){
  float i = pixel * 255;
  //i = i % 256;
  if(i < 0){
    i = 0;
  }
  if(i > 255){
    i = 255;
  }
  return (unsigned char)i;
}

void convolution(unsigned char *image, int width, int height, float **kernel, int kernel_size)
{
    unsigned char* image_copy = (unsigned char*)malloc(width * height * 4 * sizeof(unsigned char));
    copy(image, image_copy, width, height);
      int kernel_half = kernel_size / 2;
      for (int y = 0; y < height; y++)
      {
            for (int x = 0; x < width; x++)
            {
                float r = 0.0f, g = 0.0f, b = 0.0f;
                for (int i = -kernel_half; i <= kernel_half; i++)
                {
                    for (int j = -kernel_half; j <= kernel_half; j++)
                    {
                        int x_ = x  - i;
                        int y_ = y  - j;
                        if (x_ < 0 || x_ >= width || y_ < 0 || y_ >= height)
                            continue;
                        //printf("%d %d\n", x_, y_);
                        r += computeFloatingPointPixelValue(image_copy[4 * width * y_ + 4 * x_ + 0]) * kernel[i + kernel_half][j + kernel_half];
                        g += computeFloatingPointPixelValue(image_copy[4 * width * y_ + 4 * x_ + 1]) * kernel[i + kernel_half][j + kernel_half];
                        b += computeFloatingPointPixelValue(image_copy[4 * width * y_ + 4 * x_ + 2]) * kernel[i + kernel_half][j + kernel_half];
                    }
                }
                image[4 * width * y + 4 * x + 0] = computeIntegerPixelValue(r);
                image[4 * width * y + 4 * x + 1] = computeIntegerPixelValue(g);
                image[4 * width * y + 4 * x + 2] = computeIntegerPixelValue(b);
            }
      }

}

void horizontalFlip(unsigned char *image, int width, int height)
{
      for (int y = 0; y < height; y++)
      {
            for (int x = 0; x < width / 2; x++)
            {
                unsigned char temp = image[4 * width * y + 4 * x + 0];
                image[4 * width * y + 4 * x + 0] = image[4 * width * y + 4 * (width - x - 1) + 0];
                image[4 * width * y + 4 * (width - x - 1) + 0] = temp;

                temp = image[4 * width * y + 4 * x + 1];
                image[4 * width * y + 4 * x + 1] = image[4 * width * y + 4 * (width - x - 1) + 1];
                image[4 * width * y + 4 * (width - x - 1) + 1] = temp;

                temp = image[4 * width * y + 4 * x + 2];
                image[4 * width * y + 4 * x + 2] = image[4 * width * y + 4 * (width - x - 1) + 2];
                image[4 * width * y + 4 * (width - x - 1) + 2] = temp;
            }
      }
}

void verticalFlip(unsigned char *image, int width, int height)
{
      for (int y = 0; y < height / 2; y++)
      {
            for (int x = 0; x < width; x++)
            {
                unsigned char temp = image[4 * width * y + 4 * x + 0];
                image[4 * width * y + 4 * x + 0] = image[4 * width * (height - y - 1) + 4 * x + 0];
                image[4 * width * (height - y - 1) + 4 * x + 0] = temp;

                temp = image[4 * width * y + 4 * x + 1];
                image[4 * width * y + 4 * x + 1] = image[4 * width * (height - y - 1) + 4 * x + 1];
                image[4 * width * (height - y - 1) + 4 * x + 1] = temp;

                temp = image[4 * width * y + 4 * x + 2];
                image[4 * width * y + 4 * x + 2] = image[4 * width * (height - y - 1) + 4 * x + 2];
                image[4 * width * (height - y - 1) + 4 * x + 2] = temp;
            }
      }
}



void add(unsigned char *src, unsigned char *dst, int width, int height){
  // Add the image from src to dst
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      //dst[4 * width * y + 4 * x + 0] += src[4 * width * y + 4 * x + 0];
      //dst[4 * width * y + 4 * x + 1] += src[4 * width * y + 4 * x + 1];
      dst[4 * width * y + 4 * x + 2] += src[4 * width * y + 4 * x + 2];
      //printf("dst=%d\n", dst[4 * width * y + 4 * x + 2]);
    }
  }
}

void edgeDetection(unsigned char *src, unsigned char* dst, int width, int height){
  to_gray(src,width,height);

    float **sobelX = (float **)malloc(3 * sizeof(float *));
  for (int i = 0; i < 3; i++)
  {
    sobelX[i] = (float *)malloc(3 * sizeof(float));
  }
  sobelX[0][0] = -1.0f;
  sobelX[0][1] = 0.0f;
  sobelX[0][2] = 1.0f;
  sobelX[1][0] = -2.0f;
  sobelX[1][1] = 0.0f;
  sobelX[1][2] = 2.0f;
  sobelX[2][0] = -1.0f;
  sobelX[2][1] = 0.0f;
  sobelX[2][2] = 1.0f;

  //int sobelY[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };
  float **sobelY = (float **)malloc(3 * sizeof(float *));
  for (int i = 0; i < 3; i++)
  {
    sobelY[i] = (float *)malloc(3 * sizeof(float));
  }
  sobelY[0][0] = -1.0f;
  sobelY[0][1] = -2.0f;
  sobelY[0][2] = -1.0f;
  sobelY[1][0] = 0.0f;
  sobelY[1][1] = 0.0f;
  sobelY[1][2] = 0.0f;
  sobelY[2][0] = 1.0f;
  sobelY[2][1] = 2.0f;
  sobelY[2][2] = 1.0f;
  unsigned char* image_copy = (unsigned char*)malloc(width * height * 4 * sizeof(unsigned char));
  copy(src, image_copy, width, height);
  convolution(src,width,height,sobelX,3);
  convolution(image_copy,width,height,sobelY,3);
  add(src, image_copy, width, height);
  copy(image_copy, dst, width, height);
}

void boxBlur(unsigned char *src, unsigned char* dst, int width, int height){
  to_gray(src,width,height);
  float **box_kernel = (float **)malloc(3 * sizeof(float *));
  for (int i = 0; i < 3; i++)
  {
    box_kernel[i] = (float *)malloc(3 * sizeof(float));
  }
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      box_kernel[i][j] = 1.0f / 9.0f;
    }
  }
  convolution(src,width,height,box_kernel,3);
  copy(src, dst, width, height);
  free(box_kernel);
}

void sharpen(unsigned char *src, unsigned char* dst, int width, int height){
  to_gray(src,width,height);
  float **sharpen_kernel = (float **)malloc(3 * sizeof(float *));
  for (int i = 0; i < 3; i++)
  {
    sharpen_kernel[i] = (float *)malloc(3 * sizeof(float));
  }
  sharpen_kernel[0][0] = 0.0f;
  sharpen_kernel[0][1] = -1.0f;
  sharpen_kernel[0][2] = 0.0f;
  sharpen_kernel[1][0] = -1.0f;
  sharpen_kernel[1][1] = 5.0f;
  sharpen_kernel[1][2] = -1.0f;
  sharpen_kernel[2][0] = 0.0f;
  sharpen_kernel[2][1] = -1.0f;
  sharpen_kernel[2][2] = 0.0f;

  convolution(src,width,height,sharpen_kernel,3);
  copy(src, dst, width, height);
  free(sharpen_kernel);
}

int main(int argc, char *argv[]) {
  const char* filename = argc > 1 ? argv[1] : "test.png";
  const char* outputFile = argc > 2 ? argv[2] : "output.png";
  /*read the image*/
  unsigned char* image;
  unsigned width, height;
  printf("Reading %s\n", filename);
  image = decodeOneStep(filename, &width, &height);
  printf("Image loaded !");
  /*run an example*/
  //int sobelX[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
  unsigned char* image_copy = (unsigned char*)malloc(width * height * 4 * sizeof(unsigned char));
  //edgeDetection(image,image_copy,width,height);
  int option;
  printf("****** Image Processing ******\n");
 // printf("1. Edge Detection\n");
  printf("1. Horizontal Flip\n");
  printf("2. Box Blur\n");
  printf("3. Sharpen\n");
  printf("4 Gray Scale\n");
  printf("5. Vertical Flip\n");
  printf("6. Edge detection\n");
  scanf("%d", &option);
  switch(option){
    case 1:{
      clock_t start = clock();
      //copy(image, image_copy, width, height);
      //horizontalFlip(image,width,height);
      horizontal_flip_asm(image,width,height);
      clock_t end = clock();
      double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
      printf("Time spent: %f\n s", time_spent);
      break;
    }
    case 2:{
      clock_t start2 = clock();
      boxBlur(image,image_copy,width,height);
      clock_t end2 = clock();
      double time_spent2 = (double)(end2 - start2) / CLOCKS_PER_SEC;
      printf("Time spent: %f\n s", time_spent2);
      break;
    }
    case 3:{
      clock_t start3 = clock();
      sharpen(image,image_copy,width,height);
      clock_t end3 = clock();
      double time_spent3 = (double)(end3 - start3) / CLOCKS_PER_SEC;
      printf("Time spent: %f\n s", time_spent3);
      break;
    }
    case 4:{
      clock_t start4 = clock();
      to_gray(image,width,height);
      copy(image, image_copy, width, height);
      clock_t end4 = clock();
      double time_spent4 = (double)(end4 - start4) / CLOCKS_PER_SEC;
      printf("Time spent: %f\n s", time_spent4);
      break;
    }
    case 5:{
      clock_t start5 = clock();
      copy(image, image_copy, width, height);
      verticalFlip(image,width,height);
      clock_t end5 = clock();
      double time_spent = (double)(end5 - start5) / CLOCKS_PER_SEC;
      printf("Time spent: %f\n s", time_spent);
      break;
    }
    case 6:{
      clock_t start6 = clock();
      edgeDetection(image,image_copy,width,height);
      copy(image_copy,image,width,height);
      clock_t end6 = clock();
      double time_spent6 = (double)(end6 - start6) / CLOCKS_PER_SEC;
      printf("Time spent: %f\n s", time_spent6);
      break;
    }
    default:
      printf("Invalid option\n");
      break;
  }
  encodeOneStep(outputFile, image, width, height);

  free(image);
  free(image_copy);
  return 0;
}