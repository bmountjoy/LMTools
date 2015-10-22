
#ifndef TREETOPS_H
#define TREETOPS_H


#include <Python/Python.h>
#include <stdlib.h>
#include <stdio.h>

//libtiff
#include <tiffio.h>

//libgeotiff
#include "geotiff.h"
#include "xtiffio.h"

/**
 * Declarations for io.c
 */
double ** buffFile (FILE * fp, int * nPtsPtr);
int writeFile (FILE * fp, double ** A, int nCols);
int writeToFile(FILE * fp, char * pc, double ** P, int * pNPoints, float hThreshold);
int writeTiff(float *** out, char * filePath, int numBands, int numDims, int outRes, double xTiePoint, double yTiePoint);

/**
 * Declarations for memory.c
 */
char * allocc1d(int n);
int * alloci1d(int n);
float * allocf1d(int n);
int ** alloci2d(int nRows, int nCols);
double ** allocd2d(int nRows, int nCols);
float *** allocf3d(int x, int y, int z);
void freei2d(int ** buffer, int numRows);
void freed2d(double ** buffer, int numRows);
void freef3d(float *** buffer, int x, int y);

/**
 * Declarations for quantize.c
 */
float computeTreeCrownHeight(float * zValues, int nPoints, float bucketSize);

/*
 * diff_bio.c
 */
int computeCCF(double **CHM, int nPoints, int **numA, 
	double minEasting, double minNorthing, int blockSize, int outRes, float zThreshold,
	int xblkpos, int yblkpos, char * filePath);
int ** countPoints (double ** chmArr, int nPoints, double minEasting, double minNorthing, int blockSize, int outRes,
	int xpos, int ypos);

#endif


