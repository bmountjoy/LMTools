

#include "math.h"
#include <vector>
#include "pearson_correlation.hpp"

using namespace std;

double mean(vector<double> x)
{
	int i;
	double sum = 0.0;
	
	for(i = 0; i < x.size(); i++)
	{
		sum += x.at(i);
	}
	
	return sum/x.size();
		
}

float pearson_correlation(vector<double>& x, vector<double>& y)
{
	int i;
	
	double x_mean = mean(x);
	double y_mean = mean(y);
	
	double xt = 0, yt = 0, sxx = 0, syy = 0, sxy = 0;
	
	for(i = 0; i < x.size(); i++)
	{
		xt = x.at(i) - x_mean;
		yt = y.at(i) - y_mean;
		sxx += xt * xt;
		syy += yt * yt;
		sxy += xt * yt;
	}
	
	return (float)(sxy/(sqrt(sxx*syy)));
}
