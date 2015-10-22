

#include "Quantize.h"

int maxFrequency(int * freq, int size)
{
	int i, maxFreq, maxi;
	
	maxi = 0; 
	maxFreq = -1;
	
	for(i = 0; i < size; i++)
	{
		if(freq[i] > maxFreq)
		{
			maxFreq = freq[i];
			maxi = i;
		}
	}
	return maxi;
}


int min(float * buff, int size)
{
	int i, min, mini;
	
	min  = 999999;
	mini = 0;
	
	for(i = 0; i < size; i++)
	{
		if(buff[i] < min)
		{
			min = buff[i];
			mini = i;
		}
	}
	
	return mini;
}


void derivative(float * derivative, int * bucketFreq, int nBuckets)
{
	int i;
	for(i = 0; i < nBuckets-1; i++)
	{
		derivative[i] = bucketFreq[i+1] - bucketFreq[i];
	}
}

/**
 * zValues must be sorted in increasing order
 */
void quantize(float * zValues, int nPoints, int * bucketFreq, int nBuckets, float bucketSize)
{
	int i, j, k;
	float bucketMax;
	
	i = 0;
	for(j = 0; j < nBuckets; j++)
	{
		k = 0;
		bucketMax = bucketSize * (j + 1);
		
		while(zValues[i] < bucketMax && i < nPoints)
		{
			k++;
			i++;
		}
		
		bucketFreq[j] = k;
		
		if(i == nPoints)
			break;
	}
	
	//for(i = 0; i < nBuckets; i++)
	//	printf("Bucket[%d] = %d\n", i, bucketFreq[i]);
}

float computeTreeCrownHeight(float * zValues, int nPoints, float bucketSize)
{	
	int * bucketFreq, nBuckets, minIdx;
	float * freqDeriv;
	
	if(nPoints == 0){
		printf("%d is not enough points\n", nPoints);
		return -1;
	}
	
	//printf("{%f -> %f}\n", zValues[0], zValues[nPoints-1]);
	
	nBuckets = (int)ceil( zValues[nPoints-1]/bucketSize );
	
	if(nBuckets < 2){
		printf("%d buckets isn't enough\n", nBuckets);
		return -1;
	}
	
	
	if((bucketFreq = alloci1d(nBuckets)) == NULL)
		return -1;
	if((freqDeriv = allocf1d(nBuckets-1)) == NULL)
		return -1;
	
	
	/**
	 * Partition z values into buckets.
	 */
	quantize(zValues, nPoints, bucketFreq, nBuckets, bucketSize);
	maxFrequency(bucketFreq, nBuckets);
	
	/**
	 * Compute the freq/height derivative.
	 */
	derivative(freqDeriv, bucketFreq, nBuckets);
	
	/**
	 * Find the tree crown height.
	 */
	minIdx = min(freqDeriv, nBuckets-1);
	//printf("Min derivative is %f at %d\n", freqDeriv[minIdx], minIdx);
	
	free(bucketFreq);
	free(freqDeriv);
	
	/** 
	 * Return the value between buckets minIdx and minIdx+1
	 */
	return (bucketSize * (minIdx + 1));
	
}
