#ifndef BIO_IO_H
#define BIO_IO_H

#include "Memory.h"

//libtiff
#include <tiffio.h>

//libgeotiff
#include "geotiff.h"
#include "xtiffio.h"

double ** 	bufferFile(FILE * fp, int  * numPoints);
double ** 	readFile(char * filePath,int * numPoints, int * xPos, int * yPos);
int 		writeTiff(float *** out, char * filePath, int numBands, int numDims, int outRes, double xTiePoint, double yTiePoint);

#endif
