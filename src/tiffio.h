#ifndef TIFFIO_H
#define TIFFIO_H

//libtiff
#include <tiffio.h>

//libgeotiff
#include <geotiff.h>
#include <xtiffio.h>
//GTIFDefn
#include <geo_normalize.h>

void getModelPixelScaleTage(TIFF *tif, double ** mpsData, short * mpsCount);

void getModelTiePointTag(TIFF *tif, double ** mttData, short * mttCount);

float ** readTIFF(TIFF *);

double getUlEasting( double *mpsData, int mpsCount, double *mttData, int mttCount);

double getUlNorthing(double *mpsData, int mpsCount, double *mttData, int mttCount);

int extractGeoTiffTags(TIFF * tif, GTIFDefn * gtifDef, double ** mpsData, int16 *mpsCount, 
	double ** mttData, int16 *mttCount, double *ulN, double *ulE);

int src_writeTIFF(float ** outBuffer, char * filePath, int imageLength, int imageWidth, 
	GTIFDefn gtifDef, double *mpsData, int16 mpsCount, double *mttData, int16 mttCount);

#endif
