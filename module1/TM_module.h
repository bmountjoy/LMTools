#ifndef TM_MODULE_H
#define TM_MODULE_H

#include <Python/Python.h>

//libtiff
#include <tiffio.h>

//libgeotiff
#include "geotiff.h"
#include "xtiffio.h"

/**
 **************************** VARIABLES ****************************************
**/

/**
 **************************** FUNCTIONS ****************************************
**/

// TM_module.c


//TM_biometrics.c
void			calcBiometrics(float ** points, int numPoints, float minEasting, float, int, float, float, int, int);

//TM_LMoments.c
float *			computeLMoments(float * array, int size);

//TM_Math.c
float 			calculateMean1D(float * array, int size);
float 			scalculateVariance1D(float * array, int size);
float 			calculateRugosity1D(float * array, int size);
float			calculateMean(float ** buf, int numRows, int numCols);
float			calculateStandardDeviation(float ** buf, int numRows, int numCols);
float * 		calculateRange(float ** buf, int numRows, int numCols);
int 			floatCompare(const void * a, const void * b);
void 			medianFilter(float ** buf, float ** out, int width, int numRows, int numCols);

//TM_TiffIO.c
double			getUlEasting(void);
double 			getUlNorthing(void);
double 			getScaleX(void);
double 			getScaleY(void);
void 			setMttData(double i, double j, double k, double x, double y, double z);
int 			readTIFF(char * filePath);
int 			scanlineIO(TIFF * tif);
int 			tileIO(TIFF * tif);
int 			writeTIFF(float ** outBuffer, char * filePath, int imageLength, int imageWidth, int xFactor, int yFactor);

// glcm.c
int 			getNumMeasures(void);
char **			getMeasureNames(void);
int 			calculateGLCM(float ** buffer, float *** out, int numLevels, int offsets [][2], int numOffsets, int windowSize);

// TM_D8.c
void 			D8(float ** evelMap, float ** outBuf);

// TM_flow_D_infinity.c 
void 			Dinf(float ** elevMap, float ** outBuf);

// TM_Form.c
void 			slopeGradient(float ** elev, float ** out);
void 			slopeAspect(float ** elev, float ** out, int linearize, int offset);
void 			downSlopeCurvature(float ** elev, float ** out);
void 			acrossSlopeCurvature(float ** elev, float ** out);

// TM_Wetness.c
void 			wetnessIndex(float ** upslopeArea, float ** slopeGradient, float ** out);

// TM_memory.c
float ** initBuffer2D(int numRows, int numCols, float initVal);
float *** initBuffer3D(int numLevels, int numRows, int numCols, float initVal);
void freeBuffer2D(float ** buffer, int numRows);
void freeBuffer3D(float *** buffer, int numLevels, int numRows);

#endif
