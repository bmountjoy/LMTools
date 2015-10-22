#ifndef DERIVATIVE_METRICS_H
#define DERIVATIVE_METRICS_H

void compute_derivative_metrics(
	FILE * fp_sp, FILE * fp_1d, FILE * fp_2d, FILE * fp_mm, FILE * fp_if, 
	char * name, 
	float * waves, 
	float * spec, 
	int size, 
	int wnd, 
	float d_threshold);


#endif
