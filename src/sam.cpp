

#include <math.h>
#include <vector>

#include "sam.hpp"

using namespace std;

float spectral_angle (vector<double>& x, vector<double>& y)
{
	if(x.size() != y.size())
		return -2.0f;
	
	if(x.size() <= 1)
		return 0.0f;
	
	int i;
	double dot_prod = 0.0, x_norm = 0.0, y_norm = 0.0;
	
	for(i = 0; i < x.size(); i++)
	{
		dot_prod += x.at(i)*y.at(i);
		x_norm   += x.at(i)*x.at(i);
		y_norm   += y.at(i)*y.at(i);
	}
	
	x_norm = sqrt(x_norm);
	y_norm = sqrt(y_norm);
	
	if(x_norm == 0 || y_norm == 0)
		return 0.0f;
	
	return (float)(dot_prod / (x_norm * y_norm));
}
