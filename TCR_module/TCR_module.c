

/**
 * To compile : export MACOSX_DEPLOYMENT_TARGET=10.6
 *            : python setup.py install
 *       then : python interface.py
 * To build   : python setup_app.py py2app
 **/

#include <shapefil.h>
#include <Python/Python.h>
#include "TCR_module.h"
#include "dirent.h"

int extractPoints(int shapeIndex, FILE * fpOut);

typedef struct 
{	
	char * data;
	int size;
}
CharArray;

/**
 * GLOBAL VARIABLES
 */

int 		N_RECORDS;
int 		N_POINTS;				// points left to process
int 		ARRAY_SIZE				=	64;
int 		BLOCK_SIZE;
int			OUT_RES;

double 		MIN_E, 
			MIN_N;

// classifies the points as either lying inside or outside the tree crown
char * 		POINT_CLASSIFICATION;

float		Z_THRESH;
float * 	Z_VALUES;

double ** 	CHM;

SHPHandle 	H_SHP;

// indexes which tree crowns need to be process for the current CHM file
CharArray	tciFlag;

float * resize(float array [], int * pSize)
{
	int oldSize, newSize, i;
	
	oldSize = *pSize;
	newSize = oldSize * 2;
	
	float * newArray = allocf1d(newSize);
	if(!newArray){
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return NULL;
	}
	
	/**
	 * Copy array into newArray
	 */
	for(i = 0; i < oldSize; i++)
		newArray[i] = array[i];
	free(array);
	
	/**
	 * Update array size
	 */
	*pSize = *pSize * 2;
	
	return newArray;
}

void setZero(char array [], int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		array[i] = 0;
	}
}

int floatCompare(const void * x, const void * y)
{
	float difference = *(float*)x - *(float*)y;
	
	if(difference > 0)
		return 1;
	else if(difference == 0)
		return 0;
	else
		return -1;
}

int intersects(double e, double n, double e1, double n1, double e2, double n2)
{
	double  y,	//y intersect 
		x;		//x intersect
	
	double m1 = 1.41421;
		
	/**
	 * Handle case where e1 = e2 and n1 = n2.
	 */
	
	if(e1 == e2 && n1 == n2)
		 return 0;
	
	/**
	 * Line throught (e,n) with slope 1.
	 * y  = x + b1
	 * b1 = n - e
	 * m1 = 1
	 */
	 
	double b1 = n - m1*e;
	 
	 
	/**
	 * Handle case where m2 = infinity
	 * y = e1 + b1
	 * Intersection must be x = e1 = e2
	 */
	
	if(e1 == e2)
	{	 
		y = m1*e1 + b1;
		if(e1 < e  || 
		  ((y < n1 || y >= n2) && (y <= n2 || y > n1)) ){
			return 0;
		}
		//printf("e1 == e2 (%.2lf,%.2lf) yint: %.2lf e1: %.2lf  e2: %.2lf n1: %.2lf n2: %.2lf\n", e, n, y, e1, e2, n1, n2);
		return 1;
	}
	
	/**
	 * Handle case where m2 = 0
	 * Intersection must be at y = n1 = n2
	 * n1 = m1x + b1 ==> x = (n1 - b1)/m1
	 */
	if(n1 == n2)
	{
		x = (n1 - b1)/m1;
		
		if( x < e  ||
		  ((x < e1 || x >= e2) && (x <= e2 || x > e1)) ){
			return 0;
		}
		//printf("n1 == n2 (%.2lf,%.2lf) xint: %.2lf e1: %.2lf  e2: %.2lf n1: %.2lf n2: %.2lf\n", e, n, x, e1, e2, n1, n2);
		return 1;
	}
	
	
	/**
	 * Line through (e1,n1) and (e2,n2).
	 * y = m2x + b2
	 */
	
	double m2 = (n2 - n1)/(e2 - e1);
	double b2 = n1 - m2*e1;
	
	
	/**
	 * If b1 = b2 then intersection point is (0, b1).
	 */
	
	if(b1 == b2)
	{
		if((0 < e) ||
		  ((0 < e1 || 0 >= e2) && (0 <= e2 || 0 > e1)) || 
		  ((b1 < n1 || b1 >= n2) && (b1 <= n2 || b2 > n1)) ){
			return 0;
		}
	  return 1;
	}
	
	/**
	 * Solve for x
	 * x + b1 = m2x + b2 ==> (m2 - 1)/(b1 - b2)
	 */
	
	x = (b2 - b1)/(m1 - m2);
	
	/**
	 * Make sure x is between e1 and e2 and x >= e
	 */
	if( x < e ||
	  ((x < e1 || x >= e2) && (x <= e2 || x > e1)) ){
		return 0;
	}
	
	/**
	 * Solve for y using either equation/
	 */
	
	y = x + b1;
	    
	/**
	 * Make sure y is between y1 and y2
	 */
	 
	if( (y < n1 || y >= n2) && (y <= n2 || y > n1) ){
		return 0;
	}
	
	//printf("*** xint %.2lf yint: %.2lf m: %.2lf e1: %.2lf  e2: %.2lf n1: %.2lf n2: %.2lf\n", x, y, m2, e1, e2, n1, n2);
	return 1;
}

int pointInPolygon(SHPObject * pShpObj, double x, double y)
{
	int j, nVerts, intersectionCount = 0;
	double x1, y1, x2, y2, * xVerts, * yVerts;
	
	xVerts = pShpObj->padfX;
	yVerts = pShpObj->padfY;
	nVerts = pShpObj->nVertices;
		
	for(j = 0; j < nVerts; j++)
	{
		x1 = xVerts[j];
		y1 = yVerts[j];
		
		if(j == nVerts-1)
		{
			x2 = xVerts[0];
			y2 = yVerts[0];
		}
		else
		{
			x2 = xVerts[j+1];
			y2 = yVerts[j+1];
		}
		
		if( intersects(x, y, x1, y1, x2, y2) == 1 )
				intersectionCount++;
	}
	
	if(intersectionCount % 2 == 0)
		return 0;
	
	return 1;
	
}

//TODO : move this to io.c
int readChm(FILE * fpIn)
{	
	CHM = buffFile(fpIn, &N_POINTS);
	if(!CHM)
	{
		return 0;
	}
	
	if((POINT_CLASSIFICATION = allocc1d(N_POINTS)) == NULL)
	{
		return 0;
	}
	
	return 1;
}

/**
 * Main control logic.
 * - processes a single CHM
 * - sets tciFlag indicating which tree crowns intersect the current CHM
 * - calls extractPoints
 */
static PyObject* Py_execute(PyObject* self, PyObject* args)
{
	int xBlockPos, yBlockPos, bioFlag;
	char *inFile, *outFile, *outTiff;
	
	if(!PyArg_ParseTuple(args, "ssiiis", &inFile, &outFile, &xBlockPos, &yBlockPos, &bioFlag, &outTiff))
	{
		PyErr_SetString(PyExc_IOError, "Py_execute could not parse parameters.");
		return NULL;
	}
	
	FILE * fpIn, * fpOut;
	
	if(!(fpIn = fopen(inFile, "r"))){
		PyErr_SetString(PyExc_IOError, "Could not open input file.");
		return NULL;
	}
	if(!(fpOut = fopen(outFile, "w"))){
		PyErr_SetString(PyExc_IOError, "Could not open output file.");
		return NULL;
	}
	
	int i, process, **numAll;
	double xBlockMin, xBlockMax, yBlockMin, yBlockMax;
	SHPObject * pShpObj;
	
	xBlockMin = MIN_E + (xBlockPos * BLOCK_SIZE);
	xBlockMax = xBlockMin + BLOCK_SIZE;
	yBlockMin = MIN_N + (yBlockPos * BLOCK_SIZE);
	yBlockMax = yBlockMin + BLOCK_SIZE;
	
	process = 0;
	
	//index all tree crowns that intersect the current file
	for(i = 0; i < N_RECORDS; i++)
	{
		if((pShpObj = SHPReadObject(H_SHP, i)) == NULL)
		{
			PyErr_SetString(PyExc_IOError, "Error opening shape file.");
			return NULL;
		}
		
		if(pShpObj->dfXMin >= xBlockMax || pShpObj->dfXMax <= xBlockMin
			|| pShpObj->dfYMin >= yBlockMax || pShpObj->dfYMax <= yBlockMin)
		{
			tciFlag.data[i] = 0;
		}
		else{
			process = 1;
			tciFlag.data[i] = 1;
		}
		
		SHPDestroyObject(pShpObj);
	}
	
	if(!process){
		printf("Nothing to process\n");
		return Py_None;
	}
	
	if(!readChm(fpIn))
		return NULL;
	
	if(bioFlag)
	{
		numAll = countPoints(CHM, N_POINTS, MIN_E, MIN_N, BLOCK_SIZE, OUT_RES, xBlockPos, yBlockPos);
		if(!numAll)
			return NULL;
		int dim = BLOCK_SIZE/OUT_RES;
		int i,j;
		for(i = 0;i<dim;i++)
			for(j=0;j<dim;j++)
				printf("%d\n", numAll[i][j]);
		
	}
	
	//process each file
	for(i = 0; i < tciFlag.size; i++)
	{
		if(tciFlag.data[i] == 0)continue;
		
		printf("Calling:\textractPoints(%d, fpOut)\n", i);
		extractPoints(i, fpOut);
	}
	
	
	if(bioFlag)
	{
		printf("Run ccf calcs\n");
		computeCCF(CHM, N_POINTS, numAll, MIN_E, MIN_N, BLOCK_SIZE, OUT_RES, Z_THRESH, xBlockPos, yBlockPos, outTiff);
	}
	
	printf("Calling:\twriteFile(fpOut, CHM, %d)\n", N_POINTS);
	if(!writeFile(fpOut, CHM, N_POINTS))
	{
		return NULL;
	}
	
	freed2d(CHM, 3);
	free(POINT_CLASSIFICATION);
	fclose(fpIn);
	fclose(fpOut);
	
	return Py_None;
}

//assumes Z_VALUES is sorted in increasing order
float * pointsAbove2m(int * nAbove2m, int pointsInCrown)
{
	int i;
	for(i = 0; i < N_POINTS; i++)
		if(Z_VALUES[i] > 2.0)
			break;
	*nAbove2m = pointsInCrown - i;
	return &Z_VALUES[i];
}

/**
 * - classifyies points in/outside crown
 * - filters based on z values
 * - writes points to file
 */
int extractPoints(int shapeIndex, FILE * fpOut)
{
	printf("\tCrown Index  : %d\n", (shapeIndex));
	printf("\tN_POINTS     : %d \n", N_POINTS);
	
	int point, pointsInCrown, nAbove2m;
	float * zValues, zThreshold;
	double xValue, yValue, zValue;
	SHPObject * pShpObj;
	
	pointsInCrown = 0;
	pShpObj = SHPReadObject(H_SHP, shapeIndex);
	
	/**
	 * For each point determine if it lies within the current tree crown.
	 */
	for(point = 0; point < N_POINTS; point++)
	{
		xValue = CHM[0][point];
		yValue = CHM[1][point];
		zValue = CHM[2][point];
		
		if(pointInPolygon(pShpObj, xValue, yValue))
		{
			POINT_CLASSIFICATION[point] = 1;
			
			if(pointsInCrown == ARRAY_SIZE)
				if((Z_VALUES = resize(Z_VALUES, &ARRAY_SIZE)) == NULL)
					return 0;
			
			Z_VALUES[pointsInCrown++] = zValue;
		}
		else{
			POINT_CLASSIFICATION[point] = 0;
		}
	}
	
	printf("\tPoints in Crown: %d\n", pointsInCrown);
	
	if(pointsInCrown == 0){
		return 0;
	}
	 
	qsort(Z_VALUES, pointsInCrown, sizeof(float), floatCompare);
	
	//zValues -> all points > 2 meters
	zValues = pointsAbove2m(&nAbove2m, pointsInCrown);
	zThreshold = 
		computeTreeCrownHeight(zValues, nAbove2m, 0.5);
		
	if(zThreshold < 0)
		return 0;
	
	printf("\tHeight threshold: %f\n", zThreshold);
	if(!writeToFile(fpOut, POINT_CLASSIFICATION, CHM, &N_POINTS, zThreshold))
			return 0;

	SHPDestroyObject(pShpObj);
	
	return pointsInCrown;
}

static PyObject* Py_finally(PyObject* self, PyObject* args)
{	
	free(Z_VALUES);
	free(tciFlag.data);
	SHPClose(H_SHP);
	return Py_None;
}



/**
 * Allocates memory for data structures required for processing.
 * 
 * Initializes:
 * - tree crown index flag array
 * - point classification array
 * - z values array
 */
int setup()
{
	N_RECORDS = H_SHP->nRecords;
	printf("N_RECORDS: %d\n", N_RECORDS);
	
	tciFlag.size = N_RECORDS;
	if((tciFlag.data = allocc1d(tciFlag.size)) == NULL)
		return 0;
	
	if((Z_VALUES = allocf1d(ARRAY_SIZE)) == NULL) 
		return 0;
	
	return 1;
}



/**
 * Program start! 
 * Defines static variables constant through out processing
 * of canopy height models.
 * 
 * Initializes:
 * - shape file containing tree crowns
 * - minimum easting
 * - minimum norhting
 * - block size
 */
static PyObject* Py_setup(PyObject* self, PyObject* args)
{
	printf("SETUP.\n");
	
	char * shpFile;
	
	if(!PyArg_ParseTuple(args, "siddif", &shpFile, &BLOCK_SIZE, &MIN_E, &MIN_N, &OUT_RES, &Z_THRESH))
	{
		PyErr_SetString(PyExc_IOError, "Could not parse parameters.");
		return NULL;
	}

	
	H_SHP = SHPOpen(shpFile, "rb");
	if(!H_SHP)
	{
		PyErr_SetString(PyExc_IOError, "Could not open shape file.");
		return NULL;
	}
	
	if(!setup())
		return NULL;
	
	return Py_BuildValue("i", N_RECORDS);
}


/*
 * Bind Python function names to C functions
 */
static PyMethodDef TCR_methods[] = {
	{"setup", Py_setup, METH_VARARGS},
	{"finish", Py_finally, METH_VARARGS},
	{"execute", Py_execute, METH_VARARGS},
	{NULL, NULL}
};

/*
 * Python calls this to let us initialize our module
 */
void initTCR_module(void)
{
	(void) Py_InitModule("TCR_module", TCR_methods);
}


