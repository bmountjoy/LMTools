#ifndef BIO_MODULE_H
#define BIO_MODULE_H

#include <Python/Python.h>

#ifdef __cplusplus
extern "C"{
#endif

//libtiff
#include <tiffio.h>

//libgeotiff - dont need as of right now
#include "geotiff.h"
#include "xtiffio.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <dirent.h>
#include <string.h>

#include <TerraScanBin.h>
#include <Bio_renameBinary.h>
#include "Bio_biometrics.h"
#include "Memory.h"
#include "list.h"

	
// Bio_centroid.c
int centroidBiometrics
(
	int    point,
	double centroidEasting, 
	double centroidNorthing,
	int    radius,
	double minEasting,	//easting of bottom left corner 
	double minNorthing,	//northing of bottom left corner
	int    blockSize,
	int    zThreshold,
	char * chmDir,
	char * outDir
);

// Bio_square.c
int boundingBoxBiometrics
(
	int    point,
	double centroidEasting, 
	double centroidNorthing,
	int    sideLength,
	double minEasting,	//easting of bottom left corner 
	double minNorthing,	//northing of bottom left corner
	int    blockSize,
	int    zThreshold,
	char * chmDir,
	char * outDir
);

// Bio_rename.c
char * 		renameFile(char * filePath, char * outDir, double minEasting, double minNorthing, int blkSize, float zThreshold);

// Bio_LMoments.c
float * 	computeLMoments(float * array, int size);

// Bio_IO.c
double ** 	bufferFile(FILE * fp, int  * numPoints);
double ** 	readFile(char * filePath,int * numPoints, int * xPos, int * yPos);
int 		writeTiff(float *** out, char * filePath, int numBands, int numDims, int outRes, double xTiePoint, double yTiePoint);


#ifdef __cplusplus
}
#endif

// c++ declarations

#endif
