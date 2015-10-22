

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern int MIN_THRESH;			//defined in TM_module.c
extern int MAX_THRESH;			//defined in TM_module.c

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

/** take these 1D functions out of here **/

float calculateMean1D(float * array, int size)
{
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
	return pow
	(
		calculateVariance1D(array, size), 
		0.5
	);
}

/** end of 1D functions **/

float calculateMean(float ** buf, int numRows, int numCols)
{
	float sum;
	int row, col, N;
	
	N = 0;
	sum = 0.0;
	
	for(row = 0; row < numRows; row++)
	for(col = 0; col < numCols; col++)
	{
		//TDOD: check this is correct
		if(buf[row][col] > MIN_THRESH && buf[row][col] < MAX_THRESH)
		{
			sum += buf[row][col];
			++N;
		}
	}
	
	return sum / N;
}

float calculateStandardDeviation(float ** buf, int numRows, int numCols)
{
	int row, col, N;
	float mean, sum;
	
	N = 0;
	sum = 0.0;
	mean = calculateMean(buf, numRows, numCols);
	
	for(row = 0; row < numRows; row++)
	for(col = 0; col < numCols; col++)
	{
		if(buf[row][col] > MIN_THRESH && buf[row][col] < MAX_THRESH)
		{
			sum += pow( buf[row][col] - mean, 2 );
			++N;
		}
	}
	
	return pow( (sum/N), 0.5 );
}

float * calculateRange(float ** buf, int numRows, int numCols)
{
	int minIdx, maxIdx, row, col;
	float temp, * range;
	
	range = (float *)malloc(sizeof(float) * 2); //bad practice
	minIdx = 0;
	maxIdx = 1;
	
	//protect against bad values
	range[minIdx] = MAX_THRESH;
	range[maxIdx] = MIN_THRESH;
	
	for(row = 0; row < numRows; row++){
		for(col = 0; col < numCols; col++)
		{
			temp = buf[row][col];
			
			if( !(temp > MIN_THRESH && temp < MAX_THRESH) )
				continue;
			
			if(temp < range[minIdx])
				range[minIdx] = temp;
			if(temp > range[maxIdx])
				range[maxIdx] = temp;
		}
	}
	
	return range;
}

float kth_smallest(float a[], int n, int k) 
{
	register int i,j,l,m ; 
	register float x,t ;
	
	l=0 ; m=n-1 ; 
	while (l<m) {
		x=a[k] ; 
		i=l ; 
		j=m ; 
		do {
			while (a[i]<x) i++; 
			while (x<a[j]) j-- ;
			if(i<=j){
				t = a[i]; a[i] = a[j]; a[j] = t;
				i++ ; j-- ;
			} 
		} while (i<=j) ;
		if (j<k) l=i ; 
		if (k<i) m=j ;
	} 
	return a[k] ;
}


/**
 * Apply a width x width median filter to 'buf' save result to 'out'.
 */
void medianFilter(float ** buf, float ** out, int width, int numRows, int numCols)
{
	int offSet, endRow, endCol, wSqrd, i, j, index, ws_d2;
	
	offSet   = (int)(width/2);
	endRow   = numRows - width;
	endCol   = numCols - width;
	wSqrd    = width*width;
	ws_d2    = wSqrd/2;
	
	float values [wSqrd];
	
	int row, col;
	for(row = 0; row <= endRow; row++){if(row%500 == 0) printf("\trow: %d\n", row);
		for(col = 0; col <= endCol; col++)
		{
			index = 0;
			for(i = row; i < row + width; i++){
				for(j = col; j < col + width; j++)
				{
					values[index++] = buf[i][j];
				}
			}
			
			//qsort(values, wSqrd, sizeof(float), floatCompare);
			//out[row + offSet][col + offSet] = values[ws_d2];
			out[row + offSet][col + offSet] = kth_smallest(values, wSqrd, ws_d2);
		}
	}
}
