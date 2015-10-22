#ifndef CR_DYNAMIC_RANGE_H
#define CR_DYNAMIC_RANGE_H

int compute_point_of_interest_from_range(
	float *  spec, 
	float * waves, 
	int size, 
	float wl_low, 
	float wl_high, 
	int kernel, 
	int strategy,
	int sr_flag
);

#endif
