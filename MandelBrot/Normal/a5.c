//Tim McMullen Assignment 5 - 159.735 parallel, MPI

//to compile --------    mpecc -mpilog a5.c -o a5 -lcfitsio -lm
// mpirun -np 4 a5

#include <stdlib.h>
#include <stdio.h>

#include "mpi.h"

// You must include this
#include "fitsio.h"

#define npix 2048
#define filename "!output.fits"

typedef struct {
  float re, im;
} complex_t;

typedef struct {
  float* pixdata;
  int npixx, npixy;
  float x1, y1, pix_size;
} image_t;


void create_img(image_t* img, int npixx, int npixy) {
  int ntotal = npixx * npixy;
  //printf("%d\n", ntotal);
  img -> pixdata = malloc(ntotal * sizeof(float));
  img -> npixx = npixx;
  img -> npixy = npixy;
}

void set_pix(image_t* img, int ix, int iy, float value) {
  int idx = iy * img -> npixx + ix;
  img -> pixdata[idx] = value;
}

float get_pix(image_t* img, int ix, int iy) {
  int idx = iy * img -> npixx + ix;
  return img -> pixdata[idx];
}

void delete_img(image_t* img) {
  free(img -> pixdata);
}

float calc_pixel(float x, float y) {
  
  float max_value = 256;
  float count;
  complex_t z;
  float temp;
  float lensq=1e12;
  
  z.re = 0;
  z.im = 0;
  
  count = 0;

  do {
    temp = z.re * z.re - z.im * z.im + x;
    z.im = 2 * z.re * z.im + y;
    z.re = temp;

    lensq = z.re * z.re + z.im * z.im;
    ++count;
  } while ( (lensq < 4.0) && (count < max_value) );
  
 // printf("#### %f %f %f\n", x, y, count);
  return count;

}

void handle_error(int status, char* my_msg) {
  printf("Problem with %s\n", my_msg);
  fits_report_error(stdout, status);
  exit(status);
}

void write_to_fits(image_t* img)
{
  fitsfile* fptr;
  int status=0;
  long naxes[2] = {img->npixx, img->npixy};
  //long npix = naxes[0] * naxes[1];
  long firstelem = 1;
  int i;

  if (fits_create_file(&fptr, filename, &status))
    handle_error(status, "create_file");
  
  if (fits_create_img(fptr, FLOAT_IMG, 2, naxes, &status))
    handle_error(status, "create_img");

  if (fits_write_img(fptr, TFLOAT, firstelem, npix*npix, img->pixdata, &status))
    handle_error(status, "write_img");
  
  if (fits_close_file(fptr, &status))
    handle_error(status, "close_file");
}


int main(int argc,char *argv[]){

  int numproc, myid;

  // Signal from master to slave to request termination
  const int TERMINATION = -1;

  MPI_Status Stat;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numproc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

if (myid == 0) {
//Set up the image data---------------------------------------------------
double startTime = MPI_Wtime();
  // Resolution (space and intensity) of the Mandelbrot image
  int npixx = npix;
  int npixy = npix;

  // Boundaries of the image in cartesian coordinates. This will
  // define a space from (-2, -2) to (2, 2)
  float x1 = -2;
  float y1 = -2;
  float width = 4;
  float pix_size = width / npix;


  image_t* img = malloc(sizeof(image_t));
  create_img(img, npixx, npixy);
  img -> x1 = x1;
  img -> y1 = y1;
  img -> pix_size = pix_size;

//-------------------------------------------------------------------------
    npixy--;
    float *image_buffer;
    int x;
    int i;
    // The id of the slave that is requesting a new job
    int *Preq;
    Preq = malloc(sizeof(int)*2);
    image_buffer = malloc(npix* sizeof(float));

    for(i=1;i<numproc;i++){

      MPI_Send(&npixy, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      npixy--;

    }
    while(npixy >= 0) {
      // Wait for a request from a slave
      MPI_Recv(Preq, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &Stat);
      //printf("preq is %d and %d\n",Preq[0], Preq[1]);
      //Wait for the data to be sent fromt he slave
      MPI_Recv(image_buffer, npix, MPI_FLOAT, Preq[0], 0, MPI_COMM_WORLD, &Stat);
      // Send task to the slave
      MPI_Send(&npixy, 1, MPI_INT, Preq[0], 0, MPI_COMM_WORLD);
      //printf("sending line %d to %d\n",npixy, Preq[0]);
      npixy--;


      //Now add data to image
      for(x =0; x<npix;x++){
        set_pix(img,x,Preq[1],image_buffer[x]);
      }
    }

    write_to_fits(img);
    delete_img(img);

double endTime = MPI_Wtime();
printf("time taken is :%f\n",endTime-startTime);
  }
  else{

      int taskno;
      int i, x,terminate = 1;
      float *temp;
      float x1=-2,y1=-2, ii, jj;
      int *line_info;
      float width = 4;
      float pix_size = width / npix;
      temp = malloc(npix * sizeof(float));
      line_info = malloc(sizeof(int)*2);
      line_info[0] = myid;

      MPI_Recv(&taskno, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &Stat);
      line_info[1] = taskno;

      for(x = 0; x<npix;x++){
	ii = x1 + x*pix_size;
        jj = y1 + taskno * pix_size;
        temp[x] = calc_pixel(ii,jj);
      }

      while (terminate == 1) {
 
	// Request a task
	MPI_Send(line_info, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        //Send work on data
        MPI_Send(temp, npix, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
	// Get task assigned from master
	MPI_Recv(&taskno, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &Stat);
        line_info[1] = taskno;
  	// If given task number is the termination signal, set the flag
	// to jump out of the loop
	if (taskno < (numproc-1)) {
	  for(x = 0; x<npix;x++){
	    ii = x1 + x * pix_size;
	    jj = y1 + taskno * pix_size;
	    temp[x] = calc_pixel(ii,jj);
	  }
          printf("ending on node %d, on line %d \n",myid, taskno);
	  terminate = 0;
	}
	// Otherwise, compute the required task
	else {
	  //temp = compute(taskno);
	  for(x = 0; x<npix;x++){
	    ii = x1 + x*pix_size;
	    jj = y1 + taskno * pix_size;
	    temp[x] = calc_pixel(ii,jj);
	  }
	}
      }

  }

  MPI_Finalize();
}


