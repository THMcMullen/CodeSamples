//Tim McMullen, 06222757 Computer Vision - 159.731, Assignment 1
#include "cv.h"
#include "highgui.h"

#define pixel(image,x,y) \
((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels]

int main( int argc, char** argv ) {

  IplImage *image = 0, *image2 = 0;
  char* filename, *output;
  int size, temp, max, min;
  unsigned char sample[256];
//Checks that the input is correct, if not will quit the program
   if (argc == 4) {
     filename=argv[1];
     output=argv[2];
   }
   else exit(0);
   if( (image = cvLoadImage( filename, 1)) == 0 )
     return -1;


  if( (image = cvLoadImage( filename, 1)) == 0 )
  return -1;

  size=atoi(argv[3])/2;
  image2 = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);

  int height = image2->height;
  int width = image2->width;
//Only searchs from left to right then once it hits the end resets the buffer
  for (int pos_y=0;pos_y<height;pos_y++){
    temp = 0;
    for(int i=0;i<256;i++){//sets the values in the array to 0, as to stop any previous values been saved
        sample[i] = 0;
    }
    for (int pos_x=0;pos_x<width;pos_x++){
      for (int x = pos_x; temp <= x + size; temp++){
        for (int y = pos_y - size; y <= pos_y + size; y++){
          //checks to see if a number needs to be added or removed
          if(temp>=0 && temp < width && y>=0 && y < height){
            sample[pixel(image,temp,y)]++; 
          }
          if((y<height && y >=0) && (x<width && x>=0) && ((pos_x-size)-1)>=0){
            sample[pixel(image,((x-size)-1),y)]--;
          }
        }
      }//Finding the minimum value
      for(int i=0;i<255;i++){
        if(sample[i] > 0){
          min = i;
          break;
        }
      }//Finding the max
      for(int j=255;j>0;j--){
        if(sample[j] > 0){
          max = j;
          break;
        }
      }
//Checks the min and max and compaires them to see what value to use
      if(max - pixel(image, pos_x, pos_y)  <=  pixel(image, pos_x,pos_y) - min){
        pixel(image2, pos_x, pos_y) = max;
      } else (pixel(image2, pos_x, pos_y) = min);
    }
  }//saves the new image. 
  cvSaveImage (output, image2);
}


