

#include <math.h>
#include "SAM.h"


float spectral_angle (float * xx, float * yy, int size)
{
	int i;
	
	double x [size];
	double y [size];
	
	for(i = 0; i < size; i++){
		x[i] = (double)xx[i];
		y[i] = (double)yy[i];
	}
	
	double dot_prod = 0.0, x_norm = 0.0, y_norm = 0.0;
	
	
	for(i = 0; i < size; i++)
	{
		dot_prod += x[i] * y[i];
		x_norm   += x[i] * x[i];;
		y_norm   += y[i] * y[i];
	}
	
	x_norm = sqrt(x_norm);
	y_norm = sqrt(y_norm);
	
	if(x_norm == 0 || y_norm == 0)
		return 0.0f;
	
	return (float)(dot_prod / (x_norm * y_norm));
}


double mean(double * x, int size)
{
	int i;
	double sum = 0.0;
	
	for(i = 0; i < size; i++){
		sum += x[i];
	}
	
	return sum/size;
		
}

float pearson_correlation(float * xx, float * yy, int size)
{
	int i;
	
	double x [size];
	double y [size];
	
	for(i = 0; i < size; i++){
		x[i] = (double)xx[i];
		y[i] = (double)yy[i];
	}
	
	double x_mean = mean(x,size);
	double y_mean = mean(y,size);
	
	double xt = 0, yt = 0, sxx = 0, syy = 0, sxy = 0;
	
	for(i = 0; i < size; i++)
	{
		xt = x[i] - x_mean;
		yt = y[i] - y_mean;
		sxx += xt * xt;
		syy += yt * yt;
		sxy += xt * yt;
	}
	return (float)(sxy/(sqrt(sxx*syy)));
}
