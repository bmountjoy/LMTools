
/**
 * To compile : export MACOSX_DEPLOYMENT_TARGET=10.6
 *            : python setup.py install
 *       then : python TM_Inerface.py
 * To build   : python setup_app.py py2app
 **/
 
 
/**
 * INCLUDES
 */
#include "TM_module.h" 
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <geo_normalize.h> //GTIFDefn -- hack

#define ModelPixelScaleTag	33550	//geotiff tag id
#define ModelTiepointTag	33922	//geotiff tag id

/**
 * PROTOTYPES
 */
static PyObject* applyWindowFilter(PyObject* self, PyObject* args);
static PyObject* applyFileFilter(PyObject* self, PyObject* args);
static PyObject* calculateMedian(PyObject* self, PyObject* args);
static PyObject* relief(PyObject* self, PyObject* args);
static PyObject* calculateFlow(PyObject* self, PyObject* args);
static PyObject* calculateFlowDInfinity(PyObject* self, PyObject* args);
static PyObject* calculateSlopeGradient(PyObject*, PyObject*);
static PyObject* calculateWetnessIndex(PyObject* self, PyObject* args);
static PyObject* calculateSlopeAspect(PyObject*, PyObject*);
static PyObject* calculateDownSlopeCurvature(PyObject*, PyObject*);
static PyObject* calculateAcrossSlopeCurvature(PyObject*, PyObject*);
static PyObject* Py_setup(PyObject*, PyObject*);
static PyObject* teardown(PyObject*, PyObject*);


char * 		buildOutPath(char [], char * fileName);
int 		setup(char * filePath, char * outDirectory);
int 		processGlcm(float ** raster, char * dirName, int numLevels, int );

/**
 * GLOBAL CONSTANTS
 */
#define 	SUCCESS 	1


/**
 * GLOBAL VARIABLES
 */
int MIN_THRESH;		// = -1391;
int MAX_THRESH;		// = 29035;
int GRID;			// = 3;
int IMAGE_WIDTH;
int IMAGE_LENGTH;
int NUMBER_OF_ROWS;
int NUMBER_OF_COLS;
int FLOW_FLAG;
int MEDIAN_FLAG;
int FLOW_INF_FLAG;
int SLOPE_GRAD_FLAG;
int WETNESS_FLAG;

//there is no reason why these variables should be global except
//to ensure that TIFFGetField works properly in function extractWindow()
int mttCount, mpsCount;

char  OUT_BASE_DIR [256];
float **  IMAGE;
float **  MEDIAN;
float **  FLOW_DINF;
float **  SLOPE;

/**
 * Setup function called from python interface.
 */
static PyObject* Py_setup(PyObject* self, PyObject* args)
{
	char * inputFile, * outDirectory;
	
	if(!PyArg_ParseTuple(args, 
		"ssiiii", 
		&inputFile, 
		&outDirectory,
		&FLOW_FLAG, 
		&FLOW_INF_FLAG, 
		&SLOPE_GRAD_FLAG,
		&WETNESS_FLAG
		)
	)
		return NULL;
	
	MIN_THRESH = -1391;
	MAX_THRESH = 29035;
	GRID = 3;
		
	if(!setup(inputFile, outDirectory))
		return NULL;
	
	Py_INCREF(Py_None);
	return Py_None;
}

/*
 * Bind Python function names to C functions
 */
static PyMethodDef TM_module_methods[] = {
	{"setup", 			Py_setup, 			METH_VARARGS},
	{"calculateMedian", 		calculateMedian,		METH_VARARGS},
	{"calculateFlow", 		calculateFlow,			METH_VARARGS},
	{"calculateFlowDInfinity", 	calculateFlowDInfinity,		METH_VARARGS},    
	{"slopeGradient", 		calculateSlopeGradient, 	METH_VARARGS},
	{"calculateWetnessIndex", 	calculateWetnessIndex,	 	METH_VARARGS},
	{"slopeAspect", 		calculateSlopeAspect, 		METH_VARARGS},
	{"downSlopeCurvature", 		calculateDownSlopeCurvature, 	METH_VARARGS},
	{"acrossSlopeCurvature", 	calculateAcrossSlopeCurvature, 	METH_VARARGS},
	{"teardown", 			teardown, 			METH_VARARGS},
	{"relief",	 		relief, 			METH_VARARGS},
	{"windowFilter",	 	applyWindowFilter, 		METH_VARARGS},
	{"fileFilter",	 		applyFileFilter, 		METH_VARARGS},
	{NULL, NULL}
};

/*
 * Python calls this to let us initialize our module
 */
void initTM_module(void)
{
	(void) Py_InitModule("TM_module", TM_module_methods);
}

int main(void)
{
	return SUCCESS;
}

int writeTIFFWrapper(float ** buff, char * filePath, int lines, int samples, int sx, int sy)
{
	long size = (long)lines * (long)samples * 4;
	char path [256];

	// image size > 2 Gigabytes	
	if(size > pow(2,30))
	{
		printf("Splitting the image into half");
		
		int half;
		double ule, uln, y_scale;
		
		half = lines / 2;
		
		strcpy(path, filePath);
		strcat(path, "_part1.tif");
		
		if(!writeTIFF(buff, path, half, samples, sx, sy))
			return 0;
		
		ule = getUlEasting();
		uln = getUlNorthing();
		y_scale = getScaleY();
		
		strcpy(path, filePath);
		strcat(path, "_part2.tif");
		
		setMttData(0, 0, 0, ule, uln - (half*y_scale), 0);
		
		if(!writeTIFF(&buff[half], path, lines-half, samples, sx, sy))
			return 0;
		
		//undo setting mtt data
		setMttData(0, 0, 0, ule, uln, 0);
	}
	else
	{
		strcpy(path, filePath);
		strcat(path, ".tif");
		
		return writeTIFF(buff, path, lines, samples, sx, sy);
	}
	
	return 1;
	
}

int setup(char * filePath, char * outDirectory)
{
	printf("Running setup...\n");
	
	MEDIAN_FLAG		= 0;
	
	strcpy(OUT_BASE_DIR, outDirectory);
	
	char cmd [256];
	
	strcpy(cmd, "mkdir -p ");
	strcat(cmd, OUT_BASE_DIR);
	
	if(system(cmd) != 0)
	{
		PyErr_SetString(PyExc_IOError, "Could not create output directory.");
		return 0;
	}
	
	if(!readTIFF(filePath))
	{
		return 0;
	}
	
	return 1;
}

int extractWindow(double windowUlEast, double windowUlNorth, double windowBrEast, double windowBrNorth)
{
	double scaleX, scaleY, imageUlEast, imageUlNorth;
	
	int startRow, endRow, startCol, endCol;
	
	scaleX       = getScaleX();
	scaleY       = getScaleY();
	imageUlEast  = getUlEasting();
	imageUlNorth = getUlNorthing();
	
	startRow     = (int) ((imageUlNorth - windowUlNorth) / scaleY);
	endRow       = startRow + (int)((windowUlNorth - windowBrNorth) / scaleY);
	
	startCol     = (int) ((windowUlEast - imageUlEast) / scaleX);
	endCol       = startCol   + (int)((windowBrEast - windowUlEast)  / scaleX);
	
	if( (startRow < 0) || (endRow < startRow) || (endRow > IMAGE_LENGTH) ||
	    (startCol < 0) || (endCol < startCol) || (endCol > IMAGE_WIDTH ) )
	{
		printf("%d %d %d %d\n", startRow, endRow, startCol, endCol);
		PyErr_SetString(PyExc_IOError, "Bad bounding window coordinates.");
		return 0;
	}
	
	int numRows = endRow - startRow;
	int numCols = endCol - startCol;
	
	float ** temp;
	
	if((temp = initBuffer2D(numRows, numCols, 0.0)) == NULL)
	{
		return 0;
	}
	
	int row, col;
	for(row = 0; row < numRows; row++)
	{
		for(col = 0; col < numCols; col++)
		{
			temp[row][col] = IMAGE[row+startRow][col+startCol];
		}
	}
	
	freeBuffer2D(IMAGE, IMAGE_LENGTH);
	
	IMAGE_WIDTH  = NUMBER_OF_COLS = numCols;
	IMAGE_LENGTH = NUMBER_OF_ROWS = numRows;
	
	setMttData(0.0, 0.0, 0.0, windowUlEast, windowUlNorth, 0.0);
	
	IMAGE = temp;
	
	return 1;
}

/**
 * Extracts a window subset of the original image to calculate terrain modelling
 * metrics on.
 */
static PyObject* applyWindowFilter(PyObject* self, PyObject* args)
{
	printf("Applying window filter...\n");
	
	double windowUlEast, windowUlNorth, windowBrEast, windowBrNorth;
	
	if(!PyArg_ParseTuple(args, "dddd", &windowUlEast, &windowUlNorth, &windowBrEast, &windowBrNorth))
	{
		return NULL;
	}
	
	if(!extractWindow(windowUlEast, windowUlNorth, windowBrEast, windowBrNorth))
	{
		return NULL;
	}
	
	Py_INCREF(Py_None);
	return Py_None;
}

/** 
 * Extracts a window subset of the original image equal to the of the input 
 * reference geotiff with which to calculate the terrain modelling metrics.
 */
static PyObject* applyFileFilter(PyObject* self, PyObject* args)
{
	printf("applyFileFilter...\n");
	
	char * refFile;
	
	if(!PyArg_ParseTuple(args, "s", &refFile))
	{
		return NULL;
	}
	
	TIFF * refTiff;
	if((refTiff = XTIFFOpen((const char *)refFile, "r")) == NULL)
	{
		PyErr_SetString(PyExc_IOError, "Could not open reference file. TIFFOpen failed.");
		return NULL;
	}
	
	double  * mpsData, * mttData;
	//int 	  mpsCount, mttCount; -- globally defined
	
	TIFFGetField(refTiff, ModelPixelScaleTag, &mpsCount, &mpsData);
	TIFFGetField(refTiff, ModelTiepointTag,   &mttCount, &mttData);
	
	printf("mpsCount : %d mttCount : %d \n", mpsCount, mttCount);
	
	if(mpsCount < 3 || mttCount < 6)
	{
		PyErr_SetString(PyExc_IOError, "Missing 'ModelPixelScaleTag' and/or 'ModelTiepointTag' georeferencing information.");
		return NULL;
	}
	
	uint32 imageWidth, imageLength;
	
	TIFFGetField(refTiff, TIFFTAG_IMAGEWIDTH, &imageWidth);
	TIFFGetField(refTiff, TIFFTAG_IMAGELENGTH, &imageLength);
	
	double windowUlEast, windowUlNorth, windowBrEast, windowBrNorth, sx, sy;
	
	sx            = mpsData[0];
	sy            = mpsData[1];
	windowUlEast  = mttData[3]  - (mttData[0] * sx); //mttData[0] should be zero 
	windowUlNorth = mttData[4]  + (mttData[1] * sy); //mttData[1] should be zero
	windowBrEast  = windowUlEast + (imageWidth * sx);
	windowBrNorth = windowUlNorth - (imageLength * sy);
	
	XTIFFClose(refTiff);
	
	if(!extractWindow(windowUlEast, windowUlNorth, windowBrEast, windowBrNorth))
	{
		return NULL;
	}
	
	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * Teardown frees all dynamically allocated memory.
 */
static PyObject* teardown(PyObject* self, PyObject* args)
{
	printf("Running teardown...\n");
	
	freeBuffer2D(IMAGE, NUMBER_OF_ROWS);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * Builds file path of output file with name 'fileName'.
 */
char * buildOutPath(char filePath [256], char * fileName)
{
	strcpy(filePath, OUT_BASE_DIR);
	strcat(filePath, "/");
	strcat(filePath, fileName);
	
	return filePath;
}

/**
 * - Determine GLCM dimensions
 * - Create output directory for GLCM tiffs
 * - Create output tiff files for GLCM measures
 */
int processGlcm(float ** raster, char * dirName, int numLevels, int windowSize)
{
	int numMeasures,
		numRows,
		numCols,
		measure,
		numOffsets;
		
	char ** measureNames, outDir[256], command[256], outFile[256];
	
	float *** GLCM;
	
	int offsets [1][2] = {{0,1}};
	
	numOffsets   = 1;
	numRows		 = NUMBER_OF_ROWS/windowSize;
	numCols 	 = NUMBER_OF_COLS/windowSize;
	numMeasures  = getNumMeasures();
	measureNames = getMeasureNames();
	
	if((GLCM = initBuffer3D(numMeasures, numRows, numCols, 0.0)) == NULL)		return 0;
	puts("Glcm output allocated...");
		
	if(!calculateGLCM(raster, GLCM, numLevels, offsets, numOffsets, windowSize))return 0;
	
	//setup output directory and write to file
	strcpy(outDir, OUT_BASE_DIR);
	strcat(outDir, "/");
	strcat(outDir, dirName);
	strcat(outDir, "/");
	
	strcpy(command, "mkdir -p '");
	strcat(command, outDir);
	strcat(command, "'");
	
	//Note: return value of system call is system dependent
	//Create output directory
	if(system(command) != 0)
	{
		PyErr_SetString(PyExc_IOError, "Could not create relief GLCM output directory.");
		return 0;
	}
	
	for(measure = 0; measure < numMeasures; measure++)
	{
		strcpy(outFile, outDir);
		strcat(outFile, measureNames[measure]);
		strcat(outFile, ".tif");
		
		if(!writeTIFF(GLCM[measure], outFile, numRows, numCols, windowSize, windowSize))
		{
			return 0;
		}
	}
	freeBuffer3D(GLCM, numMeasures, numRows);
	
	return 1;
}

/**
 * Calculate
 * - GLCM measures
 * - range
 * - standard deviaton
 * for image relief.
 */
static PyObject* relief(PyObject* self, PyObject* args)
{
	printf("Relief...\n");
	
	int glcmLevels, windowSize;
	
	
	if(!PyArg_ParseTuple(args, "ii", &glcmLevels, &windowSize))			return NULL;
	
	if(!processGlcm(IMAGE, "GLCM_Relief", glcmLevels, windowSize))			return NULL;
	
	
	float * range, stdDev;
	
	range  = calculateRange(IMAGE, NUMBER_OF_ROWS, NUMBER_OF_COLS);
	stdDev = calculateStandardDeviation(IMAGE, NUMBER_OF_ROWS, NUMBER_OF_COLS);

	return Py_BuildValue("fff", range[0], range[1], stdDev); 
}


/**
 * Apply's a median filter to 'IMAGE' (relief) and save it in MEDIAN; MEDIAN
 * is used by both the D8 and D-Infinity flow algorithms. This function is 
 * only called if one of these flow metrics is to be calculated.
 */
static PyObject* calculateMedian(PyObject* self, PyObject* args)
{
	int row, col;
	
	if( (MEDIAN = (float **) _TIFFmalloc(sizeof(float *) * NUMBER_OF_ROWS)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return NULL;
	}
	
	for(row = 0; row < NUMBER_OF_ROWS; row++)
	{
		if( (MEDIAN[row] = (float *)_TIFFmalloc(NUMBER_OF_COLS * sizeof(float))) == NULL )
		{
			PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
			return NULL;
		}
		
		//copy 'IMAGE' into 'MEDIAN'
		for(col = 0; col < NUMBER_OF_COLS; col++)
			MEDIAN[row][col] = IMAGE[row][col];
	}
	
	MEDIAN_FLAG = 1;
	
	medianFilter(IMAGE, MEDIAN, 9, NUMBER_OF_ROWS, NUMBER_OF_COLS);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * Calculate D8 flow of DEM with median filter applied.
 */
static PyObject * calculateFlow(PyObject* self, PyObject* args)
{	
	printf("D8 flow\n");
	
	int band, glcmLevels, windowSize;
	if(!PyArg_ParseTuple(args, "iii", &band, &glcmLevels, &windowSize)) 		return NULL;
	
	float ** flowD8;
	if((flowD8 = initBuffer2D(NUMBER_OF_ROWS+10, NUMBER_OF_COLS, 1.0)) == NULL) 	return NULL;
	
	D8(MEDIAN, flowD8);
	
	if(!FLOW_INF_FLAG)
		freeBuffer2D(MEDIAN, NUMBER_OF_ROWS);
	
	char filePath [256];
	if(!writeTIFFWrapper(flowD8, buildOutPath(filePath, "Flow_D8"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))
		return NULL;
	//if(!writeTIFF(flowD8, buildOutPath(filePath, "Flow_D8.tif"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))	return NULL;
	
	if(!processGlcm(flowD8, "GLCM_D8", glcmLevels, windowSize))					return NULL;
	
	freeBuffer2D(flowD8, NUMBER_OF_ROWS);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * Calculate D-Infinity flow of DEM with median filter applied.
 */
static PyObject* calculateFlowDInfinity(PyObject* self, PyObject* args)
{
	printf("Calculating flow with D-infinity...\n");
	
	int band, glcmLevels, windowSize;
	if(!PyArg_ParseTuple(args, "iii", &band, &glcmLevels, &windowSize))				return NULL;
	
	if((FLOW_DINF = initBuffer2D(NUMBER_OF_ROWS, NUMBER_OF_COLS, 0.0)) == NULL) 	return NULL;
	
	Dinf(MEDIAN, FLOW_DINF);
	
	freeBuffer2D(MEDIAN, NUMBER_OF_ROWS);
	
	char filePath [256];
	if(!writeTIFFWrapper(FLOW_DINF, buildOutPath(filePath, "Flow_DInf"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))
		return NULL;
	//if(!writeTIFF(FLOW_DINF, buildOutPath(filePath, "Flow_DInf.tif"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1)) 	return NULL;
	
	if(!processGlcm(FLOW_DINF, "GLCM_DInf", glcmLevels, windowSize))				return NULL;
	
	if(!WETNESS_FLAG)
		freeBuffer2D(FLOW_DINF, NUMBER_OF_ROWS);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * Calculate slope gradient of DEM. 
 */
static PyObject* calculateSlopeGradient(PyObject* self, PyObject* args)
{	
	printf("Calculating slope gradient.\n");
	
	int band, glcmLevels, windowSize;
	
	if(!PyArg_ParseTuple(args, "iii", &band, &glcmLevels, &windowSize))				return NULL;
	
	if((SLOPE = initBuffer2D(NUMBER_OF_ROWS, NUMBER_OF_COLS, 0.0)) == NULL)			return NULL;
	
	slopeGradient(IMAGE, SLOPE);
	
	char filePath [256];
	if(!writeTIFFWrapper(SLOPE, buildOutPath(filePath, "SlopeGradient"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))
		return NULL;
	//if(!writeTIFF(SLOPE, buildOutPath(filePath, "SlopeGradient.tif"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1)) 	return NULL;
	
	if(!processGlcm(SLOPE, "GLCM_SlopeGradient", glcmLevels, windowSize))			return NULL;
	
	float * range = calculateRange(SLOPE, NUMBER_OF_ROWS, NUMBER_OF_COLS);
	float stdDev  = calculateStandardDeviation(SLOPE, NUMBER_OF_ROWS, NUMBER_OF_COLS);
	float mean    = calculateMean(SLOPE, NUMBER_OF_ROWS, NUMBER_OF_COLS);
	
	if(!WETNESS_FLAG)
		freeBuffer2D(SLOPE, NUMBER_OF_ROWS);
	
	return Py_BuildValue("ffff", range[0], range[1], stdDev, mean);
}

/**
 * Calculate the wetness index DEM using upslope area determined by the 
 * D-Infinity flow algorithm.
 */
static PyObject* calculateWetnessIndex(PyObject* self, PyObject* args)
{
	printf("Calculating wetness.\n");
	int glcmLevels, windowSize;
	if(!PyArg_ParseTuple(args, "ii", &glcmLevels, &windowSize))					return NULL;
	
	float ** wetness;
	
	if((wetness = initBuffer2D(NUMBER_OF_ROWS, NUMBER_OF_COLS, 0.0)) == NULL) 	return NULL;
	
	wetnessIndex(FLOW_DINF, SLOPE, wetness);
	
	freeBuffer2D(FLOW_DINF, NUMBER_OF_ROWS);
	freeBuffer2D(SLOPE,     NUMBER_OF_ROWS);
	
	char filePath [256];
	if(!writeTIFFWrapper(wetness, buildOutPath(filePath, "Wetness"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))
		return NULL;
	//if(!writeTIFF(wetness, buildOutPath(filePath, "Wetness.tif"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))	return NULL;
	
	if(!processGlcm(wetness, "GLCM_Wetness", glcmLevels, windowSize))			return NULL;
	
	freeBuffer2D(wetness, NUMBER_OF_ROWS);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * Calculate the slope aspect of DEM measured clockwise from the west.
 */
static PyObject* calculateSlopeAspect(PyObject* self, PyObject* args)
{
	printf("Calculating slope aspect.\n");
	
	int band, glcmLevels, windowSize, linearize, offset;
	if(!PyArg_ParseTuple(args, "iiiii", &band, &glcmLevels, &windowSize, &linearize, &offset))
	{
		return NULL;
	}
	
	float ** aspect;
	
	if((aspect = initBuffer2D(NUMBER_OF_ROWS, NUMBER_OF_COLS, 0.0)) == NULL)
	{
		return NULL;
	}
	
	slopeAspect(IMAGE, aspect, linearize, offset);
	
	char filePath [256];
	if(!writeTIFFWrapper(aspect, buildOutPath(filePath, "SlopeAspect"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))
		return NULL;
	//if(!writeTIFF(aspect, buildOutPath(filePath, "Slope_Aspect.tif"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))
	//{
	//	return NULL;
	//}
	
	if(!processGlcm(aspect, "GLCM_SlopeAspect", glcmLevels, windowSize))		return NULL;
	
	freeBuffer2D(aspect, NUMBER_OF_ROWS);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * - calculate downslope curvature of DEM and write to tiff file
 * - return curvature mean to interface
 */
static PyObject* calculateDownSlopeCurvature(PyObject* self, PyObject* args)
{
	printf("Calculating ds curvature.\n");
	
	int band, glcmLevels, windowSize;
	
	if(!PyArg_ParseTuple(args, "iii", &band, &glcmLevels, &windowSize)){
		return NULL;
	}
	
	float ** dCurv;
	
	if((dCurv = initBuffer2D(NUMBER_OF_ROWS, NUMBER_OF_COLS, 0.0)) == NULL){
		return NULL;
	}
	
	downSlopeCurvature(IMAGE, dCurv);
	
	char filePath [256];
	if(!writeTIFFWrapper(dCurv, buildOutPath(filePath, "DownSlopeCurvature"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))
		return NULL;
	//if(!writeTIFF(dCurv, buildOutPath(filePath, "DownSlopeCurvature.tif"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1)){
	//	return NULL;
	//}
	
	if(!processGlcm(dCurv, "GLCM_DownslopeCurvature", glcmLevels, windowSize)){
		return NULL;
	}
	
	float mean = calculateMean(dCurv, NUMBER_OF_ROWS, NUMBER_OF_COLS);
	
	freeBuffer2D(dCurv, NUMBER_OF_ROWS);
	
	return Py_BuildValue("f", mean);
}

/**
 * - Calculate acrossslope curvature and write to tiff file
 * - Rerturn curvature mean to interface
 */
static PyObject* calculateAcrossSlopeCurvature(PyObject* self, PyObject* args)
{
	printf("Calculating as curvature.\n");
	
	int band, glcmLevels, windowSize;
	if(!PyArg_ParseTuple(args, "iii", &band, &glcmLevels, &windowSize))				return NULL;
	
	float ** aCurv;
	
	if((aCurv = initBuffer2D(NUMBER_OF_ROWS, NUMBER_OF_COLS, 0.0)) == NULL) 			return NULL;
	
	acrossSlopeCurvature(IMAGE, aCurv);
	
	char filePath [256];
	if(!writeTIFFWrapper(aCurv, buildOutPath(filePath, "AcrossSlopeCurvature"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1))
		return NULL;
	//if(!writeTIFF(aCurv, buildOutPath(filePath, "AcrossSlopeCurvature.tif"), IMAGE_LENGTH, IMAGE_WIDTH, 1, 1)) 	return NULL;
	
	if(!processGlcm(aCurv, "GLCM_AcrossslopeCurvature", glcmLevels, windowSize))			return NULL;
	
	float mean = calculateMean(aCurv, NUMBER_OF_ROWS, NUMBER_OF_COLS);
	
	freeBuffer2D(aCurv, NUMBER_OF_ROWS);
	
	return Py_BuildValue("f", mean);
}

/*
	double imageUlEast, imageUlNorth, scaleX, scaleY;
	int startRow, startCol, endRow, endCol;
	
	//potentially add error handling.... 
	scaleX       = getScaleX();
	scaleY       = getScaleY();
	imageUlEast  = getUlEasting();
	imageUlNorth = getUlNorthing();
	
	startRow     = (int) ((imageUlNorth - windowUlNorth) / scaleY);
	endRow       = startRow + (int)((windowUlNorth - windowBrNorth) / scaleY);
	
	startCol     = (int) ((windowUlEast - imageUlEast) / scaleX);
	endCol       = startCol   + (int)((windowBrEast - windowUlEast)  / scaleX);
	
	if( (startRow < 0) || (endRow < startRow) || (endRow >= IMAGE_LENGTH) ||
	    (startCol < 0) || (endCol < startCol) || (endCol >= IMAGE_WIDTH ) )
	{
		printf("%d %d %d %d\n", startRow, endRow, startCol, endCol);
		printf("%lf %lf %lf %lf\n", windowUlEast, windowUlNorth, windowBrEast, windowBrNorth);
		PyErr_SetString(PyExc_IOError, "Bad window.");
		return NULL;
	}
	
	NUMBER_OF_ROWS = (endRow - startRow) + 1;
	NUMBER_OF_COLS = (endCol - startCol) + 1;
	
	float ** temp;
	
	if((temp = initBuffer2D(NUMBER_OF_ROWS, NUMBER_OF_COLS, 0.0)) == NULL)
	{
		return NULL;
	}
	
	int row, col;
	for(row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for(col = 0; col < NUMBER_OF_COLS; col++)
		{
			temp[row][col] = IMAGE[row+startRow][col+startCol];
		}
	}
	
	freeBuffer2D(IMAGE, IMAGE_LENGTH);
	
	IMAGE_WIDTH  = NUMBER_OF_COLS;
	IMAGE_LENGTH = NUMBER_OF_ROWS;
	
	setMttData(0.0, 0.0, 0.0, windowUlEast, windowUlNorth, 0.0);
	
	IMAGE = temp;
	*/
	
	/* 
	
	Apply file filter...........
	
	scaleX       = getScaleX();
	scaleY       = getScaleY();
	imageUlEast  = getUlEasting();
	imageUlNorth = getUlNorthing();
	
	startRow     = (int) ((imageUlNorth - windowUlNorth) / scaleY);
	endRow       = startRow + (int)((windowUlNorth - windowBrNorth) / scaleY);
	
	startCol     = (int) ((windowUlEast - imageUlEast) / scaleX);
	endCol       = startCol   + (int)((windowBrEast - windowUlEast)  / scaleX);
	
	if( (startRow < 0) || (endRow < startRow) || (endRow >= IMAGE_LENGTH) ||
	    (startCol < 0) || (endCol < startCol) || (endCol >= IMAGE_WIDTH ) )
	{
		printf("%d %d %d %d\n", startRow, endRow, startCol, endCol);
		printf("%lf %lf %lf %lf\n", windowUlEast, windowUlNorth, windowBrEast, windowBrNorth);
		PyErr_SetString(PyExc_IOError, "Bad window.");
		return NULL;
	}
	
	NUMBER_OF_ROWS = (endRow - startRow) + 1;
	NUMBER_OF_COLS = (endCol - startCol) + 1;
	
	float ** temp;
	
	if((temp = initBuffer2D(NUMBER_OF_ROWS, NUMBER_OF_COLS, 0.0)) == NULL)
	{
		return NULL;
	}
	
	int row, col;
	for(row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for(col = 0; col < NUMBER_OF_COLS; col++)
		{
			temp[row][col] = IMAGE[row+startRow][col+startCol];
		}
	}
	
	freeBuffer2D(IMAGE, IMAGE_LENGTH);
	
	IMAGE_WIDTH  = NUMBER_OF_COLS;
	IMAGE_LENGTH = NUMBER_OF_ROWS;
	
	setMttData(0.0, 0.0, 0.0, windowUlEast, windowUlNorth, 0.0);
	
	IMAGE = temp;
	*/