
#include <Python/Python.h>

#include <stdlib.h>
#include <stdio.h>

#include "Bio_LMoments.h"

/**
 * 'array' must be sorted in increasing order.
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
