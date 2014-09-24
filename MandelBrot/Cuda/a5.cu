//Tim McMullen Assignment 5 - 159.735 parallel, Cuda

//nvcc -o a5 a5.cu -I/home/MASSEY/06222757/Desktop/cfitsio -L/home/MASSEY/06222757/Desktop/cfitsio fitsfile.o -l cfitsio -lm

#include <iostream>
#include <cmath>
#include <ctime>
#include <string>
#include <cuda.h>

#include "fitsfile.h"

#define npix 2048
#define WIDTH 4.0
#define max_value 256
#define filename "output.fits"

void write_to_fits(float* h, int nrows, int ncols){

  FitsFile ff;
  ff.createFile(filename);
  ff.createImage2D(ncols, nrows, FLOAT_IMG);

  // Write to image file row by row
  for (int i = 0; i < nrows; ++i) {
    long firstel = i * ncols + 1;
    long ntotal = ncols;
    ff.writeImage(&h[i*ncols], firstel, ntotal);
  }
  ff.closeFile();

}

// Kernel that executes on the CUDA device. This is executed by ONE stream processor
__global__ void calc_pixels(float* image_out, int size, float max_val, float width){

  int i;
  float row, col, ii, jj, x1, y1, pix_size;
  // set the x and y values
  int npixx = size;
  int npixy = size;
  // What element of the array does this thread work on
  i = blockDim.x * blockIdx.x + threadIdx.x;

  if(i > npixy*npixx)return;

  y1 = x1 = -(width/2);
  row = i/npixx;
  col = i%npixx;
  pix_size = (float)width/npixx;
  ii = x1+col*pix_size;
  jj = y1+row*pix_size;

  float count, temp, lensq, x, y;
  lensq=1e12;
  x = 0;
  y = 0;
  count = 0;
  do {
    temp = x * x - y * y + ii;
    y = 2 * x * y + jj;
    x = temp;

    lensq = x * x + y * y;
    ++count;
  } while ( (lensq < width) && (count < max_value) );

  image_out[i] = count;

}

int main(int argc, char* argv[]){

  int image_size, totalSize;
  float *Buffer, *image_out;

  totalSize = npix*npix;

  Buffer = new float[totalSize];
  image_size = totalSize*sizeof(float);
  // Allocate in DEVICE memory
  cudaMalloc(&image_out, image_size);

  // calculate number of blocks per grid required to run sobel operator on image
  int threadsPerBlock = 256;
  int blocksPerGrid = (totalSize + threadsPerBlock - 1) / threadsPerBlock;

  // Invoke kernel
  calc_pixels<<<blocksPerGrid, threadsPerBlock>>>(image_out, npix, max_value, WIDTH);

  // Copy result from device memory into host memory
  cudaMemcpy(Buffer, image_out, image_size, cudaMemcpyDeviceToHost);

  cudaFree(image_out);

  write_to_fits(Buffer, npix, npix);

  free(Buffer);
}
