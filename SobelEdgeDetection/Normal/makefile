CPP = g++
CFLAGS = -w -O3
MPICPP = mpic++

# Locally compiled modules
OBJS = fitsfile.o

# Link to these libraries
LIBS = -lcfitsio -lm
INCP = -I/home/MASSEY/06222757/Desktop/cfitsio
LIBP = -L/home/MASSEY/06222757/Desktop/cfitsio


MODS = $(INCP) $(LIBP) $(OBJS) $(LIBS)

a4 : a4.cpp $(OBJS)
	${CPP} $(CFLAGS) -o a4 a4.cpp $(MODS)

# Modules compiled and linked separately
fitsfile.o : fitsfile.cpp fitsfile.h
	${CPP} $(CFLAGS) $(INCP) -c fitsfile.cpp

