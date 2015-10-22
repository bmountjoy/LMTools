


#include "TCR_module.h"

/**
 * Returns all point within a pixel of the ouput image that are above 'zThreshold'.
 */
float * filter(
	double ** points,
	int       numPoints,
	int 	  zThreshold,
	double    xStart,
	double    yStart,
	double    xEnd,
	double    yEnd,
	int		* size	
	)
{
	float * zValues;
	int col, ii, count;
	
	count = 0;
	
	for(col = 0; col < numPoints; col++)
	{
		double x = points[0][col];
		double y = points[1][col];
		double z = points[2][col];
		
		if( (x >= xStart) && (x < xEnd) && (y >= yStart) && (y < yEnd) && (z > zThreshold))
		{
			++count;
		}
	}
	
	zValues = (float *)malloc(sizeof(float) * count);
	ii = 0;
	for(col = 0; col < numPoints; col++)
	{
		double x = points[0][col];
		double y = points[1][col];
		double z = points[2][col];
		
		if( (x >= xStart) && (x < xEnd) && (y >= yStart) && (y < yEnd) && (z > zThreshold))
		{
			zValues[ii++] = (float)z;
		}
	}
	
	*size = count;
	
	return zValues;
}


float *** initGriddedZBuffer(int dimensions)
{
	float *** zBuf;
	if( (zBuf = (float ***)malloc(sizeof(float **) * dimensions)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	int i;	
	for(i = 0; i < dimensions; i++)
	{	
		if( (zBuf[i] = (float **)malloc(sizeof(float *) * dimensions)) == NULL)
		{
			PyErr_SetString(PyExc_MemoryError, "malloc failed.");
			return NULL;
		}
	}
	
	return zBuf;
}

int db_floatCompare(const void * a, const void * b)
{
	float result = *(float*)a - *(float*)b;
	if(result > 0)return 1;
	else if(result == 0)return 0;
	else return -1;
}


/**
 * Count the number of points in each pixel of the ouput tiff.
 */
int ** countPoints (double ** chmArr, int nPoints, double minEasting, double minNorthing, int blockSize, int outRes,
	int xpos, int ypos)
{
	int i, j, x, y, dim, **numA;
	double blkE, blkN;
	
	dim  = blockSize/outRes;
	blkE = minEasting  + (xpos * blockSize);
	blkN = minNorthing + (ypos * blockSize);
	
	if((numA  = alloci2d(dim, dim)) == NULL) return NULL;
	
	for(i = 0; i < dim; i++)
		for(j = 0; j < dim; j++)
			numA[i][j] = 0;

	for(i = 0; i < nPoints; i++)
	{	
		x = (int)((chmArr[0][i]-blkE)/outRes);
		y = (int)((chmArr[1][i]-blkN)/outRes);
		
		if(x < 0 || y < 0 || x >= dim || y >= dim)
		{	
			continue;
		}
 		//numA[x][y]++;
 		numA[y][x]++;
	}

	return numA;
}


/**
 * Group the points, from input CHM, belonging to each pixel in output tiff that are above
 * the specified z threshold.
 */
void gridChm (double ** chmArr, int nPoints, float ***gzVals, int **numZ,
	double blkE, double blkN, int dim, int outRes, int zThreshold)
{
	int i, j;
	double xStart, yStart, xEnd, yEnd;
	
	for(j = 0; j < dim; j++)
	{
		for(i = 0; i < dim; i++)
		{
			//TODO: remove the multiplication
			xStart = blkE  + (outRes * i);
			yStart = blkN + (outRes * j);
			xEnd   = xStart + outRes;
			yEnd   = yStart + outRes;
			
			gzVals[j][i] = filter(chmArr, nPoints, zThreshold, xStart, yStart, xEnd, yEnd, &numZ[j][i]);
			
			qsort(gzVals[j][i], numZ[j][i], sizeof(float), db_floatCompare);
		}
	}
}

int getCCFCount(float * zValues, int numValues, float threshold)
{
	int count, i;
	
	count = 0;

	for(i = 0; i < numValues; i++)
		if(zValues[i] > threshold) 
			++count;
	
	return count;
}

int computeCCF(double **CHM, int nPoints, int **numA, 
	double minEasting, double minNorthing, int blockSize, int outRes, float zThreshold,
	int xblkpos, int yblkpos, char * filePath)
{
	int i, j, k, dim, band, ccfCount, **numZ;
	float curHeight, ccfPercent, htIncr, ***ccfArr, ***gzVals;
	double blkEasting, blkNorthing;
	
	dim = blockSize/outRes;
	
	if( (ccfArr = allocf3d(21, dim, dim)) == NULL )return 0;
	if( (numZ   = alloci2d(dim, dim)) == NULL ) return 0;
	if( (gzVals = initGriddedZBuffer(dim)) == NULL ) return 0;
	
	for(i = 0; i < 21; i++)
		for(j = 0; j < dim; j++)
			for(k = 0; k < dim; k++)
				ccfArr[i][j][k] = -999;
			
	blkEasting = minEasting + (blockSize * xblkpos);
	blkNorthing = minNorthing + (blockSize * yblkpos);
	
	/**
	 * Grid the z values from the CHM.
	 */
	gridChm(CHM, nPoints, gzVals, numZ, blkEasting, blkNorthing, dim, outRes, zThreshold);
	
	
	/**
	 * Compute the CCF from the gridded z values.
	 */
	for(j = 0; j < dim; j++)
	{
		for(i = 0; i < dim; i++)
		{
			float * zVals = gzVals[j][i];
			int     nz    = numZ[j][i];
			int     na    = numA[j][i];
			
			if(nz < 75)
			{
				continue;
			}
			
			htIncr    = (zVals[nz-1] - zThreshold) / 20.0;
			curHeight = zThreshold;
			
			for(band = 0; band <= 20; band++)
			{
				ccfCount   = getCCFCount(zVals, nz, curHeight);
				ccfPercent = ccfCount / (float) na;
				
				ccfArr[band][j][i] = ccfPercent;
				
				curHeight += htIncr;
			}
		}
	}
	
	writeTiff
	(
		ccfArr, 
		filePath, 
		21, 
		dim, 
		outRes, 
		blkEasting, 
		blkNorthing + blockSize	
	);
	
	freef3d(ccfArr, 21, dim);
	freei2d(numZ, dim);
	freef3d(gzVals, dim, dim);
	freei2d(numA, dim);
	
	return 1;
}

