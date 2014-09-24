#ifndef FITSFILE
#define FITSFILE

#include <string>

//namespace FITSIO {

#include "fitsio.h"


class FitsFile {

 public:

  FitsFile();

  void openFile(std::string filename);
  void openFile(std::string filename, int mode);

  void createFile(std::string filename);
  void createImage2D(int npixx, int npixy, int bitpix);

  void getSize(int& npixx, int& npixy);

  void readImage(int* buffer, int ntotal);

  void writeImage(int* buffer, int ntotal);
  void writeImage(float* buffer, long firstelem, long ntotal);

  void closeFile(void);

  ~FitsFile();

 private:
  
  fitsfile *fptr;

};

void handle_error(int status);

#endif
