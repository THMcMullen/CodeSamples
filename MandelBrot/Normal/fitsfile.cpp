#include <iostream>

#include "fitsfile.h"

void handle_error(int status)
{
  std::cout << "FITSIO ERROR! Status = " << status << std::endl;
  exit(99);
}

FitsFile::FitsFile(void)
{
}

void FitsFile::openFile(std::string filename)
{
  openFile(filename, READONLY);
}

void FitsFile::openFile(std::string filename, int mode)
{
  int status = 0;
  if (fits_open_file(&fptr, filename.c_str(), mode, &status))
    handle_error(status);
}

void FitsFile::createFile(std::string filename)
{
  int status = 0;
  remove(filename.c_str());
  if (fits_create_file(&fptr, filename.c_str(), &status))
    handle_error(status);
}

void FitsFile::createImage2D(int npixx, int npixy, int bitpix)
{
  int status = 0;
  long naxis = 2;
  long naxes[2] = {npixx, npixy};
  if (fits_create_img(fptr, bitpix, naxis, naxes, &status))
    handle_error(status);
}

void FitsFile::getSize(int& npixx, int& npixy)
{
  int status = 0;
  int nfound;
  long naxes[2];
  if (fits_read_keys_lng(fptr, "NAXIS", 1, 2, naxes, &nfound, &status))
    handle_error(status);
  npixx = naxes[0];
  npixy = naxes[1];
}

void FitsFile::writeImage(float* buffer, long firstelem,long ntotal)
{
  int status = 0;
  if (fits_write_img(fptr, TFLOAT, firstelem, ntotal, buffer, &status))
    handle_error(status);
}

void FitsFile::closeFile(void)
{
  int status = 0;
  if (fits_close_file(fptr, &status))
    handle_error(status);
}

FitsFile::~FitsFile()
{
}

