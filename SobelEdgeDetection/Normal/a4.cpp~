#include <iostream>
#include <cmath>
#include <ctime>
#include <string>

#include "fitsfile.h"

void write_to_fits(float* h, int nrows, int ncols, std::string filename)
{
  std::cout << "## Writing to " << filename << std::endl;

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

int main(int argc, char* argv[]){

  if(argc!=3){
    printf("The program requires the user to input the image required and output name\n");
    return -1;
  }
  char *fname, *Oname;
  int imagewidth, imageheight;
  fname = argv[1];
  Oname = argv[2];



  FitsFile ff;

  ff.openFile(fname);

  ff.getSize(imagewidth,imageheight);
  float *h = new float[imagewidth*imageheight];
  float *r = new float[imagewidth*imageheight];
  //The master matrix
  //Creates the kernel
       int matrix[9][9];
       matrix[0][0] = 4;
       matrix[1][0] = 5;
       matrix[2][0] = 6;
       matrix[3][0] = 7;
       matrix[4][0] = 8;
       matrix[5][0] = 7;
       matrix[6][0] = 6;
       matrix[7][0] = 5;
       matrix[8][0] = 4;
       matrix[0][1] = 3;
       matrix[1][1] = 4;
       matrix[2][1] = 5;
       matrix[3][1] = 6;
       matrix[4][1] = 7;
       matrix[5][1] = 6;
       matrix[6][1] = 5;
       matrix[7][1] = 4;
       matrix[8][1] = 3;
       matrix[0][2] = 2;
       matrix[1][2] = 3;
       matrix[2][2] = 4;
       matrix[3][2] = 5;
       matrix[4][2] = 6;
       matrix[5][2] = 5;
       matrix[6][2] = 4;
       matrix[7][2] = 3;
       matrix[8][2] = 2;
       matrix[0][3] = 1;
       matrix[1][3] = 2;
       matrix[2][3] = 3;
       matrix[3][3] = 4;
       matrix[4][3] = 5;
       matrix[5][3] = 4;
       matrix[6][3] = 3;
       matrix[7][3] = 2;
       matrix[8][3] = 1;
       matrix[0][4] = 0;
       matrix[1][4] = 0;
       matrix[2][4] = 0;
       matrix[3][4] = 0;
       matrix[4][4] = 0;
       matrix[5][4] = 0;
       matrix[6][4] = 0;
       matrix[7][4] = 0;
       matrix[8][4] = 0;
       matrix[0][5] = -1;
       matrix[1][5] = -2;
       matrix[2][5] = -3;
       matrix[3][5] = -4;
       matrix[4][5] = -5;
       matrix[5][5] = -4;
       matrix[6][5] = -3;
       matrix[7][5] = -2;
       matrix[8][5] = -1;
       matrix[0][6] = -2;
       matrix[1][6] = -3;
       matrix[2][6] = -4;
       matrix[3][6] = -5;
       matrix[4][6] = -6;
       matrix[5][6] = -5;
       matrix[6][6] = -4;
       matrix[7][6] = -3;
       matrix[8][6] = -2;
       matrix[0][7] = -3;
       matrix[1][7] = -4;
       matrix[2][7] = -5;
       matrix[3][7] = -6;
       matrix[4][7] = -7;
       matrix[5][7] = -6;
       matrix[6][7] = -5;
       matrix[7][7] = -4;
       matrix[8][7] = -3;
       matrix[0][8] = -4;
       matrix[1][8] = -5;
       matrix[2][8] = -6;
       matrix[3][8] = -7;
       matrix[4][8] = -8;
       matrix[5][8] = -7;
       matrix[6][8] = -6;
       matrix[7][8] = -5;
       matrix[8][8] = -4;

 /* int count = 0;
  for(int i =0;i<imagewidth;i++){
    for (int j=0; j<imageheight;j++){
      printf("%d\n",count);
      count++;
    }
  }*/
  int istart, iend, jstart, jend, yystart, xxstart;
  int sum1=0, sum2=0, sumf=0;

  //printf("imagewidth = %d\n",imagewidth);
  //printf("imageheight = %d\n",imageheight);
  ff.readImage(r, imagewidth*imageheight);

 //int count = 0;
  for(int i =0;i<imagewidth;i++){
    for (int j=0; j<imageheight;j++){
      //h[(j*imagewidth) + i] = r[(j*imagewidth) + i];
      sum1 = 0;
      sum2 = 0;

      istart = i - 4;
      istart = istart<0?0:istart;
      xxstart = istart<0?-istart:0;
      iend = i + 4;
      iend = iend>=imagewidth?imagewidth-1:iend;

      jstart = j - 4;
      jstart = jstart<0?0:jstart;
      yystart = jstart<0?-jstart:0;
      jend = j + 4;
      jend = jend>=imageheight?imageheight-1:jend;
      // if(i == 5 && j == 5)printf("istart = %d, iend = %d, jstart = %d, jend = %d\n", istart,iend,jstart,jend);
        for(int y=jstart, yy=yystart;y<=jend;y++,yy++){
            for(int x=istart, xx=xxstart;x<=iend;x++,xx++){
              sum1 += r[(y*imagewidth)+x] * matrix[xx][yy];
              sum2 += r[(y*imagewidth)+x] * matrix[yy][xx];
          }
      }
      //printf("%d\n",count);
      //count++;
    sumf = abs(sum1) + abs(sum2);
    h[(j*imagewidth) + i] = sumf;
    }
  }
  write_to_fits(h,imageheight, imagewidth, Oname);
}