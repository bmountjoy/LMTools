
#include <Python/Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bio_IO.h"

#define ModelPixelScaleTag	33550	//geotiff tag id
#define ModelTiepointTag	33922	//geotiff tag id

const char * DELIM = "._abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

double ** bufferFile (FILE * fp, int * numPoints)
{
	double x, y, z;
	int numLines, line;
	
	numLines = 0;
	
	while(fscanf(fp, " %lf %lf %lf", &x, &y, &z) == 3)
	{
		++numLines;
	}
	
	*numPoints = numLines;
	fseek(fp, 0, SEEK_SET);
	
	double ** points;
	if((points = initDoubleBuffer2D(3, numLines, 0.0)) == NULL){
		return NULL;
	}
	
	line = 0;
	
	while(fscanf(fp, "%lf %lf %lf", &x, &y, &z) == 3)
	{
		points[0][line] = x;
		points[1][line] = y;
		points[2][line] = z;
		++line;
	}
	
	return points;
}

// TODO : add error handling
int extractBlockPosition (char * filePath, int * blkPos [2])
{
	char * cPtr = 
	strrchr(filePath, '/') + 1;
	
	char fileName [strlen(cPtr)+1];
	
	strcpy(fileName, cPtr);
	
	int index;
	
	index = 0;
	cPtr  = strtok(fileName, DELIM);
	
	while(cPtr != NULL){
		*blkPos[index++] = atoi(cPtr);
		cPtr = strtok(NULL, DELIM);
	}
	
	if(index != 2) {
		PyErr_SetString(PyExc_IOError, "Input file format error.");
		return 0;
	}
	
	return 1;
}

double ** readFile(char * filePath, int * numPoints, int * xPos, int * yPos)
{
	FILE * fp;
	
	fp = fopen(filePath, "r");
	if (!fp) 
	{
		PyErr_SetString(PyExc_IOError, "Could not open input file.");
		return NULL;
	}
	
	double ** points;
	if((points = bufferFile(fp, numPoints)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "Could not allocate memory for points.");
		return NULL;
	}
	
	fclose(fp);
	
	int * blkPos [2] = {xPos, yPos};
	if(!extractBlockPosition(filePath, blkPos))
	{
		return NULL;
	}
	
	return points;
}


/**
 * out - buffer to be written
 * filePath - tiff to write to
 * numBands - number of bands
 * numDims - image width and height
 * outRes - size of each pixel
 * xTiePoint - x coordinate of pixel (0,0)
 * yTiePoint - y coordinate of pixel (0,0)
 */
int writeTiff(float *** out, char * filePath, int numBands, int numDims, int outRes, double xTiePoint, double yTiePoint)
{
	printf("Writing TIFF to file. \n");
	GTIF * outGtif;
	TIFF * outFile;
	
	if( (outFile = XTIFFOpen(filePath, "w")) == NULL )
	{
		PyErr_SetString(PyExc_IOError, "Could not open output TIFF file.");
		return 0;
	}
	if( (outGtif = GTIFNew(outFile)) == NULL )
	{
		PyErr_SetString(PyExc_IOError, "GTIFNew failed in function 'writeTIFF'.");
		return 0;
	}
	
	TIFFSetField(outFile, TIFFTAG_IMAGEWIDTH, 		numDims);
	TIFFSetField(outFile, TIFFTAG_IMAGELENGTH, 		numDims);
	TIFFSetField(outFile, TIFFTAG_BITSPERSAMPLE, 	32);
	TIFFSetField(outFile, TIFFTAG_SAMPLESPERPIXEL, 	numBands);
	TIFFSetField(outFile, TIFFTAG_COMPRESSION, 		COMPRESSION_NONE);
	TIFFSetField(outFile, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
	TIFFSetField(outFile, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
	TIFFSetField(outFile, TIFFTAG_DATATYPE, 		3);
	TIFFSetField(outFile, TIFFTAG_PLANARCONFIG, 	PLANARCONFIG_SEPARATE);
	TIFFSetField(outFile, TIFFTAG_ORIENTATION, 		ORIENTATION_TOPLEFT);
	
	double mps [3] = {outRes, outRes, 1.0};
	double mtt [6] = {0.0, 0.0, 0.0, xTiePoint, yTiePoint, 0.0};
	
	TIFFSetField(outFile,	ModelPixelScaleTag, 3, mps);
	TIFFSetField(outFile,	ModelTiepointTag,	6, mtt);
	
	//still missing some geo tag info but will see if pixel scale and tiepoints will be enough
	
	int band, row, imgRow;
	
	for(band = 0; band < numBands; band++)
	{
		imgRow = 0;
		
		for(row = numDims-1; row >= 0; row--)
		{
			if(TIFFWriteScanline(outFile, out[band][row], imgRow++, band) == -1)
			{
				PyErr_SetString(PyExc_IOError, "Error writing scanline to file.");
				return 0;
			}
		}
	}
	
	GTIFFree(outGtif);
	TIFFClose(outFile);
	
	return 1;
}
