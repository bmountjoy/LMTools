
#ifndef TCD_MODULE_H
#define TCD_MODULE_H

//shape file library
#include <shapefil.h>

//libtiff
#include <tiffio.h>

//libgeotiff
#include "geotiff.h"
#include "xtiffio.h"

//GTIFDefn
#include <geo_normalize.h>

#include "moore_neighbour_tracing.h"
#include "memory.h"
#include "treetop_metrics.h"

//
//boxMuller.c
float box_muller(float mean, float stddev);

//
//tiffio.c
void getModelPixelScaleTage(TIFF *tif, double ** mpsData, int16 * mpsCount);
void getModelTiePointTag(TIFF *tif, double ** mttData, int16 * mttCount);
float ** readTIFF(TIFF *);
double getUlEasting( double *mpsData, int mpsCount, double *mttData, int mttCount);
double getUlNorthing(double *mpsData, int mpsCount, double *mttData, int mttCount);
int extractGeoTiffTags(TIFF * tif, GTIFDefn * gtifDef, double ** mpsData, int16 *mpsCount, 
	double ** mttData, int16 *mttCount, double *ulN, double *ulE);
int src_writeTIFF(float ** outBuffer, char * filePath, int imageLength, int imageWidth, 
	GTIFDefn gtifDef, double *mpsData, int16 mpsCount, double *mttData, int16 mttCount);

//
//TreeTops.c
int findTreeTops(float **orig, double * mpsData, int imageLength, int imageWidth, double minEasting, double minNorthing, 
	int run_range3, float range3_min, float range3_max, 
	int run_range5, float range5_min, float range5_max,
	int run_range7, float range7_min, float range7_max,
	int run_range9, float range9_min, float range9_max,
	int run_range11, float range11_min, float range11_max,
	SHPHandle hshp, DBFHandle hdbf, int smooth_type, int add_noise);

int findInPartition(float **image, double * mpsData, int imageLength, int imageWidth, double minEasting, double minNorthing,
	int run_range3, float range3_min, float range3_max, 
	int run_range5, float range5_min, float range5_max,
	int run_range7, float range7_min, float range7_max,
	SHPHandle hshp, DBFHandle hdbf, int add_noise);

//
// partitionTiff.c
int partitionTiff(char * tiffPath, char * outDir);

//
// TreeCrownDelineation
int tcd (SHPHandle HSHP_treetops, DBFHandle HDBF_treetops, const char * tc_out_path, 
	float ** inTif, int length, int width, 
	double ulEasting, double ulNorthing, 
	int run_h1, float h1_min, float h1_max,
	int run_h2, float h2_min, float h2_max,
	int run_h3, float h3_min, float h3_max, 
	float perc_1, float perc_2, float perc_3, 
	int rad_1, int rad_2, int rad_3,
	double * mpsData, int smooth_type, int shape_crown);


//
// treetop_metrics

#endif
