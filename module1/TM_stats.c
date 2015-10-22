
/**
 * 
 */

#include <math.h>
#include <stdio.h>

float calcMean(float * array, int size)
{
	float sum = 0.0;
	int i;
	for(i = 0; i < size; i++)
	{
		sum += array[i];
	}
	
	return sum / size;
}

float calcVariance(float * array, int size)
{
	float mean, diff_sum = 0.0;
	int i;
	
	mean = calcMean(array, size);
	
	for(i = 0; i < size; i++)
	{
		diff_sum += pow( array[i] - mean, 2);
	}
	
	return diff_sum / (size - 1);
}

int main()
{
	float array [20] = {
		1.0, 2.0, 3.0, 4.0, 5.0, 5.5, 5.5, 6.0, 6.7, 6.7,
		5.6, 6.5, 8.0, 9.0, 8.1, 8.3, 8.5, 3.4, 4.5, 5.3
	};
	
	int size = 20;
	
	printf("mean : %f\n", calcMean(array, size));
	printf("variance : %f\n", calcVariance(array, size));
	
	return 1;
}

