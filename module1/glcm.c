#include "TM_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float calculateGlcmMean(float ** glcm, int);
float calculateGlcmVariance(float ** glcm, int);
float calculateGlcmEntropy(float ** glcm, int);
float calculateGlcmAngular2ndMoment(float ** glcm, int);
float calculateGlcmHomogeneity(float ** glcm, int);
float calculateGlcmContrast(float ** glcm, int);
float calculateGlcmDissimilarity(float ** glcm, int);
float calculateGlcmCorrelation(float ** glcm, int);

#define ROW_OFFSET 0
#define COL_OFFSET 1

extern int IMAGE_WIDTH;
extern int IMAGE_LENGTH;
extern int NUMBER_OF_ROWS;
extern int NUMBER_OF_COLS;
extern int MAX_THRESH;
extern int MIN_THRESH;


/**
 * The number of measure functions.
 */
static int NUM_MEASURES = 10;
//static int MIN_THRESH = -1391;
//static int MAX_THRESH = 29035;

/**
 * Names of the measures calculated.
 */
char * MEASURE_NAMES [] = {
	"mean", 
	"variance", 
	"entropy", 
	"angular_second_moment", 
	"homogeneity", 
	"contrast", 
	"dissimilarity",
	"correlation",
	"image_mean",
	"image_standard_deviation"
};

/**
 * Pointers to GLCM measure functions.
 */
float (*functions[]) (float **, int) = {
		calculateGlcmMean,
		calculateGlcmVariance,
		calculateGlcmEntropy,
		calculateGlcmAngular2ndMoment,
		calculateGlcmHomogeneity,
		calculateGlcmContrast,
		calculateGlcmDissimilarity,
		calculateGlcmCorrelation
};

int getNumMeasures()
{
	return NUM_MEASURES;
}

char ** getMeasureNames()
{
	return MEASURE_NAMES;
}

void freeIntBuffer2D(int ** buffer, int numRows)
{
	int row;
	for(row = 0; row < numRows; row++)
	{
		free(buffer[row]);
	}
	free(buffer);
}

/**
 * Returns a GLCM of a portion of 'image' of size windowSize x windowSize defined by
 * minRow, maxRow, minCol, and maxCol.
 */
float *** graycomatrix(
	float ** image,
	float *** glcm,
	int numLevels, 
	int offsets [][2], 
	int numOffsets, 				
	int minRow, 
	int maxRow, 
	int minCol, 
	int maxCol,
	int windowSize
	)
{
	//printf("Number of offsets : %d %d %d %d %d %d %d \n", numLevels, offsets[0][0], offsets[0][1], minRow, maxRow, minCol, maxCol);
	
	double sum;
	float min, max, temp, range, scaleFactor, minThresh, maxThresh;
	int offset, rowMax, colMax, rIndex, cIndex, tempi, row, col;
	
	maxThresh = 29035; // mt. everest (ft) - protect against bad data
	minThresh = -1391; // dead sea (ft) - protect against bad data
	
	
	/**
	 * - Determine maximum and minimum values within [minThresh, maxThresh].
	 */
	 //puts("\tDeterming max and min values");
	 if(maxCol > NUMBER_OF_COLS || maxRow > NUMBER_OF_ROWS || minRow < 0 || minCol < 0) 
		 printf("minRow:%d minCol:%d maxRow:%d maxCol:%dn", maxRow, maxCol);
	
	 min = maxThresh;
	 max = minThresh;
	 
	 for(row = minRow; row < maxRow; row++){
		for(col = minCol; col < maxCol; col++)
		{
			temp = image[row][col];
			
			if(temp > maxThresh || temp < minThresh)
				continue;
			
			if(temp > max) max = temp;
			if(temp < min) min = temp;
		}
	}
	
	/**
	 * - Determine the range and scale factor.
	 */
	if((range = max - min) == 0.0)
		scaleFactor = 0.0;
	else
		scaleFactor = (numLevels - 1) / range;
	
	/**
	 * - Copy image -> in
	 * - Filter extreme values
	 * - Translate and scale values to lie with [0, numLevels-1]
	 *
	 * TODO: review filtering method.
	 */ 
	int ** in = (int **) malloc(sizeof(int *) * windowSize);
	for(row = minRow; row < maxRow; row++)
	{
		in[row-minRow] = (int *) malloc(sizeof(int) * windowSize);
		
		for(col = minCol; col < maxCol; col++)
		{
			temp = image[row][col];
			
			if(row - minRow < 0 || row - minRow >= windowSize || col - minCol < 0 || col - minCol >= windowSize)
				printf("Error will occur\n");
			
			if(temp < minThresh)
				in[row-minRow][col-minCol] = minThresh - 1; //no data
			else if(temp > maxThresh)
				in[row-minRow][col-minCol] = maxThresh + 1; //no data
			else			
				in[row-minRow][col-minCol] = (int)roundf((temp - min) * scaleFactor);
		}
	}
	
	/**
	 * - Compute GLCM from the adjusted window only considering values within 
	 *   [minThresh, maxThresh].
	 * - NOTE: replaced numOffsets with 1
	**/
	for(offset = 0; offset < 1 /*numOffsets*/; offset++)
	{	
		rowMax = windowSize - offsets[offset][ROW_OFFSET];
		colMax = windowSize - offsets[offset][COL_OFFSET];
		
		for(row = 0; row < rowMax; row++){
			for(col = 0; col < colMax; col++)
			{
				rIndex = in[row][col];
				cIndex = in[ (row+offsets[offset][ROW_OFFSET]) ][ (col+offsets[offset][COL_OFFSET]) ];
				
				if(rIndex < minThresh || rIndex > maxThresh || cIndex < minThresh || cIndex > maxThresh)
					continue;
				if(rIndex >= numLevels || cIndex >= numLevels){
					printf("rIndex:%d cIndex:%d numLevs:%d\n", rIndex, cIndex, numLevels);
					continue;
				}
					
				
				glcm[offset][rIndex][cIndex]++;
			}
		}
	}
	freeIntBuffer2D(in, windowSize);
	/**
	 * - Add the transpose to make symmetric.
	 * - NOTE: glcm matrix is square
	**/
	for(offset = 0; offset < numOffsets; offset++)
	{
		for(row = 0; row < numLevels; row++){
			for(col = 0; col <= row; col++)
			{
				if(col == row)
					glcm[offset][row][col] += glcm[offset][row][col];
				else{
					tempi = glcm[offset][row][col];
					glcm[offset][row][col] += glcm[offset][col][row];
					glcm[offset][col][row] += tempi;
				}
			}
		}
	}
	
	/**
	 * Normalize the values in the glcm matrices so that each cell represents
	 * a percentage of the corresponding spatial relationship.
	 */
	//puts("\tnormalize");
	for(offset = 0; offset < numOffsets; offset++)
	{
		sum = 0.0;
		
		//find the sum of each glcm matrix
		for(row = 0; row < numLevels; row++){
			for(col = 0; col < numLevels; col++)
			{
				sum += glcm[offset][row][col];
			}
		}
		
		if(sum == 0)
			return glcm;
		
		//normalize each glcm matrix
		for(row = 0; row < numLevels; row++){
			for(col = 0; col < numLevels; col++)
			{
				glcm[offset][row][col] /= sum;
			}
		}
	}
	
	return glcm;
}

float calculateGlcmMean(float ** glcm, int numLevels)
{
	int row, col;
	float result;
	
	result = 0.0;
	
	for(row = 0; row < numLevels; row++){
		for(col = 0; col < numLevels; col++)
		{
			result += row * glcm[row][col];			
		}
	}
	
	return result;
}

float calculateGlcmVariance(float ** glcm, int numLevels)
{
	int row, col;
	float result, mean;
	
	result  = 0.0;
	mean    = calculateGlcmMean(glcm, numLevels);
	
	for(row = 0; row < numLevels; row++){
		for(col = 0; col < numLevels; col++)
		{
			result += glcm[row][col] * (row - mean) * (row - mean);			
		}
	}
	
	return result;
}

float calculateGlcmEntropy(float ** glcm, int numLevels)
{
	int row, col;
	float result, temp;
	
	result = 0.0;
	
	for(row = 0; row < numLevels; row++){
		for(col = 0; col < numLevels; col++)
		{
			if((temp = glcm[row][col]) == 0.0)
				continue;
			
			result += (-1.0) * temp * log(temp);			
		}
	}
	
	return result;
}

float calculateGlcmAngular2ndMoment(float ** glcm, int numLevels)
{
	int row, col;
	float result;
	
	result = 0.0;
	
	for(row = 0; row < numLevels; row++){
		for(col = 0; col < numLevels; col++)
		{
			result += ( glcm[row][col] * glcm[row][col] );
		}
	}
	
	return result;
}

float calculateGlcmHomogeneity(float ** glcm, int numLevels)
{
	int row, col;
	float result;
	
	result = 0.0;
	
	for(row = 0; row < numLevels; row++){
		for(col = 0; col < numLevels; col++)
		{
			result += ( (glcm[row][col]) / (1.0 + (row - col) * (row-col)) );
		}
	}
	
	return result;
}

float calculateGlcmContrast(float ** glcm, int numLevels)
{
	int row, col;
	float result;
	
	result = 0.0;
	
	for(row = 0; row < numLevels; row++){
		for(col = 0; col < numLevels; col++)
		{
			result += ( glcm[row][col] * (row - col) * (row-col) );
		}
	}
	
	return result;
}

float calculateGlcmDissimilarity(float ** glcm, int numLevels)
{
	int row, col;
	float result;
	
	result = 0.0;
	
	for(row = 0; row < numLevels; row++){
		for(col = 0; col < numLevels; col++)
		{
			result += ( glcm[row][col] * fabs(row - col) );
		}
	}
	
	return result;
}

float calculateGlcmCorrelation(float ** glcm, int numLevels)
{
	int row, col;
	float result, mean, variance;
	
	// this is inefficient -- mean is getting calculated twice
	mean     = calculateGlcmMean(glcm, numLevels);
	variance = calculateGlcmVariance(glcm, numLevels);
	
	if(variance == 0.0)
		return 1.0;
	
	result   = 0.0;
	
	for(row = 0; row < numLevels; row++){
		for(col = 0; col < numLevels; col++)
		{
			result += ((row - mean) * (row - mean)) / (variance);			
		}
	}
	
	return result;
}

float calculateWindowMean(float ** raster, int minRow, int minCol, int window)
{
	int row, col, N;
	float sum = 0.0;
	
	N = 0;
	
	for(row = minRow; row < minRow + window; row++)
	for(col = minCol; col < minCol + window; col++)
	{
		//TDOD: check this is correct
		if(raster[row][col] > MIN_THRESH && raster[row][col] < MAX_THRESH)
		{
			sum += raster[row][col];
			++N;
		}
	}
	
	if(N == 0)
		return 0;
	return sum / N;
}

float calculateWindowStandardDeviation(float ** raster, int minRow, int minCol, int window)
{
	float mean, sum;
	int row, col, N;
	
	mean = calculateWindowMean(raster, minRow, minCol, window);
	sum  = 0;
	N    = 0;
	
	for(row = minRow; row < minRow + window; row++)
	for(col = minCol; col < minCol + window; col++)
	{
		//TODO: verify this should be done
		if(raster[row][col] > MIN_THRESH && raster[row][col] < MAX_THRESH)
		{
			sum += pow( raster[row][col] - mean, 2 );
			++N;
		}
	}
	
	if(N == 0)
		return 0;
	return pow(sum / N, 0.5);
}


/**
 * Control loop that call graycomatrix for each windowSize x windowSize portion
 * of 'buffer'.
 */
int calculateGLCM(float ** inputRaster, float *** out, int numLevels, int offsets [][2], int numOffsets, int windowSize)
{
	int row, col, measure;
	float *** GLCM;
	
	for(row = 0; row + windowSize <= NUMBER_OF_ROWS; row += windowSize){
		for(col = 0; col + windowSize <= NUMBER_OF_COLS; col += windowSize)
		{
			if((GLCM = initBuffer3D(1, numLevels, numLevels, 0.0)) == NULL)	return 0;
			
			graycomatrix(
				inputRaster,
				GLCM,
				numLevels,
				offsets,
				numOffsets,
				row,
				row + windowSize,
				col,
				col + windowSize,
				windowSize
			);
						
			for(measure = 0; measure < NUM_MEASURES-2; measure++)
			{
				out[measure][row/windowSize][col/windowSize] = (*functions[measure]) (GLCM[0], numLevels);
			}
			freeBuffer3D(GLCM, 1, numLevels);
			
			//caluclate inputRaster mean and standard deviation
			out[measure][row/windowSize][col/windowSize] =  calculateWindowMean (inputRaster, row, col, windowSize);
			measure++;
			out[measure][row/windowSize][col/windowSize] =  calculateWindowStandardDeviation (inputRaster, row, col, windowSize);
		}
	}
	return 1;
}


/**********

//write glcm matrix to file
	//TODO: adjust this to work for multiple spacial relationships
	TIFF * out;
	if((out = TIFFOpen(outFile, "w")) == NULL)return;
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, 		numLevels);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, 		numLevels);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 	32);
	TIFFSetField(out, TIFFTAG_COMPRESSION, 		COMPRESSION_NONE);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
	TIFFSetField(out, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
	TIFFSetField(out, TIFFTAG_DATATYPE, 		3);
	for(row = 0; row < numLevels; row++)
		TIFFWriteScanline(out, glcm[0][row], row, 0);
	TIFFClose(out);
	
	/*
	 * Free the glcm matrix.
	 *
	 for(offset = 0; offset < numOffsets; offset++){
	 	 for(row = 0; row < numLevels; row++){
	 	 	 free(glcm[offset][row]);
	 	 }
	 	 free(glcm[offset]);
	 }
	 free(glcm);
	 
******************************/
