
#include <Python/Python.h>

#include <stdlib.h>
#include <math.h>

#include "Bio_LMoments.h"
#include "Bio_biometrics.h"
#include "Memory.h"

void computeLhq(float * zValues, int numZ, float * lhq)
{
	printf("computeLhq : %d %f %f\n", numZ, zValues[0], zValues[numZ-1]);
	
	if(numZ < 75) return;
	
	float rank, diff;
	int   base, band, rank1, rank2;
	
	lhq[0] = zValues[0]; //boundary case
	
	for(band = 1; band < 20; band++)
	{
		rank  = (band/20.0) * (numZ+1);
		base  = (int)floor(rank);
		diff  = rank - base;
		
		rank1 = base-1; if(rank1 < 0) rank1 = 0;
		rank2 = base;
		
		lhq[band] = ((1.0 - diff) * zValues[rank1]) + (diff * zValues[rank2]);
	}
	lhq[20] = zValues[numZ-1]; //boundary case
}

void computeCcf(float * zValues, int numZ, int numAll, int zThreshold, float * ccf)
{
	printf("computeCcf : %d %d %f %f \n", numZ, numAll, zValues[0], zValues[numZ-1]);
	
	if(numZ < 75) return;
	
	int band, ccfCount;
	float curHeight, ccfPercent, htIncrement;
	
	htIncrement = (zValues[numZ-1] - zThreshold) / 20.0; printf("htIncrement %f\n", htIncrement);
	curHeight   = zThreshold;
	
	for(band = 0; band <= 20; band++)
	{
		ccfCount   = getCCFCount(zValues, numZ, curHeight);
		ccfPercent = ccfCount / (float) numAll;
		
		ccf[band] = ccfPercent;
		
		curHeight += htIncrement;
	}
}

//at some point the zvalues need to be sorted
float * computeBiometrics(float * zValues, int numz, int numAll, int zThreshold)
{
	
	qsort(zValues, numz, sizeof(float), floatCompare);
	
	printf("computeBiometrics:%d,%d,%f,%f\n", numAll, numz, zValues[0], zValues[numz-1]);
	
	float * bio, *lhq, *ccf;
	bio = initFloatBuffer1D(50, -999.0);
	if(bio == NULL)
	{
		return NULL;
	}
	
	if(numz <= zThreshold)
	{
		return bio;
	}
	
	lhq = &bio[8];
	ccf = &bio[29];
	
	computeRugosity(zValues, numz, &bio[0]);
	computeGap(numz, numAll, &bio[1]);
	computePercentile(zValues, numz, 0.85, &bio[2]);
	
	if(numz <= 75)
	{
		return bio;
	}
	
	float * lmoments = 
	computeLMoments(zValues, numz);
	bio[3] = lmoments[0];
	bio[4] = lmoments[1];
	bio[5] = lmoments[2];
	bio[6] = lmoments[3];
	free(lmoments);
	
	computeLhq(zValues, numz, lhq);
	computeCcf(zValues, numz, numAll, zThreshold, ccf);
	
	bio[7] = lhq[17];

	return bio;
}

//TODO : centralize
int floatCompare(const void * a, const void * b)
{
	float result = *(float*)a - *(float*)b;
	
	if(result > 0)
	{
		return 1;
	}
	else if(result == 0)
	{
		return 0;
	}	
	else{			
		return -1;
	}
}

double getDoubleMax(double * array, int size)
{
	double max;
	int i;
	
	max = array[0];
	
	for(i = 1; i < size; i++)
	{
		if(array[i] > max){
			max = array[i];
		}
	}
	
	return max;
} 

double getDoubleMin(double * array, int size)
{
	double min;
	int i;
	
	min = array[0];
	
	for(i = 1; i < size; i++)
	{
		if(array[i] < min){
			min = array[i];
		}
	}
	
	return min;
}
	
float getMax(float * array, int size)
{
	float max;
	int i;
	
	max = array[0];
	
	for(i = 1; i < size; i++)
	{
		if(array[i] > max){
			max = array[i];
		}
	}
	
	return max;
}

/**
 * Return the number of z values greater than the threshold.
 */
int getCCFCount(float * zValues, int numValues, float threshold)
{
	int count, i;
	
	count = 0;
	
	for(i = 0; i < numValues; i++)
	{
		if(zValues[i] > threshold) 
			++count;
	}
	
	return count;
}

float calculateMean1D(float * array, int size)
{
	if(size <= 0)
	{		
		return -999.0;
	}
	
	float sum = 0.0;
	int i;
	
	for(i = 0; i < size; i++)
	{
		sum += array[i];
	}
	
	return sum / size;
}

float calculateVariance1D(float * array, int size)
{
	float mean, diff_sum = 0.0;
	int i;
	
	mean = calculateMean1D(array, size);
	
	for(i = 0; i < size; i++)
	{
		diff_sum += pow( array[i] - mean, 2);
	}
	
	return diff_sum / (size - 1);
}

float calculateRugosity1D(float * array, int size)
{
	return pow( calculateVariance1D(array, size), 0.5 );
}
	

Array1D filterByZThreshold(
	
	float * array,
	int     numPoints,
	float   zThresh
	
	)
{
	float * zValues, z;
	int i, ii, count;
	
	count = 0;
	
	for(i = 0; i < numPoints; i++)
		if( array[i] > zThresh ) 
			++count;
	
	zValues = (float *)malloc(sizeof(float) * count);
	
	ii = 0;
	
	for(i = 0; i < numPoints; i++)
	{
		z = array[i];
		
		if( z > zThresh )
		{
			zValues[ii++] = z;
		}
	}
	
	Array1D  A;
	
	A.points = zValues;
	A.size   = count;
	
	return A;
}

Array1D filterValuesLess(
	
	float * array,
	int     numPoints,
	float   threshold
	
	)
{
	float * zValues, z;
	int i, ii, count;
	
	count = 0;
	
	for(i = 0; i < numPoints; i++)
		if( array[i] >= threshold ) 
			++count;
	
	zValues = (float *)malloc(sizeof(float) * count);
	
	ii = 0;
	
	for(i = 0; i < numPoints; i++)
	{
		z = array[i];
		
		if( z >= threshold )
		{
			zValues[ii++] = z;
		}
	}
	
	Array1D  A;
	
	A.points = zValues;
	A.size   = count;
	
	return A;
}

/*
 * Faster than previous function
 */
void gridPoints(
	
	int dimensions,
	int resolution,
	double blkEasting,
	double blkNorthing,
	double ** points,
	int numPoints, 
	int ** num_z,
	int ** num_all,
	float *** z_values,
	float *** z_all,
	float threshold,
	float zmax
	)
{
	int i,j;
	int cur_z [dimensions][dimensions];
	int cur_a [dimensions][dimensions];
	
	for(i = 0; i < dimensions; i++){
	for(j = 0; j < dimensions; j++){
		cur_z[i][j] = 0;
		cur_a[i][j] = 0;
	}}
	
	//determine how many points belong to each pixel
	for(i = 0; i < numPoints; i++)
	{
		double x = points[0][i];
		double y = points[1][i];
		double z = points[2][i];
		int row = (int)((y-blkNorthing)/resolution);
		int col = (int)((x-blkEasting)/resolution);
		
		if(row < 0 || row >= dimensions || col < 0 || col >= dimensions || z > zmax)
		{
			//printf("BAD POINT --> row:%d, col:%d\n, height:%lf", row, col, z);
			continue;
		}
		/*
		else if(row >=5 && row <= 15){
			printf("target row : %d\n", row);
		}
		*/
		num_all[row][col]++;
		if(z >= threshold) //add aditional
			num_z[row][col]++;
	}
	
	//initialize buffer to hold points above zthreshold
	for(i = 0; i < dimensions; i++){
		for(j = 0; j < dimensions; j++)
		{
			z_values[i][j] = (float*)malloc(sizeof(float) * num_z[i][j]);
			z_all[i][j] = (float*)malloc(sizeof(float) * num_all[i][j]);
		}
	}
	
	//buffer the points
	for(i = 0; i < numPoints; i++)
	{
		double x = points[0][i];
		double y = points[1][i];
		double z = points[2][i];
		int row = (int)((y-blkNorthing)/resolution);
		int col = (int)((x-blkEasting)/resolution);
		
		if(row < 0 || row >= dimensions || col < 0 || col >= dimensions || z > zmax)
		{
			continue;
		}
		
		z_all[row][col][cur_a[row][col]++] = (float)z;
		if(z >= threshold){
			z_values[row][col][cur_z[row][col]++] = (float)z;
		}
	}
	
	//sort the z values in increasing order
	for(i = 0; i < dimensions; i++){
		for(j = 0; j < dimensions; j++)
		{
			qsort(z_values[i][j], num_z[i][j], sizeof(float), floatCompare);
			qsort(z_all[i][j],    num_all[i][j], sizeof(float), floatCompare);
		}
	}
}

/**
 * Return and array of points whos x and y  coords fit withtin the specified bounds.
 */
Array1D filterByArea(
	
	double ** points,
	int       numPoints, 
	double    xStart,
	double    yStart,
	double    xEnd,
	double    yEnd
	
	)
{
	//printf("%d %lf %lf %lf %lf \n", numPoints, xStart, yStart, xEnd, yEnd);
	//printf("numPoints %d ", numPoints);
	//printf("%lf %lf %lf \n", points[0][0], points[1][0], points[2][0]);
	
	float * zValues;
	int col, ii, count;
	
	count = 0;
	
	for(col = 0; col < numPoints; col++)
	{
		double x = points[0][col];
		double y = points[1][col];
		
		if( (x >= xStart) && (x < xEnd) && (y >= yStart) && (y < yEnd) )
		{
			++count;
		}
	}
	
	//printf("area count : %d \n", count);
	
	zValues = (float *)malloc(sizeof(float) * count);
	
	ii = 0;
	
	for(col = 0; col < numPoints; col++)
	{
		double x = points[0][col];
		double y = points[1][col];
		
		if( x >= xStart && x < xEnd && y >= yStart && y < yEnd )
		{
			zValues[ii++] = (float)points[2][col];
		}
	}
	
	Array1D A;
	
	A.points = zValues;
	A.size   = count;
	
	return A;// returning local memory .. could be an issue	
}


void computeGap(int numThresh, int numAll, float * result)
{
	*result = 1 - ((float)numThresh / (float)numAll);
}

void computeRugosity(float * buf, int size, float * result)
{
	*result = calculateRugosity1D(buf, size);
}

void computePercentile(float * zValues, int numVals, float percentile, float * result)
{
	Array1D B;
	float pValue;
	
	pValue     = zValues[ (int)(numVals * percentile) ];	 // this probably isn't how the 85th percentile should be found
	B          = filterValuesLess(zValues, numVals, pValue);
	*result    = calculateMean1D(B.points, B.size);
}
