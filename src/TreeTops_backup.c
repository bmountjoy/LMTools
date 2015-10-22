

#include "TCD_module.h"

#include <stdlib.h>	
#include <stdio.h>

void copy(float **from, float **to, int imageLength, int imageWidth)
{
	int i, j;
	for(i = 0; i < imageLength; i++)
	for(j = 0; j < imageWidth; j++)
		to[i+2][j+2] = from[i][j];
}

/**
 * - copies orig image into new buffer with 2 rows & cols of padding
 * - fills in out rows & cols with its mirror
 */
float ** copyAndPad(float **from, int imageLength, int imageWidth)
{
	int i, j;
	float **to = allocf2d(imageLength+4, imageWidth+4);
	if(!to){
		return NULL;
	}
	
	//copy values
	for(i = 0; i < imageLength; i++)
	for(j = 0; j < imageWidth; j++)
		to[i+2][j+2] = from[i][j];
	
	//copy cols
	for(i = 0; i < imageLength; i++)
	{
		to[i+2/*3*/][2] = from[i][2];
		to[i+2/*3*/][1] = from[i][3];
		to[i+2/*3*/][0] = from[i][4];
		to[i+2/*3*/][imageWidth+1] = from[i][imageWidth-3];
		to[i+2/*3*/][imageWidth+2] = from[i][imageWidth-4];
		to[i+2/*3*/][imageWidth+3] = from[i][imageWidth-5];
	}
	//copy rows
	for(j = 0; j < imageWidth; j++)
	{
		to[2][j+2/*3*/] = from[2][j];
		to[1][j+2/*3*/] = from[3][j];
		to[0][j+2/*3*/] = from[4][j];
		to[imageLength+1][j+2/*3*/] = from[imageLength-3][j];
		to[imageLength+2][j+2/*3*/] = from[imageLength-4][j];
		to[imageLength+3][j+2/*3*/] = from[imageLength-5][j];
	}
	
	return to;
}

/**
 * Applies a 2-dimension boxcare average of width 3 and returns the smoothed
 * array and leaving the originial array unchanged.
 */
float ** smooth(float **buff, int d1, int d2)
{
	int i, j, ii, jj;
	float sum;
	float **smoothed = allocf2d(d1, d2);
	if(!smoothed){
		return NULL;
	}
	
	//copy edges
	for(i = 0; i < d1; i++)
	{
		smoothed[i][0] = buff[i][0];		
		smoothed[i][d2-1] = buff[i][d2-1];
	}
	for(j = 0; j < d2; j++)
	{
		smoothed[0][j] = buff[0][j];		
		smoothed[d1-1][j] = buff[d1-1][j];
	}
	
	//apply average to middle
	for(i = 1; i < d1-1; i++)
	for(j = 1; j < d2-1; j++)
	{	
		sum = 0;
		for(ii = i-1; ii <= i+1; ii++)
		for(jj = j-1; jj <= j+1; jj++){
			sum += buff[ii][jj];
		}
		smoothed[i][j] = sum/9.0;		
	}
	return smoothed;
}

/** 
 * If the point (i,j) is the highest within the specificied window of the smoothed
 * data save that value in position (i,j) of the output array.
 */
void checkPixel(float **gridded, float **outArr, float height, int i, int j, int wnd_size, int wnd_sqrd)
{  
	int ii, jj, half, count = 0;
	
	half = (wnd_size - 1)/2;
	
	for(ii = i - half; ii <= i + half; ii++)
	for(jj = j - half; jj <= j + half; jj++)
		if(gridded[ii][jj] < height)
			count++;
	
	if(count == wnd_sqrd-1)
		outArr[i][j] = height;
}

void findTreeTops(float **orig, int imageLength, int imageWidth, double minEasting, double minNorthing, FILE * fpOut)
{
	// add as parameters
	float range3_min, range3_max, range5_min, range5_max, range7_min, range7_max;
	range3_min = 3;
	range3_max = 6;
	range5_min = 6;
	range5_max = 35;
	range7_min = 35;
	range7_max = 100;
	// end add as params
	
	int i, j, length, width;
	float **back, **temp, **gridded, **outArr;
	
	back = copyAndPad(orig, imageLength, imageWidth);
	if(!back){
		return;
	}
	puts("copyandpad back,orig");
	temp = smooth(orig, imageLength, imageWidth); freef2d(orig, imageLength);
	puts("temp <- smooth(orig)");
	puts("2 copies live");
	puts("applying box muller");
	for(i = 0; i < imageLength; i++)
		for(j = 0; j < imageWidth; j++)
		{
			temp[i][j] += box_muller(0,1)*0.01; 
		}
	
	gridded = copyAndPad(temp, imageLength, imageWidth); freef2d(temp, imageLength);
	puts("2 copies live");
	
	length = imageLength + 4;
	width  = imageWidth + 4;
	
	outArr = allocf2d(length, width);
	if(!outArr){
		return;
	}
	puts("3 copies live");
	
	for(i=0;i<length;i++)for(j=0;j<width;j++)outArr[i][j]=0;
	
	for(j = 3/*1*/; j < width-4/*1*/; j++){ //each col
		for(i = 3/*1*/; i < length-4/*1*/; i++) //each row
		{
			float height = gridded[i][j];
			
			if(height >= range3_min && height < range3_max && 1 /*check 3 flag*/)
			{
				checkPixel(gridded, outArr, height, i, j, 3, 9);
			}
			else if(height >= range5_min && height < range5_max && 1 /*check 5 flag*/)
			{
				checkPixel(gridded, outArr, height, i, j, 5, 25);
			}
			else if(height >= range7_min && height < range7_max && 1 /*check 7 flag*/)
			{
				checkPixel(gridded, outArr, height, i, j, 7, 49);
			}
		}
	}
	freef2d(gridded, length);
	puts("2 copies live");
	
	for(j = 3; j <= width-4; j++){
		for(i = 3; i <= length-4; i++)
		{
			float pixelHeight = outArr[i][j];
			
			if(pixelHeight < 3)
				continue;
			
			//check 3x3 window
			int ii, jj, maxi = i-1, maxj = j-1;
			float max = back[i-1][j-1];
			for(jj = j-1; jj<=j+1; jj++)
				for(ii = i-1; ii<=i+1; ii++)
					if(back[ii][jj] > max){
						max  = back[ii][jj];
						maxi = ii;
						maxj = jj;
					}
			
			fprintf(fpOut, "%.2lf %.2lf %.2f\n", minEasting-2+maxj, minNorthing+2-maxi, max);
		}
	}
	
	freef2d(back, length);
	freef2d(outArr, length);
}
