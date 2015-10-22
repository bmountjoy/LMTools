


#include "Biometrics.h"


int floatCompare(const void * a, const void * b)
{
	float 
	result = *(float*)a - *(float*)b;
	
	if(result > 0)
		return 1;
	else if(result == 0)
		return 0;
	else	
		return -1;
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

float computeGap(int numThresh, int numAll)
{
	return 1 - ((float)numThresh / (float)numAll);
}

float calculateMean(float * array, int size)
{
	if(size <= 0)
		return -999.0;
	
	int i;
	float sum = 0.0;
	
	for(i = 0; i < size; i++)
		sum += array[i];
	
	return sum / size;
}

float calculateVariance(float * array, int size)
{
	float mean, diff_sum = 0.0;
	int i;
	
	mean = calculateMean(array, size);
	
	for(i = 0; i < size; i++)
		diff_sum += pow( array[i] - mean, 2);
	
	return diff_sum / (size - 1);
}

float computeRugosity(float * buf, int size)
{
	return pow(calculateVariance(buf, size), 0.5);
}

float averageAbovePercentile(float * zValues, int numVals, float percentile)
{
	Array1D B;
	float pValue;
	
	pValue = zValues[ (int)(numVals * percentile) ];	 // this probably isn't how the 85th percentile should be found
	B      = filterValuesLess(zValues, numVals, pValue);
	
	return calculateMean(B.points, B.size);
}

int getCCFCount(float * z_values, int n_values, float height)
{
	int count = 0, i;
	
	for(i = 0; i < n_values; i++)
		if(z_values[i] > height) 
			++count;
	
	return count;
}


/**
 * zValues must be sorted in increasing order.
 */
void computeLhq(float * zValues, int numZ, float * lhq)
{	
	if(numZ < 25) return;
	
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


/**
 * zValues must be sorted in increasing order.
 */
void computeCcf(float * zValues, int numZ, int numAll, float zThreshold, float * ccf)
{	
	if(numZ < 25) return;
	
	int band, ccfCount;
	float curHeight, ccfPercent, htIncrement;
	
	htIncrement = (zValues[numZ-1] - zThreshold) / 20.0; //printf("htIncrement %f\n", htIncrement);
	curHeight   = zThreshold;
	
	for(band = 0; band <= 20; band++)
	{
		ccfCount   = getCCFCount(zValues, numZ, curHeight);
		ccfPercent = ccfCount / (float) numAll;
		
		ccf[band] = ccfPercent;
		
		curHeight += htIncrement;
	}
}


/**
 *  Array must be sorted in increasing order.
 */
float * computeLMoments(float * array, int size)
{
	int i;
	
	int N = size;
	
	float L1, L2, L3, L4, V, CL1, CL2, CL3, CR1, CR2, CR3, C1, C2, C3, C4,
			L_MEAN,
			L_COV,
			L_SKEW,
			L_KURT;
			
	L1 = 0.0;
	L2 = 0.0;
	L3 = 0.0;
	L4 = 0.0;
	
	for(i = 0; i < size; i++)
	{
		V   = i + 1;
		CL1 = V - 1;
		CL2 = CL1 * (V - 1 - 1) / 2.0;
		CL3 = CL2 * (V - 1 - 2) / 3.0;
		CR1 = N - V;
		CR2 = CR1 * (N - V - 1) / 2.0;
		CR3 = CR2 * (N - V - 2) / 3.0;
		L1  = L1 + array[i];
		L2  = L2 + (CL1-CR1)*array[i];
		L3  = L3 + (CL2 - 2*CL1 * CR1 + CR2 ) * array[i];
		L4  = L4 + (CL3 - 3*CL2 * CR1 + 3*CL1 * CR2 - CR3 ) * array[i];
	}
	
	C1     = N;
	C2     = C1*(N-1)/2.0;
	C3     = C2*(N-2)/3.0;
	C4     = C3*(N-3)/4.0;
	L1     = L1/C1;
	L2     = L2/C2/2.0;
	L3 	   = L3/C3/3.0;
	L4     = L4/C4/4.0;
	L_MEAN = L1;
	L_COV  = L2/L1;
	L_SKEW = L3/L2;
	L_KURT = L4/L2;
	
	float * rVal = (float*)malloc(sizeof(float)*4);
	
	rVal[0] = L_MEAN;
	rVal[1] = L_COV;
	rVal[2] = L_SKEW;
	rVal[3] = L_KURT;
	
	return rVal;
}

float * computeBiometrics(float * zValues, int numz, int numAll, float zThreshold)
{
	int i;
	float * bio, *lhq, *ccf;
	
	bio = (float*)malloc(sizeof(float) * 52);
	if(!bio)
		return NULL;
	
	for(i = 0; i < 52; i++) bio[i] = -999.0;
	
	lhq = &bio[10];
	ccf = &bio[31];
	
	qsort(zValues, numz, sizeof(float), floatCompare);
		
	
	bio[0] = computeRugosity(zValues, numz);
	bio[1] = computeGap(numz, numAll);
	bio[2] = averageAbovePercentile(zValues, numz, 0.85);
	bio[8] = numz;
	bio[9] = numAll;
	
	//might have to axe this
	if(numz <= 25)
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
