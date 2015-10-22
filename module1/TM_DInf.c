#include "TM_module.h"
#include <stdio.h>
#include <math.h>

#define	PI	3.14592654

extern int IMAGE_WIDTH;
extern int IMAGE_LENGTH;
extern int NUMBER_OF_ROWS;
extern int NUMBER_OF_COLS;

float ** fDir;

int getIndexOfMaxSlope(float * slopes)
{
	int i;
	float max, temp, maxIndex;
	
	max = slopes[0];
	maxIndex = 0;
	
	for(i = 1; i < 8; i++)
	{
		temp = slopes[i];
		if(temp > max)
		{
			max = temp;
			maxIndex = i;
		}
	}
	return maxIndex;
}

void facetFlow(float e0, float e1, float e2, float * slope, float * theta)
{
	float s1, s2, diag_angle, diag_dist;
	
	s1 = (e0 - e1); // /d1;
	s2 = (e1 - e2); // /d2;
	
	if(s2 == 0 && s1 == 0)
		*theta = 0.0;
	else
		*theta = (float) atan2(s2, s1);
	
	/*
	if(s1 == 0.0)
		*theta = 0.0;
	else
		*theta = atan(s2/s1);
	*/
		
	diag_angle = atan(1/1);
	diag_dist  = sqrt(1*1 + 1*1);
	
	if(*theta < 0)
	{
		float t1, t2;
		t1 = s1;
		t2 = (e0 - e2)/diag_dist;
		if(t2 > t1){
			*theta = diag_angle;
			*slope = t2;
		}else{
			*theta = 0;
			*slope = t1;
		}
		/*
		*theta = 0;
		*slope = s1;
		*/
	}
	else if(*theta > diag_angle)
	{
		*theta = diag_angle;
		*slope = (e0 - e2)/diag_dist;
	}
	else
	{
		*slope = (float)sqrt(s1*s1 + s2*s2);
	}
}

void pixelFlow(float ** elevMap, int row, int col, float * r)
{
	int facet, maxIndex, maxSlope;
	
	float slopes[8], angles[8], middle;
	
	float ac [] = {0,  1, 1,  2, 2,  3, 3,  4};
	float af [] = {1, -1, 1, -1, 1, -1, 1, -1};
	float e1 [] = {
		elevMap[row][col+1], elevMap[row-1][col], elevMap[row-1][col], elevMap[row][col-1],
		elevMap[row][col-1], elevMap[row+1][col], elevMap[row+1][col], elevMap[row][col+1]
	};
	float e2 [] = {
		elevMap[row-1][col+1], elevMap[row-1][col+1], elevMap[row-1][col-1], elevMap[row-1][col-1],
		elevMap[row+1][col-1], elevMap[row+1][col-1], elevMap[row+1][col+1], elevMap[row+1][col+1]
	};
	
	middle = elevMap[row][col];
	
	for(facet = 0; facet < 8; facet++)
	{
		facetFlow( middle, e1[facet], e2[facet], &(slopes[facet]), &(angles[facet]) );
	}
	
	maxIndex  = getIndexOfMaxSlope(slopes);
	maxSlope  = slopes[maxIndex];
	
	if(maxSlope >= 0)
	// max downslope is positive - that is angle in facet
	{
		*r = af[maxIndex] * angles[maxIndex] + ac[maxIndex] * (PI/2);
	}
	else
	// pit or flat : -ve to indicate no flow
	{
		*r = -0.01;
	}
	fDir[row][col] = *r;
}

/*
 * Not used. Although could be. Just an alternative way to calculate the direction
 * of maximum flow.
 */
void pixelFlowAspect(float ** elevMap, int row, int col, float * r)
{
	float g, h, middle;
			
	middle = elevMap[row][col];
	
	g = (elevMap[row+1][col  ] - elevMap[row-1][col  ]) / (2 * 3);
	h = (elevMap[row  ][col-1] - elevMap[row  ][col+1]) / (2 * 3);
	
	//if( (row % 200 == 0) && (col % 200 == 0) )
		//printf("%f\t%f\t%f\n", IMAGE_BUFFER[row-1][col-1], IMAGE_BUFFER[row-1][col], IMAGE_BUFFER[row-1][col+1]);
	
	if( ((h == 0) && (g == 0)) || (g == 0) )
	{
		fDir[row+1][col] = 361 * (2*PI / 360);
	}
	else
	{
		float temp1 = fabs(h/(double)g);
		float temp  = (atan(temp1))*57.958;
		if(temp < 0)
		{
			fDir[row+1][col] = 361 * (2*PI / 360);
		}
		else
		{
			if(h > 0 && g > 0)
			{
				*r = 270-(fabs(temp)) + 180;
				if(*r > 360.0)
					*r -= 360.0;
				*r *= (2*PI / 360);
				fDir[row+1][col] = *r;
			}
			else if (h > 0 && g < 0)
			{
				*r = 90 +(fabs(temp)) + 180;
				if(*r > 360.0)
					*r -= 360.0;
				*r *= (2*PI / 360);
				fDir[row+1][col] = *r;
			}
			else if(h < 0 && g < 0)
			{
				*r = 90 -(fabs(temp)) + 180;
				if(*r > 360.0)
					*r -= 360.0;
				*r *= (2*PI / 360);
				fDir[row+1][col] = *r;
			}
			else if(h < 0 && g > 0)
			{
				*r = 270+(fabs(temp)) + 180;
				if(*r > 360.0)
					*r -= 360.0;
				*r *= (2*PI / 360);
				fDir[row+1][col] = *r;
			}
		}
	}
}

void upslopeArea(float ** elevMap, float ** area, int row, int col)
{
	if( area[row][col] > 0 		||
		row <= 0 		|| 
		row >= NUMBER_OF_ROWS-1 || 
		col <= 0 		|| 
		col >= NUMBER_OF_COLS-1		)
	{
		return;
	}
	
	area[row][col] = 1.0; // itself
	
	float r, p;
	
	int neighbors [8][2] = {
		{row, col+1}, {row-1, col+1}, {row-1, col}, {row-1, col-1}, 
		{row, col-1}, {row+1, col-1}, {row+1, col}, {row+1, col+1}
	};
	
	int i;
	for(i = 0; i < 8; i++)
	// for each 8 neighbors
	{
		//float s;
		//pixelFlow(elevMap, neighbors[i][0], neighbors[i][1], &s, &r);
		r = fDir[neighbors[i][0]][ neighbors[i][1]];
		
		if(r < 0.0) continue; // no flow -- pit or flat
		
		p = 0.0;
		
		switch (i)
		{
			case 0:
				
				if(r > 3*PI/4 && r < PI) 
					p = (r - 3*PI/4) / (PI/4);
				else if(r >= PI && r < 5*PI/4) 
					p = (5*PI/4 - r) / (PI/4);
				break;
				
			case 1:
				
				if(r > PI && r < 5*PI/4) 
					p = (r - PI) / (PI/4);
				else if(r >= 5*PI/4 && r < 3*PI/2) 
					p = (3*PI/2 - r) / (PI/4);
				break;
				
			case 2:
				
				if(r > 5*PI/4 && r < 3*PI/2) 
					p = (r - 5*PI/4) / (PI/4);
				else if(r >= 3*PI/2 && r < 7*PI/4) 
					p = (7*PI/4 - r) / (PI/4);
				break;
				
			case 3:
				
				if(r > 3*PI/2 && r < 7*PI/4) 
					p = (r - 3*PI/2) / (PI/4);
				else if(r >= 7*PI/4 && r < 2*PI) 
					p = (2*PI - r) / (PI/4);
				break;
				
			case 4:
				
				if(r == 2*PI || r == 0)
					p = 1;
				else if(r > 7*PI/4 && r < 2*PI) 
					p = (r - 7*PI/4) / (PI/4);
				else if(r > 0 && r < PI/4) 
					p = (PI/4 - r) / (PI/4);
				break;
				
			case 5:
				
				if(r > 0 && r < PI/4) 
					p = (r - 0) / (PI/4);
				else if(r >= PI/4 && r < PI/2) 
					p = (PI/2 - r) / (PI/4);
				break;
				
			case 6:
				
				if(r > PI/4 && r < PI/2) 
					p = (r - PI/4) / (PI/4);
				else if(r >= PI/2 && r < 3*PI/4) 
					p = (3*PI/4 - r) / (PI/4);
				break;
				
			case 7:
				
				if(r > PI/2 && r < 3*PI/4) 
					p = (r - PI/2) / (PI/4);
				else if(r >= 3*PI/4 && r < PI) 
					p = (PI - r) / (PI/4);
				break;
				
			default:
				break;
		}
		if(p < 0 || p > 1)
			printf("i : %d , p : %f\n",i, p);
		
		if(p > 0){
			upslopeArea(elevMap, area, neighbors[i][0], neighbors[i][1]);
			area[row][col] = area[row][col] + p * area[ neighbors[i][0] ][ neighbors[i][1] ];	
		}
	}
}

//todo - remove allocation of memeory from this file
void Dinf(float ** elevMap, float ** outBuf)
{
	int row, col;
	
	fDir = (float **) _TIFFmalloc(sizeof(float *) *  NUMBER_OF_ROWS);
	for(row = 0; row < NUMBER_OF_ROWS; row++)
	{
		fDir[row] = (float *) _TIFFmalloc(sizeof(float) *  NUMBER_OF_COLS);
		
		for(col = 0; col < NUMBER_OF_COLS; col++)
		{
			fDir  [row][col]   = 0.0;
		}
	}
	
	float r;
	for(row = 1; row < NUMBER_OF_ROWS-1; row++)
	for(col = 1; col < NUMBER_OF_COLS-1; col++){
			pixelFlow(elevMap, row, col, &r);
	}
	
	for(row = 1; row < NUMBER_OF_ROWS-1; row++)
	for(col = 1; col < NUMBER_OF_COLS-1; col++){
			upslopeArea(elevMap, outBuf, row, col);
	}
	
	for(row = 1; row < NUMBER_OF_ROWS-1; row++)
	for(col = 1; col < NUMBER_OF_COLS-1; col++){
		if(outBuf[row][col] == 0.0)
			outBuf[row][col] = 1.0;
	}
	
	// todo : remove this ... for debugging purposes at the moment --> -!-
	/*
	TIFF * fdout;
	printf("Writing flow direction to file\n");
	if((fdout = TIFFOpen("/Users/Ben/Desktop/out/TM_flowDirection.tif", "w")) == NULL) return;
	TIFFSetField(fdout, TIFFTAG_IMAGEWIDTH, 		IMAGE_WIDTH);
	TIFFSetField(fdout, TIFFTAG_IMAGELENGTH, 		IMAGE_LENGTH);
	TIFFSetField(fdout, TIFFTAG_BITSPERSAMPLE, 		32);
	TIFFSetField(fdout, TIFFTAG_COMPRESSION, 		COMPRESSION_NONE);
	TIFFSetField(fdout, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
	TIFFSetField(fdout, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
	TIFFSetField(fdout, TIFFTAG_DATATYPE, 			3);
	for(row = 0; row < NUMBER_OF_ROWS; row++)
	{
		TIFFWriteScanline(fdout, fDir[row], row, 0);
		_TIFFfree(fDir[row]);
	}
	TIFFClose(fdout);
	_TIFFfree(fDir);
	*/
}
