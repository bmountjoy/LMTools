

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


int deriv_next_positive(float * a, int size, int curr)
{
	while(curr < size && a[curr] < 0) curr++;
	return curr;
}

int deriv_next_negative(float * a, int size, int curr)
{
	while(curr < size && a[curr] >= 0) curr++;
	return curr;
}


void find_zeros(float * arr, int size, float * zeros)
{
	
	int i;
	
	for(i = 0; i < size; i++)
		zeros[i] = 0.0f;
	
	for(i = 0; i < size; i++)
	{
		if(arr[i] >= 0)
		{
			i = deriv_next_negative(arr, size, i);
			if(i < size && i >= 1)
			{
				zeros[i] = 1.0f;
				
			}
			
		}
		else
		{
			i = deriv_next_positive(arr, size, i);
			if(i < size && i >= 1)
			{
				zeros[i] = 1.0;
			}
		}
	}
}


void compute_derivative_metrics(FILE * fp_sp, FILE * fp_1d, FILE * fp_2d, FILE * fp_mm, FILE * fp_if, char * name, float * waves, float * spec, int size, int wnd, float d_threshold)
{
	
	int i;
	int d_pad  = (wnd - 1) / 2;
	
	float * deriv   = (float *)malloc(sizeof(float) * size);
	float * deriv_2 = (float *)malloc(sizeof(float) * size);
	float * deriv_3 = (float *)malloc(sizeof(float) * size);
	
	for(i = 0; i < size - (wnd-1)/2; i++)
	{
		deriv[i] = deriv_2[i] = deriv_3[i] = 0.0f;	
	}
	
	for(i = d_pad; i < size - d_pad; i++)
	{
		deriv[i] = (spec[i+d_pad] - spec[i-d_pad]) / (waves[i+d_pad] - waves[i - d_pad]);
	}
	
	for(i = 2*d_pad; i < size - 2*d_pad; i++)
	{
		deriv_2[i] = (deriv[i+d_pad] - deriv[i-d_pad]) / (waves[i+d_pad] - waves[i - d_pad]);
	}
	
	/*
	for(i = 3*d_pad; i < size - 3*d_pad; i++)
	{
		deriv_3[i] = (deriv_2[i+d_pad] - deriv_2[i-d_pad]) / (waves[i+d_pad] - waves[i - d_pad]);
	}
	*/
	
	float * d1_zeros = (float *)malloc(sizeof(float) * size);
	float * d2_zeros = (float *)malloc(sizeof(float) * size);
	//float * d3_zeros = (float *)malloc(sizeof(float) * size);
	//float * d4_zeros = (float *)malloc(sizeof(float) * size);
	
	find_zeros(deriv,   size, d1_zeros);
	find_zeros(deriv_2, size, d2_zeros);
	
	// the spectrum
	fprintf(fp_sp, "%s,", name);
	for(i = 0; i < size; i++)
	{
		if(i == size-1){
			fprintf(fp_sp, "%f\n", spec[i]);
		}
		else{
			fprintf(fp_sp, "%f,",  spec[i]);
		}
	}
	
	
	// the first derivative
	fprintf(fp_1d, "%s,", name);
	for(i = 0; i < size; i++)
	{
		if(i == size-1){
			fprintf(fp_1d, "%f\n", deriv[i]);
		}
		else{
			fprintf(fp_1d, "%f,", deriv[i]);
		}
	}
	
	//the location of the 0's of the 1st derivative
	fprintf(fp_mm, "%s,", name);
	for(i = 0; i < size; i++)
	{
		if(i == size-1){
			
			if(d1_zeros[i] == 0.0f)
				fprintf(fp_mm, "1\n");
			else
				fprintf(fp_mm, "0\n");
		}
		else{
			if(d1_zeros[i] == 0.0f)
				fprintf(fp_mm, "0,");
			else
				fprintf(fp_mm, "1,");
		}
	}
	
	// the second derivative
	fprintf(fp_2d, "%s,", name);
	for(i = 0; i < size; i++)
	{
		if(i == size-1){
			fprintf(fp_2d, "%f\n", deriv_2[i]);
		}
		else{
			fprintf(fp_2d, "%f,", deriv_2[i]);
		}
	}
	
	//location of the zeros of the 2nd derivative
	fprintf(fp_if, "%s,", name);
	for(i = 0; i < size; i++)
	{
		if(i == size-1){
			
			if(d2_zeros[i] == 1.0f && abs(deriv[i]) >= d_threshold)
				fprintf(fp_if, "1\n");
			else
				fprintf(fp_if, "0\n");
		}
		else{
			if(d2_zeros[i] == 1.0f && abs(deriv[i]) >= d_threshold)
				fprintf(fp_if, "1,");
			else
				fprintf(fp_if, "0,");
		}
	}
	
	/*
	for(i = 0; i < n_d1_zeros; i++)
	{
		//find the max/min by interpolation
		
		//float m = (spec[i] - spec[i-1]) / (waves[i] - waves[i-1]);
		//float b = spec[i] - m*waves[i];
		//float w = (-1*b)/m;
				
	}
	*/
	
}










































