

#include "TCR_module.h"
#include <math.h>

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
	
	for(i = 0; i < nBuckets; i++)
		printf("Bucket[%d] = %d\n", i, bucketFreq[i]);
}

float computeTreeCrownHeight(float * zValues, int nPoints, float bucketSize)
{
	int * bucketFreq, nBuckets, minIdx;
	float * freqDeriv;
	
	nBuckets = (int)ceil( zValues[nPoints-1]/bucketSize );

	if((bucketFreq = alloci1d(nBuckets)) == NULL)
	{
		puts("error allocationg memory 1");
		return -1;
	}
	
	if((freqDeriv = allocf1d(nBuckets-1)) == NULL)
	{
		puts("error allocationg memory 2");
		return -1;
	}
	
	/**
	 * Partition z values into buckets.
	 */
	quantize(zValues, nPoints, bucketFreq, nBuckets, bucketSize);
	int maxf = maxFrequency(bucketFreq, nBuckets);
	printf("Max frequency is %d at %d\n", bucketFreq[maxf], maxf);
	
	/**
	 * Compute the freq/height derivative.
	 */
	derivative(freqDeriv, bucketFreq, nBuckets);
	
	/**
	 * Find the tree crown height.
	 */
	minIdx = min(freqDeriv, nBuckets-1);
	printf("Min derivative is %f at %d\n", freqDeriv[minIdx], minIdx);
	
	free(bucketFreq);
	free(freqDeriv);
	
	/** 
	 * Return the value between buckets minIdx and minIdx+1
	 */
	return (bucketSize * (minIdx + 1));
	
}
