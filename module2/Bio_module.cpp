
#include "Bio_module.hpp"



double ** POINTS;
float *** Z_VALUES;	//array of all the z values of the input file above threshold
float *** Z_ALL;	//array of all the z values of the input file
int    ** NUM_Z;	//the number of z values > Z_THRESHOLD
int    ** NUM_ALL;	//the number of z values in each cell

//global user input
char OUT_DIR [256];
double MIN_EASTING;	//grid easting minimum
double MIN_NORTHING;	//grid norhting minimum
float  Z_THRESHOLD;		//remove ground / noise
float  Z_MAX;			//bye bye birdy filter height
int    BLK_SIZE;	//side length of the block represented by the input file
int    OUT_RESOLUTION;	//the size each pixel represents

double  BLK_EASTING;
double  BLK_NORTHING;
int  	NUM_POINTS;	//number of points in the input file
int   	X_POS;		//x block position
int   	Y_POS;		//y block position
int  	DIMENSIONS;	//the x and y dimension of the output tiff



/**
 * Returns partially allocated 3D buffer.
 */
float *** initZBuffer(int dimensions)
{
	float *** zBuf;
	if( (zBuf = (float ***)_TIFFmalloc(sizeof(float **) * dimensions)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return NULL;
	}
	
	int i;	
	for(i = 0; i < dimensions; i++)
	{	
		if( (zBuf[i] = (float **)_TIFFmalloc(sizeof(float *) * dimensions)) == NULL)
		{
			PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
			return NULL;
		}
	}
	
	return zBuf;
}

/** 
 * Returns the path to the ouput file and creates the directory.
 * NOTE : Allocates memory that it does not free. Make sure pointer assigned to 
 *        memory returned are freed.
 */
char * buildOutPath(char * dName, char * fName)
{
	char pDir [256];
	
	strcpy(pDir, OUT_DIR);
	strcat(pDir, "/");
	strcat(pDir, dName);
	
	char cmd [256];
	
	strcpy(cmd, "mkdir -p ");
	strcat(cmd, pDir);
	
	if(system(cmd) != 0)
	{
		PyErr_SetString(PyExc_IOError, "Could not create ouput dir.");
		return NULL;
	}
	
	char xStr [16], yStr [16];
	
	sprintf(xStr, "%d", X_POS);
	sprintf(yStr, "%d", Y_POS);
	
	char * filePath = (char *)_TIFFmalloc(sizeof(char) * 256);
	
	strcpy(filePath, pDir);
	
	strcat(filePath, "/");
	strcat(filePath, fName);
	strcat(filePath, "_");
	strcat(filePath, xStr);
	strcat(filePath, "_");
	strcat(filePath, yStr);
	strcat(filePath, ".tif");
	
	return filePath;
}

int extractGridPosition(char * filePath, int * blkPos [2])
{
	const char * tok = "._abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	char * cPtr = 
	strrchr(filePath, '/') + 1;
	
	char fileName [strlen(cPtr)+1];
	
	strcpy(fileName, cPtr);
	
	int index;
	
	index = 0;
	cPtr  = strtok(fileName, tok);
	
	while(cPtr != NULL){
		*blkPos[index++] = atoi(cPtr);
		cPtr = strtok(NULL, tok);
	}
	
	if(index != 2) {
		PyErr_SetString(PyExc_IOError, "Input file format error.");
		printf("index:%d\n", index);
		return 0;
	}
	
	return 1;
}

double ** bufferPoints(char * path)
{
	char * type = (char *)(path + strlen(path) - 4);
	
	if(strcmp(type, ".bin") == 0)
	{
		puts("Buffering .bin");
		return bufferTerraScanBinFile(path, &NUM_POINTS);
	}
	else if(strcmp(type, ".las") == 0)
	{
		puts("Buffereing .las");
		return bufferLasFile(path, &NUM_POINTS);
	}
	else{
		return NULL;
	}	
}

int setup(char * inputFile, char * outputDir, double * ulEastPtr, double * brEastPtr, double * ulNorthPtr, double * brNorthPtr)
{
	puts("Bio_module : in setup");
	
	strcpy(OUT_DIR, outputDir);
	
	puts("Bio_module : reading input file");
	
	if((POINTS = bufferPoints(inputFile)) == NULL) return 0;
	
	if(NUM_POINTS < 1)
		return 0;
	
	X_POS = (int)floor((POINTS[0][0] - MIN_EASTING)  / BLK_SIZE);
	Y_POS = (int)floor((POINTS[1][0] - MIN_NORTHING) / BLK_SIZE);

	printf("x_pos:%d\ny_pos:%d\n", X_POS, Y_POS);
	printf("num points : %d \n", NUM_POINTS);
	
	DIMENSIONS   = BLK_SIZE / OUT_RESOLUTION;
	BLK_EASTING  = MIN_EASTING + (X_POS * BLK_SIZE);
	BLK_NORTHING = MIN_NORTHING + (Y_POS * BLK_SIZE);
	
	*ulEastPtr   = BLK_EASTING;
	*ulNorthPtr  = BLK_NORTHING - BLK_SIZE;
	*brEastPtr   = BLK_EASTING  + BLK_SIZE;
	*brNorthPtr  = BLK_NORTHING;
	
	printf("Max E : %lf,%lf N : %lf,%lf Z : %lf \n", getDoubleMin(POINTS[0], NUM_POINTS), getDoubleMax(POINTS[0], NUM_POINTS), getDoubleMin(POINTS[1], NUM_POINTS), getDoubleMax(POINTS[1], NUM_POINTS), getDoubleMax(POINTS[2], NUM_POINTS));
	printf("minE %lf\n", MIN_EASTING);
	printf("minN %lf\n", MIN_NORTHING);
	printf("blkS %d\n", BLK_SIZE);
	printf("xPos %d\n", X_POS);
	printf("yPos %d\n", Y_POS);
	printf("blkE %lf\n", BLK_EASTING);
	printf("blkN %lf\n", BLK_NORTHING);
	
	if((NUM_Z    = initIntBuffer2D(DIMENSIONS, DIMENSIONS, 0.0)) == NULL) return 0;
	if((NUM_ALL  = initIntBuffer2D(DIMENSIONS, DIMENSIONS, 0.0)) == NULL) return 0;
	if((Z_VALUES = initZBuffer(DIMENSIONS)) == NULL) return 0;
	if((Z_ALL    = initZBuffer(DIMENSIONS)) == NULL) return 0;
	
	//double xStart, yStart, xEnd, yEnd;
	//int j, i;
	
	puts("Bio_module : setting up buffers");
	
	gridPoints(DIMENSIONS, OUT_RESOLUTION, BLK_EASTING, BLK_NORTHING, POINTS,
		NUM_POINTS, NUM_Z, NUM_ALL, Z_VALUES, Z_ALL, Z_THRESHOLD, Z_MAX);
	
	freeDoubleBuffer2D(POINTS,  3);
	
	return 1;
}

static PyObject* Py_setup(PyObject * self, PyObject * args)
{
	
	char * inputFile, * outputDir;
	
	if(!PyArg_ParseTuple( args, "ssddiiff", 
		
		&inputFile,
		&outputDir,
		&MIN_EASTING,
		&MIN_NORTHING,
		&BLK_SIZE,
		&OUT_RESOLUTION,
		&Z_THRESHOLD,
		&Z_MAX)
	)
	{
		return NULL;
	}
	
	double minEasting, maxEasting, minNorthing, maxNorthing;
	
	if(!setup(inputFile, outputDir, &minEasting, &maxEasting, &minNorthing, &maxNorthing))
	{
		return NULL;
	}
	
	return Py_BuildValue("ffff", minEasting, maxEasting, minNorthing, maxNorthing);
}

/**
 * outArr[0] = rugosity
 * outArr[1] = gap fraction
 * outArr[2] = mean above 85th percentile
 * outArr[3-6] = l-moments
 * outArr[7] = 85th percentile
 * outArr[8] = number of points above threshold
 * outArr[9] = number of points
 */
static PyObject* Py_Biometrics(PyObject * self, PyObject * args)
{
	puts("Py_Biometrics : Compute biometrics.");
	
	int numBands = 10;
	
	float *** outArr;
	if( (outArr = initFloatBuffer3D(numBands, DIMENSIONS, DIMENSIONS, -999.0)) == NULL )	return NULL;
	
	int j, i;
	for(j = 0; j < DIMENSIONS; j++)
	{
		for(i = 0; i < DIMENSIONS; i++)
		{
			float * zValues = Z_VALUES[j][i];
			int     numZ    = NUM_Z[j][i];
			int     numAll  = NUM_ALL[j][i];
			
			outArr[8][j][i] = numZ;
			outArr[9][j][i] = numAll;
			
			printf("OUT_RESOLUTION: %d\n", OUT_RESOLUTION);
			
			if((OUT_RESOLUTION >= 20 && numZ < 75) || (OUT_RESOLUTION < 20 && numZ < 10))
			{
				continue;	
			}
			
			computeRugosity(zValues, numZ, &outArr[0][j][i]);
			computeGap(numZ, numAll, &outArr[1][j][i]);
			computePercentile(zValues, numZ, 0.85, &outArr[2][j][i]);
			
			float * lMoments =
			computeLMoments(zValues,  numZ);
			outArr[3][j][i] = lMoments[0];
			outArr[4][j][i] = lMoments[1];
			outArr[5][j][i] = lMoments[2];
			outArr[6][j][i] = lMoments[3];
			
			
			_TIFFfree(lMoments);
			
			//this could be extracted to bio_biometrics
			float rank, diff;
			int   base, rank1, rank2;
			
			rank = (0.85) * (numZ+1);
			base = (int)floor(rank);
			diff = rank - base;
			
			rank1 = base - 1;
			rank2 = base;
			
			outArr[7][j][i] = (1.0 - diff) * zValues[rank1] + diff * zValues[rank2];
		}
	}
	
	char * fPath = buildOutPath("bio", "bio");
	
	writeTiff
	(
		outArr, 
		fPath, 
		numBands, 
		DIMENSIONS, 
		OUT_RESOLUTION, 
		BLK_EASTING, 
		BLK_NORTHING + BLK_SIZE
	);
	
	_TIFFfree(fPath);
	freeFloatBuffer3D(outArr, numBands, DIMENSIONS);
	
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* Py_LHQ(PyObject * self, PyObject * args)
{
	puts("Bio_module : calc lhq");
	
	float *** lhqArr;
	if( (lhqArr = initFloatBuffer3D(21, DIMENSIONS, DIMENSIONS, -999.0)) == NULL )	return NULL;
	
	int j, i;
	for(j = 0; j < DIMENSIONS; j++)
	{
		for(i = 0; i < DIMENSIONS; i++)
		{
			float * zValues = Z_ALL[j][i];
			int     numZ    = NUM_ALL[j][i];
			
			if((OUT_RESOLUTION >= 20 && numZ < 75) || (OUT_RESOLUTION < 20 && numZ < 25))
			{
				continue;	
			}
			
			float rank, diff;
			int   base, band, rank1, rank2;
			
			lhqArr[0][j][i] = zValues[0]; //boundary case
			
			for(band = 1; band < 20; band++)
			{
				rank  = (band/20.0) * (numZ+1);
				base  = (int)floor(rank);
				diff  = rank - base;
				
				rank1 = base-1; if(rank1 < 0) rank1 = 0;
				rank2 = base;
				
				lhqArr[band][j][i] = ((1.0 - diff) * zValues[rank1]) + (diff * zValues[rank2]);
			}
			
			lhqArr[20][j][i] = zValues[numZ-1]; //boundary case
		}
	}
	
	char * fPath = buildOutPath("lhq", "lhq");
	
	writeTiff
	(
		lhqArr, 
		fPath, 
		21, 
		DIMENSIONS, 
		OUT_RESOLUTION, 
		BLK_EASTING, 
		BLK_NORTHING + BLK_SIZE
	);
	
	_TIFFfree(fPath);
	freeFloatBuffer3D(lhqArr, 21, DIMENSIONS);
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* Py_CCF(PyObject * self, PyObject * args)
{
	puts("Bio_module : calc ccf");
	
	float *** ccfArr;
	
	if( (ccfArr = initFloatBuffer3D(21, DIMENSIONS, DIMENSIONS, -999.0)) == NULL )	return NULL;
	
	int j, i;
	for(j = 0; j < DIMENSIONS; j++)
	{
		for(i = 0; i < DIMENSIONS; i++)
		{
			float * zValues = Z_VALUES[j][i];
			int     numZ    = NUM_Z[j][i];
			int     numAll  = NUM_ALL[j][i];
			
			if((OUT_RESOLUTION >= 20 && numZ < 75) || (OUT_RESOLUTION < 20 && numZ < 25))
			{
				continue;	
			}
			
			int band, ccfCount;
			float curHeight, ccfPercent, htIncrement;
			
			htIncrement = (zValues[numZ-1] - Z_THRESHOLD) / 20.0;
			curHeight   = Z_THRESHOLD;
			
			for(band = 0; band <= 20; band++)
			{
				ccfCount   = getCCFCount(zValues, numZ, curHeight);
				ccfPercent = ccfCount / (float) numAll;
				
				ccfArr[band][j][i] = ccfPercent;
				
				curHeight += htIncrement;
			}
		}
	}
	
	char * fPath = buildOutPath("ccf", "ccf");
	
	writeTiff
	(
		ccfArr, 
		fPath, 
		21, 
		DIMENSIONS, 
		OUT_RESOLUTION, 
		BLK_EASTING, 
		BLK_NORTHING + BLK_SIZE	
	);
	
	_TIFFfree(fPath);
	freeFloatBuffer3D(ccfArr, 21, DIMENSIONS);
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* Py_teardown(PyObject * self, PyObject * args)
{
	freeFloatBuffer3D(Z_VALUES, DIMENSIONS, DIMENSIONS);
	freeFloatBuffer3D(Z_ALL,    DIMENSIONS, DIMENSIONS);
	freeIntBuffer2D(NUM_Z, DIMENSIONS);
	freeIntBuffer2D(NUM_ALL, DIMENSIONS);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * Renames all the files in the specified directory.
 * NOTE : Could replace the control loop with python.
 */
static PyObject* Py_Rename(PyObject * self, PyObject * args)
{
	puts("Py_Rename : Coordinate renaming.");
	
	char * inFile, * outDir;
	
	float zThreshold;
	
	double minEasting, minNorthing;
	
	int blockSize, outText, outBin;
	
	if(!PyArg_ParseTuple( args, "ssddifii", &inFile, &outDir, &minEasting, &minNorthing, &blockSize, &zThreshold,
		&outText, &outBin))
	{
		return NULL;
	}
	
	char * outFile;
	/*
	if((outFile = renameFile(inFile, outDir, minEasting, minNorthing, blockSize, zThreshold)) == NULL)
	{
		printf("Rename failed somewhere.\n");
		return NULL;
	}
	*/
	if((outFile = renameBinFile(inFile, outDir, minEasting, minNorthing, blockSize, zThreshold, outText, outBin)) == NULL){
		return NULL;
	}
	
	return Py_BuildValue("s", outFile);
}

static PyObject* Py_computeBiometricsAroundCentroid(PyObject * self, PyObject * args)
{
	puts("Computing biometrics around centroid.");
	
	double centroidEasting, centroidNorthing, minEasting, minNorthing;
	
	char * chmDir, * outFile;
	
	int radius, blockSize, zThreshold, point;
	
	if(!PyArg_ParseTuple( args, "iddddiiiss",
		&point,
		&centroidEasting,
		&centroidNorthing,
		&minEasting,
		&minNorthing,
		&radius,
		&blockSize,
		&zThreshold,
		&chmDir,
		&outFile))
	{
		return NULL;
	}
	
	if(!centroidBiometrics
		(
			point,
			centroidEasting, 
			centroidNorthing, 
			radius, 
			minEasting, 
			minNorthing, 
			blockSize, 
			zThreshold, 
			chmDir, 
			outFile
		)
	)
	{	
		return NULL;
	}
	
	
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* Py_computeBiometricsInBoundingBox(PyObject * self, PyObject * args)	
{
	puts("Computing biometrics in square.");
	
	double centroidEasting, centroidNorthing, minEasting, minNorthing;
	
	char * chmDirectory, * outFile;
	
	int sideLength, blockSize, zThreshold, point;
	
	if(!PyArg_ParseTuple( args, "iddddiiiss",
		&point,
		&centroidEasting,
		&centroidNorthing,
		&minEasting,
		&minNorthing,
		&sideLength,
		&blockSize,
		&zThreshold,
		&chmDirectory,
		&outFile))
	{
		return NULL;
	}
	
	if(!boundingBoxBiometrics
		(
			point,
			centroidEasting, 
			centroidNorthing, 
			sideLength, 
			minEasting, 
			minNorthing, 
			blockSize, 
			zThreshold, 
			chmDirectory, 
			outFile
		)
	)
	{	
		return NULL;
	}
	
	
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef Bio_module_methods[] = {
	{"setup", 		Py_setup, 		METH_VARARGS},
	{"bio", 		Py_Biometrics, 		METH_VARARGS},
	{"lhq", 		Py_LHQ, 		METH_VARARGS},
	{"ccf", 		Py_CCF,			METH_VARARGS},
	{"teardown",		Py_teardown,		METH_VARARGS},
	{"rename",		Py_Rename,		METH_VARARGS},
	{"computeBiometricsAroundCentroid",	Py_computeBiometricsAroundCentroid,	METH_VARARGS},
	{"computeBiometricsInBoundingBox",	Py_computeBiometricsInBoundingBox,	METH_VARARGS},
	{NULL, NULL}
};

PyMODINIT_FUNC
initBio_module(void)
{
	(void) Py_InitModule("Bio_module", Bio_module_methods);
}

