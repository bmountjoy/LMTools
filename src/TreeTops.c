

#include <Python/Python.h>
#include "TCD_module.h"

#include <stdlib.h>	
#include <stdio.h>

void copy(float **from, float **to, int imageLength, int imageWidth)
{
	int i, j;
	for(i = 0; i < imageLength; i++)
	for(j = 0; j < imageWidth; j++)
		to[i][j] = from[i][j];
}

/**
 * - copies interior rows/cols to edge
 * - leaves corners unchanged!!
 */
void extrapolateEdges(float **buff, int length, int width)
{
	int i, j;
	
	//copy cols
	for(i = 2; i < length-2; i++)
	{
		buff[i][1] = buff[i][3];
		buff[i][0] = buff[i][4];
		buff[i][width-2] = buff[i][width-4];
		buff[i][width-1] = buff[i][width-5];
	}
	
	//copy rows
	for(j = 2; j < width-2; j++)
	{
		buff[1][j] = buff[3][j];
		buff[0][j] = buff[4][j];
		buff[length-2][j] = buff[length-4][j];
		buff[length-1][j] = buff[length-5][j];
	}
}

/**
 * Applies a 2-dimensional boxcare average of width 3 to 'buff' and
 * saves the result in 'smoothed'.
 */
void smooth(float **buff, float **smoothed, int d1, int d2)
{
	int i, j, ii, jj;
	float sum;
	
	for(i = 1; i < d1-1; i++){
		for(j = 1; j < d2-1; j++)
		{
			sum = 0.0;
			for(ii = i-1; ii <= i+1; ii++)
				for(jj = j-1; jj <= j+1; jj++)
				{
					sum += buff[ii][jj];
				}
			smoothed[i][j] = sum/9.0;
		}
	}
}

/**
 * Applies a 2-dimensional smoothing filter to the original CHM and
 * returns the smoothed array with an additional 2 pixel buffer.
 *
 * Original buffer is unchanged.
 */
float ** smoothAndPad(float ** chm, int d1, int d2, int smooth_type)
{
	
	int i, j, ii, jj;
	float sum, ** smoothed;
	
	
	smoothed = allocf2d(d1+4, d2+4);
		if(!smoothed){
			return NULL;
		}
	
	// fill with 0's
	for(i = 0; i < d1+4; i++)
	{
		for(j = 0; j < d2+4; j++)
		{
			smoothed[i][j] = 0.0f;
		}
	}
	
	// 'copy edges' of chm to smoothed
	for(i = 0; i < d1; i++)
	{
		for(j = 0; j < d2; j++)
		{
			smoothed[i+2][j+2] = chm[i][j];
		}
	}
	
	if(smooth_type == 1)
	{
		return smoothed;
	}
	
	//
	// apply smoothing
	//
	switch(smooth_type)
	{
		//3x3 boxcar
		case 2:
			
			for(i = 1; i < d1-1; i++){
			for(j = 1; j < d2-1;  j++)
			{
				sum = 0.0;
				for(ii = i-1; ii <= i+1; ii++)
				for(jj = j-1; jj <= j+1; jj++)
					sum += chm[ii][jj];
				
				smoothed[i+2][j+2] = sum / 9.0;			
			}}
			
			break;
			
		//3x3 gaussian
		case 3:
			{
		
			float g3 [3][3] = {
				{0.0751136, 0.123841, 0.0751136},
				{0.1238410, 0.204180, 0.1238410},
				{0.0751136, 0.123841, 0.0751136}};
				
			
			for(i = 1; i < d1-1; i++){
				for(j = 1; j < d2-1; j++)
				{
					sum = 0.0;
					for(ii = 0; ii < 3; ii++)
					for(jj = 0; jj < 3; jj++)
						sum += chm[i-1+ii][j-1+jj] * g3[ii][jj];
					
					smoothed[i+2][j+2] = sum;
				}
			}	
			
			break;
			}
			
		//3x3 gaussian
		case 4:
			{
/*
			float g5 [5][5] = {
				{0.00296902, 0.0133062, 0.0219382, 0.0133062, 0.00296902},
				{0.01330620, 0.0596343, 0.0983203, 0.0596343, 0.0133062},
				{0.02193820, 0.0983203, 0.1621030, 0.0983203, 0.0219382},
				{0.01330620, 0.0596343, 0.0983203, 0.0596343, 0.0133062},
				{0.00296902, 0.0133062, 0.0219382, 0.0133062, 0.00296902}};
*/
			float g5 [5][5] = { 
				{0.01247764154323288, 0.02641516735431067, 0.03391774626899505, 0.02641516735431067, 0.01247764154323288}, 
				{0.02641516735431067, 0.05592090972790156, 0.07180386941492609, 0.05592090972790156, 0.02641516735431067}, 
				{0.03391774626899505, 0.07180386941492609, 0.09219799334529226, 0.07180386941492609, 0.03391774626899505}, 
				{0.02641516735431067, 0.05592090972790156, 0.07180386941492609, 0.05592090972790156, 0.02641516735431067}, 
				{0.01247764154323288, 0.02641516735431067, 0.03391774626899505, 0.02641516735431067, 0.01247764154323288}};
				
			
			for(i = 2; i < d1-2; i++){
				for(j = 2; j < d2-2; j++)
				{
					sum = 0.0;
					for(ii = 0; ii < 5; ii++)
					for(jj = 0; jj < 5; jj++)
						sum += chm[i-2+ii][j-2+jj] * g5[ii][jj];
					
					smoothed[i+2][j+2] = sum;
				}
			}
			
			break;
			}
			
		//do nothing
		default:
			return NULL;
	}
	
	return smoothed;
}

/** 
 * This function works only when the whole image is being processed at once.
 *
 * If the point (i,j) is the highest within the specificied window of the smoothed
 * data - then - write the highest point of the original image in a 3x3 window centered
 * around the point to file.
 *
 * Assumes that the position corresponding to position (i,j) in the original image is
 * at position (i+2, j+2) in 'gridded'.
 */
int checkPixel(float **gridded, float **orig, int i, int j, int wnd_size, int wnd_sqrd,
				double * mpsData, double ulEasting, double ulNorthing, SHPHandle hshp, DBFHandle hdbf)
{  
	
	int ii, jj, half, count = 0, maxi, maxj, nrecords;
	float max, height;
	
	height = gridded[i][j];
	half   = (wnd_size - 1)/2;
	
	for(ii = i - half; ii <= i + half; ii++)
	for(jj = j - half; jj <= j + half; jj++)
		if(gridded[ii][jj] < height)
			count++;
	
	if(count < wnd_sqrd-1)
	{	// not the highest tree in the window
		return 1;
	}
	
	printf("highest found at (%d,%d) = %f\n", i,j, gridded[i][j]);
	
	//find the highest point in the original data set
	maxi = i-3; maxj = j-3;
	max  = orig[maxi][maxj];
	
	for(jj = j - 3; jj <= j - 1; jj++)
	for(ii = i - 3; ii <= i - 1; ii++){
		if(orig[ii][jj] > max)
		{
			max  = orig[ii][jj];
			maxi = ii;
			maxj = jj;
		}
	}
	/**
 	 * Write point to shape file.
 	 */
	SHPObject * shape;
	double * xpts, * ypts, * zpts;
	
	xpts = (double *) malloc(sizeof(double)); if(!xpts){puts("malloc error");return 0;}
	ypts = (double *) malloc(sizeof(double)); if(!ypts){puts("malloc error ypts");return 0;}
	zpts = (double *) malloc(sizeof(double)); if(!ypts){puts("malloc error ypts");return 0;}
	
	xpts[0] = ulEasting + maxj * mpsData[0] + (mpsData[0]/2.0);
	ypts[0] = ulNorthing - maxi * mpsData[1] - (mpsData[1]/2.0);
	zpts[0] = (double)max;
	
	//printf("treetops zpts: %lf\n", zpts[0]);
	
	shape = SHPCreateSimpleObject(SHPT_POINT, 1, xpts, ypts, zpts);
	if(!shape){
		PyErr_SetString(PyExc_Exception, "SHPCreateSimpleObject failed.");
		return 0;
	}
	
	SHPWriteObject(hshp, -1, shape);
	SHPGetInfo(hshp, &nrecords, NULL, NULL, NULL);
	DBFWriteDoubleAttribute(
		hdbf,
		nrecords-1,
		DBFGetFieldIndex(hdbf, "Height"),
		(double)max
	);
	//DBFWriteNULLAttribute(
	//	hdbf, 
	//	nrecords-1, 
	//	DBFGetFieldIndex(hdbf, "TC Index")
	//);
	
	SHPDestroyObject(shape);
	free(xpts);free(ypts);
	
	return 1;
}


/**
 * Works when tree tops are being found in a partition of the original image because padding
 * isn't done.
 */
int checkPixelPart(float **gridded, float **orig, int i, int j, int wnd_size, int wnd_sqrd,
				double * mpsData, double ulEasting, double ulNorthing, SHPHandle hshp, DBFHandle hdbf)
{  
	int ii, jj, half, count = 0, maxi, maxj, nrecords;
	float max, height;
	
	height = gridded[i][j];
	half   = (wnd_size - 1)/2;
	
	for(ii = i - half; ii <= i + half; ii++)
	for(jj = j - half; jj <= j + half; jj++)
		if(gridded[ii][jj] < height)
			count++;
	
	if(count < wnd_sqrd-1)
	{	// not the highest tree in the window
		return 1;
	}
	
	//find the highest point in the original data set
	maxi = i-1, maxj = j-1;
	max  = orig[i-1][j-1];
	
	for(jj = j - 1; jj <= j + 1; jj++)
	for(ii = i - 1; ii <= i + 1; ii++)
		if(orig[ii][jj] > max)
		{
			max  = orig[ii][jj];
			maxi = ii;
			maxj = jj;
		}
		
	//
 	//Write point to shape file.
 	//
	SHPObject * shape;
	double * xpts, * ypts, * zpts;
	
	xpts = (double *) malloc(sizeof(double));
	ypts = (double *) malloc(sizeof(double));
	zpts = (double *) malloc(sizeof(double));
	
	xpts[0] = ulEasting + maxj * mpsData[0];
	ypts[0] = ulNorthing - maxi * mpsData[1];
	zpts[0] = (double)max;
	
	shape = SHPCreateSimpleObject(SHPT_POINT, 1, xpts, ypts, zpts);
	if(!shape){
		PyErr_SetString(PyExc_Exception, "SHPCreateSimpleObject failed.");
		return 0;
	}
	
	SHPWriteObject(hshp, -1, shape);
	SHPGetInfo(hshp, &nrecords, NULL, NULL, NULL);
	DBFWriteDoubleAttribute(
		hdbf, 
		nrecords-1, 
		DBFGetFieldIndex(hdbf, "Height"),
		(double)max
	);
	//DBFWriteNULLAttribute(
	//	hdbf, 
	//	nrecords-1, 
	//	DBFGetFieldIndex(hdbf, "TC Index")
	//);
	
	SHPDestroyObject(shape);
	free(xpts);free(ypts);
	
	return 1;
}

/**
 * Add error checking.
 *
 * orig - buffered tiff elevation model
 * minNorthing - ulNorthing
 * minEasting - ulEasting
 */
int findTreeTops(float **orig, double * mpsData, int imageLength, int imageWidth, double ulEasting, double ulNorthing, 
	int run_range3, float range3_min, float range3_max, 
	int run_range5, float range5_min, float range5_max,
	int run_range7, float range7_min, float range7_max,
	int run_range9, float range9_min, float range9_max, 
	int run_range11, float range11_min, float range11_max, 
	SHPHandle hshp, DBFHandle hdbf, int smooth_type, int add_noise)
{
	puts("findTreeTops");

	int i, j, length, width;
	float **gridded;
	
	// dimensions of gridded
	length  = imageLength + 4;
	width   = imageWidth  + 4;
	
	gridded = smoothAndPad(orig, imageLength, imageWidth, smooth_type);
	if(!gridded)
		return 0;
	
	if (add_noise) {
		for(i = 1; i < length - 1; i++)
			for(j = 1; j < width - 1 ; j++)
				gridded[i][j] += box_muller(0,1)*0.001;
	}
	
	//determine if necessary
	extrapolateEdges(gridded, length, width);
	
	puts("Checking pixels.");
	
	for(j = 2; j < width-2; j++)//each col
	{ 
		for(i = 2; i < length-2; i++) //each row
		{
			float height = gridded[i][j];
			
			// 3 = 1 + 2
			if(i >= 3 && j >= 3 && i < length-3 && j < width-3 && run_range3 && height >= range3_min && height < range3_max)
			{
				checkPixel(gridded, orig, i, j, 3, 9, mpsData, ulEasting, ulNorthing, hshp, hdbf);
			}
			// 4 = 2 + 2 = half_window+buffer
			else if(i >= 4 && j >= 4 && i < length-4 && j < width-4 && run_range5 && height >= range5_min && height < range5_max)
			{
				checkPixel(gridded, orig, i, j, 5, 25, mpsData, ulEasting, ulNorthing, hshp, hdbf);
			}
			else if(i >= 5 && j >= 5 && i < length-5 && j < width-5 && run_range7 && height >= range7_min && height < range7_max)
			{
				checkPixel(gridded, orig, i, j, 7, 49, mpsData, ulEasting, ulNorthing, hshp, hdbf);
			}
			else if(i >= 6 && j >= 6 && i < length-6 && j < width-6 && run_range9 && height >= range9_min && height < range9_max)
			{
				checkPixel(gridded, orig, i, j, 9, 81, mpsData, ulEasting, ulNorthing, hshp, hdbf);
			}
			else if(i >= 7 && j >= 7 && i < length-7 && j < width-7 && run_range11 && height >= range11_min && height < range11_max)
			{
				checkPixel(gridded, orig, i, j, 11, 121, mpsData, ulEasting, ulNorthing, hshp, hdbf);
			}
		}
	}
	
	freef2d(gridded, length);
	freef2d(orig, imageLength);
	
	return 1;
}


int findInPartition(float **image, double * mpsData, int imageLength, int imageWidth, double ulEasting, double ulNorthing,
	int run_range3, float range3_min, float range3_max, 
	int run_range5, float range5_min, float range5_max,
	int run_range7, float range7_min, float range7_max, 
	SHPHandle hshp, DBFHandle hdbf, int add_noise)
{
	puts("findInPartition");
	
/*
	float range3_max, range5_max;
	range3_max = range5_min;
	range5_max = range7_min;
*/

	float **gridded = allocf2d(imageLength, imageWidth);
	if(!gridded){
		return 0;
	}
	
	copy(image, gridded, imageLength, imageWidth);
	smooth(image, gridded, imageLength, imageWidth);
	
	int i, j;
	
	if (add_noise) {
		for(i = 1; i < imageLength-1; i++)
			for(j = 1; j < imageWidth-1; j++)
				gridded[i][j] += box_muller(0,1)*0.001;
	}
	
	for(j = 3/*1*/; j < imageWidth-3/*1*/; j++)//each col
	{ 
		for(i = 3/*1*/; i < imageLength-3/*1*/; i++) //each row
		{
			float height = gridded[i][j];
			
			if(run_range3 && height >= range3_min && height < range3_max)
			{
				checkPixelPart(gridded, image, i, j, 3, 9, mpsData, ulEasting, ulNorthing, hshp, hdbf);
			}
			else if(run_range5 && height >= range5_min && height < range5_max)
			{
				checkPixelPart(gridded, image, i, j, 5, 25, mpsData, ulEasting, ulNorthing, hshp, hdbf);
			}
			else if(run_range7 && height >= range7_min && height < range7_max)
			{
				checkPixelPart(gridded, image, i, j, 7, 49, mpsData, ulEasting, ulNorthing, hshp, hdbf);
			}
		}
	}
	
	freef2d(image, imageLength);
	freef2d(gridded, imageLength);
	
	return 1;
}
