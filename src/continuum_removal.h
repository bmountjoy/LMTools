#ifndef CONTINUUM_REMOVAL_H
#define CONTINUUM_REMOVAL_H


char * continuum_removal_envi
(
	char * in_hdr_path,
	char * in_dat_path,
	char * out_dat_path,
	float wl_low,
	float wl_high		);


 char * continuum_removal_envi_dynamic
(
	char * in_hdr_path,
	char * in_dat_path,
	char * out_dat_path,
	float srange_min,
	float srange_max,
	float lrange_min,
	float lrange_max,
	int s_strat,
	int l_strat,
	int kernel);

char * continuum_removal_text
(
	FILE * fp_mt,
	FILE * fp_cr,
	char * name,
	float * waves,
	float * spec,
	int size,
	int depth_norm);

char * continuum_removal_text_dynamic
(
	FILE * fp_mt,
	FILE * fp_cr,
	float * waves,
	float * spec,
	int size,
	int f_width,
	int offset
	);

#endif
