

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "types.h"
#include "memory.h"
#include "cr_dynamic_range.h"


/**
 * Returns the derviative of 'spec' of size 'size'-2.
 */
FloatArr * derivative(float * spec, float * waves, int size, int k)
{
	//printf("derivative\n");
	
	if(size-k+1 <= 0)
	{
		printf("Error. Range too small.\n");
		return NULL;
	}
	FloatArr * deriv = (FloatArr *)malloc(sizeof(FloatArr));
	
	deriv->size = size-(k-1);
	deriv->buff = allocf1d(deriv->size);
		if(!deriv->buff) return NULL;
		
	int i;
	for(i = 0; i < deriv->size; i++)
	{
		deriv->buff[i] = (spec[i+k-1] - spec[i]) / (waves[i+k-1] - waves[i]);
	}
	
	return deriv;
}

int next_positive(float * a, int size, int curr)
{
	while(curr < size && a[curr] < 0) curr++;
	return curr;
}

int next_negative(float * a, int size, int curr)
{
	while(curr < size && a[curr] >= 0) curr++;
	return curr;
}

/**
 * sr_flag - short range flag
 */
int get_point_of_interest(FloatArr * deriv, int strategy, int sr_flag)
{
	printf("get_point_of_interest");
	
	int i, min_i, max_i, size = deriv->size, fi_point, li_point;
	
	float min, max, * arr = deriv->buff;
	
	switch (strategy)
	{
	case 0:
		//zero
		
		min_i = 0;
		min = fabs(arr[0]);
		
		for(i = 0; i < size; i++)
		{
			if(fabs(arr[i]) < min)
			{
				min = fabs(arr[i]);
				min_i = i;
			}
		}
		
		fi_point = -1;
		li_point  = -1;
		
		//inflection points
		i = 0;
		while(i < size)
		{
			if(arr[i] >= 0)
			{	
				i = next_negative(arr, size, i);
				if(i < size)
				{	
					li_point = i;
					if(fi_point == -1) fi_point = i;
				}
			}
			else
			{
				i = next_positive(arr, size, i);
			}
		}
		
		printf(":[%d,%d,%d]\n", min_i, fi_point, li_point);
		
		if(sr_flag){
			//inflection point found
			if(li_point != -1){
				return li_point - 1;
			}
			//no inflection point found
			else{
				return min_i;
			}
		}
		else{
			//inflection point found
			if(fi_point != -1){
				return fi_point - 1;
			}
			//no inflection point found
			else{
				return min_i;
			}
		}
		
	case 1:
		//min
		puts("");
		min_i = 0;
		min = arr[0];
		
		for(i = 0; i < size; i++)
		{
			if(arr[i] < min)
			{
				min = arr[i];
				min_i = i;
			}
		}
		
		return min_i;
		
	case 2:
		//max
		puts("");
		max_i = 0;
		max = arr[0];
		
		for(i = 0; i < size; i++)
		{
			if(arr[i] > max)
			{
				max = arr[i];
				max_i = i;
			}
		}
		return max_i;
		
	default:
		return -1;
	}
}

/**
 * This function receives the whole range of spectrum.
 */
int compute_point_of_interest_from_range(
	float *  spec, 
	float * waves, 
	int size, 
	float wl_low, 
	float wl_high, 
	int kernel, 
	int strategy,
	int sr_flag)
{
	puts("compute_point_of_interest_from_range");
	
	int i = 0, poi, low, high, f_width;
	
	while(waves[i++] <= wl_low);
	low = ((waves[i] - wl_low < wl_low - waves[i-1]) ? i : i-1);
	low -= kernel/2; 												//buffer for kernel
	if(low < 0) low = 0;
	
	while(waves[i++] <= wl_high);
	high = ((waves[i] - wl_high < wl_high - waves[i-1]) ? i : i-1);
	high += kernel/2; 												//buffer for kernel
	if(high >= size) high = size-1;
	
	f_width = high - low + 1;
	
	FloatArr * deriv = derivative(spec+low, waves+low, f_width, kernel);
		if(!deriv) return -1;
		
	poi = get_point_of_interest(deriv, strategy, sr_flag);
	
	//adjust poi to reflect position in 'waves' and 'spec'
	poi += low + kernel/2; 
	
	free(deriv->buff);
	free(deriv);
	
	return poi;
}




















































