#ifndef BIO_BIOMETRICS_H
#define BIO_BIOMETRICS_H

typedef struct
{
	float * points;
	int     size;
}
	Array1D;

float * 	computeBiometrics(float * zValues, int numz, int numAll, int zThreshold);
void 		computeGap(int numThresh, int numAll, float * result);
void 		computeRugosity(float * buf, int size, float * result);
void 		computePercentile(float * zValues, int numVals, float percentile, float * result);
int 		getCCFCount(float * zValues, int numValues, float threshold);
int 		floatCompare(const void * a, const void * b);
double		getDoubleMax(double*, int);
double		getDoubleMin(double*, int);
float  		getMax(float*, int);
Array1D 	filterByZThreshold(float * array, int numPoints, float zThresh);
Array1D 	filterByArea(double ** points, int numPoints, double xStart, double yStart, double xEnd, double yEnd);
void gridPoints(
	int dimensions,
	int resolution,
	double blkEasting,
	double blkNorthing,
	double ** points,
	int numPoints, 
	int ** num_z,
	int ** num_all,
	float *** z_values,
	float *** z_all,
	float threshold,
	float zmax
	);

#endif
