#ifndef BIOMETRICS_H
#define BIOMETRCIS_H

#include "memory.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct
{
	float * points;
	int     size;
}
	Array1D;
	
float * computeLMoments(float * array, int size);
void computeLhq(float * zValues, int numZ, float * lhq);
void computeCcf(float * zValues, int numZ, int numAll, float zThreshold, float * ccf);
float * computeLMoments(float * array, int size);
float * computeBiometrics(float * zValues, int numz, int numAll, float zThreshold);

#endif
