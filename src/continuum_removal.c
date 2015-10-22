



#include <string.h>
#include "memory.h"
#include "ENVIStandardIO.h"
#include "cr_dynamic_range.h"
#include "continuum_removal.h"

int N_FEATURES = 7;
char * FEATURE_NAMES [] = {
	"slope", 
	"y-int", 
	"depth",
	"depth_position",
	"area",
	"area_left",
	"area_right"};

int find_max(float * a, int size)
{
	int i, maxi;
	float max;
	
	max  = a[0];
	maxi = 0;
	
	for(i = 1; i < size; i++)
	{
		if(a[i] > max)
		{
			max  = a[i];
			maxi = i;
		}
	}
	
	return maxi;
}

float get_max(float * a, int size)
{
	float max = -999.0f;
	int i;
	
	for(i = 0; i < size; i++)
	{
		if(a[i] > max) max = a[i];
	}
	return max;
}

int find_middle_min(float * a, int size)
{
	int i, mini, count, half, seen;
	float min;
	
	min  = a[0];
	mini = 0;
	count = 0;
	
	for(i = 1; i < size; i++)
	{
		if(a[i] < min)
		{
			min  = a[i];
			mini = i;
			count = 0;
		}
		else if(a[i] == min)
		{
			count++;
		}
	}
	
	seen = 0;
	half = (count > 1) ? count / 2 : 1;
	
	for(i = 0; i < size; i++)
	{
		if(a[i] == min)
		{
			if(++seen == half)
			{
				return i;
			}
		}
	}
	return -1;
}

float poly_area(float x [], float y [], int size)
{
	int i;
	float area = 0.0;

	for(i = 0; i < size-1; i++)
	{
		area += x[i]*y[i+1] - x[i+1]*y[i];
	}
	
	area += x[size-1]*y[0] - x[0]*y[size-1];
	area /= 2.0f;
	area = (area < 0) ? area * -1.0 : area;
	
	return area;
}

float compute_left_area(float * waves, float * cr, int maxi)
{
	float max = get_max(cr, maxi+1);
	
	printf("max left area:%f\n", max);
	
	int i;
	float x_left[maxi+2], y_left[maxi+2];
	
	for(i = 0; i <= maxi; i++)
	{
		x_left[i] = waves[i];
		y_left[i] = cr[i];
	}
	
	//add final vertex
	x_left[maxi+1] = waves[maxi];
	y_left[maxi+1] = max;//1.0f;
	
	return poly_area(x_left, y_left, maxi+2);
}

/*
float compute_left_area(float * waves, float * cr, int maxi)
{
	int i;
	float x_left[maxi+2], y_left[maxi+2];
	
	for(i = 0; i <= maxi; i++)
	{
		x_left[i] = waves[i];
		y_left[i] = cr[i];
	}
	
	//add final vertex
	x_left[maxi+1] = waves[maxi];
	y_left[maxi+1] = 1.0f;
	
	return poly_area(x_left, y_left, maxi+2);
}
*/
float compute_right_area(float * waves, float * cr, int size, int maxi)
{
	float max = get_max(cr, size);
	printf("max right area:%f\n", max);
	
	
	int i;
	float x_right [size-maxi+1], y_right[size-maxi+1];
	
	x_right[0] = waves[maxi];
	y_right[0] = max;
	
	for(i = 0; i < size-maxi; i++)
	{
		x_right[i+1] = waves[i+maxi];
		y_right[i+1] = cr[i+maxi];
	}
	
	return poly_area(x_right, y_right, size-maxi+1);
}


char * process_feature
(
	float * spec,
	float * waves,
	float * cr,
	float * metrics,
	int size,
	int depth_norm)
{
	int i, bdci;
	float m, q, bdc;
	float * y;
	
	
	y = (float*)malloc(size * sizeof(float));
		if(!y) return "malloc failed";
	m = (spec[size-1] - spec[0]) / (waves[size-1] - waves[0]);
	q = spec[0] - m * waves[0];
	
	
	//compute point on the continuum for each wavelength
	for(i = 0; i < size; i++)
	{
		y[i] = m * waves[i] + q;
		
		if(spec[i] > y[i])
			spec[i] = y[i];
	}
	
	//compute continuum removal + flip
	for(i = 0; i < size; i++)
	{
		cr[i] = 1 - (spec[i] / y[i]);
	}
	
	//find bd center
	bdci = find_max(cr, size);
	bdc  = cr[bdci];
	
	//
	// either depth or area normalize the curve
	//
	if(depth_norm)
	{
		//normalize, flip, shift so curve is in [0,1]
		for(i = 0; i < size; i++)
		{
			cr[i] /= bdc;
			cr[i] = (cr[i]-1.0) * -1.0;
		}
	}
	else
	{
		float max = -1.0;
		float tot_area = poly_area(waves, cr, size);
		
		for(i = 0; i < size; i++)
		{
			cr[i] /= tot_area;
			
			if(cr[i] > max)
			{
				max = cr[i];
			}
		}
		
		for(i = 0; i < size; i++)
			cr[i] = (cr[i] * -1.0) + max;
	}
	
	//
	// compute depth, area_left, area_right, and symmetry
	//
	int mini;
	float depth, area_left, area_right;
	
	mini = find_middle_min(spec, size); 
		if(mini == -1) return "find_middle_min returned -1";
	
	depth 	   = 1.0 - spec[mini]/y[mini];
	area_left  = compute_left_area(waves, cr, bdci);
	area_right = compute_right_area(waves, cr, size, bdci);
	
	printf("mini:bdci => %d, %d\n", bdci, mini);
	
	free(y);
	
	metrics[0] = m;
	metrics[1] = q;
	metrics[2] = depth;
	metrics[3] = waves[mini];
	metrics[4] = area_left + area_right;
	metrics[5] = area_left;
	metrics[6] = area_right;
	
	
	//tests
	/*
	printf("Test:\n%f,%f,%f,%f,%d,%dn", area_left,
				area_right,
				area_left + area_right,
				poly_area(waves, cr, size),
				bdci,
				mini);
	*/
	
	return NULL;	
}

char * continuum_removal_text
(
	FILE * fp_mt,
	FILE * fp_cr,
	char * name,
	float * waves,
	float * spec,
	int size,
	int depth_norm
	)
{
	float * metrics = allocf1d(N_FEATURES);
		if(!metrics) return "allocf1d failed";
		
	float * cr = allocf1d(size);
		if(!cr) return "allocf1d failed";
		
	char * res = 
	process_feature(spec, waves, cr, metrics, size, depth_norm);
	
	if(res != NULL)
		return res;
	
	int i;
	
	fprintf(fp_cr, "%s,", name);
	for(i = 0; i < size; i++)
	{
		if(i == size-1)
		{
			fprintf(fp_cr, "%.5f\n", cr[i]);
		}
		else
		{
			fprintf(fp_cr, "%.5f,", cr[i]);
		}
	}
	
	fprintf(fp_mt, "%s,", name);
	for(i = 0; i < N_FEATURES; i++)
	{
		if(i == N_FEATURES-1)
		{
			fprintf(fp_mt, "%.5f\n", metrics[i]);
		}
		else
		{
			fprintf(fp_mt, "%.5f,", metrics[i]);
		}
	}
	
	free(cr);
	free(metrics);
	
	return NULL;
}

char * continuum_removal_text_dynamic
(
	FILE * fp_mt,
	FILE * fp_cr,
	float * waves,
	float * spec,
	int size,
	int f_width,
	int offset
	)
{
	float * metrics = allocf1d(N_FEATURES);
		if(!metrics) return "allocf1d failed";
		
	float * cr = allocf1d(size);
		if(!cr) return "allocf1d failed";
		
	int i;
	for(i = 0; i < size; i++)
		cr[i] = -1.0f;
	
	char * res = 
	process_feature(spec+offset, waves+offset, cr+offset, metrics, f_width, 1);
	
	if(res != NULL)
		return res;
	
	for(i = 0; i < size; i++)
	{
		if(i == size-1)
		{
			fprintf(fp_cr, "%.5f\n", cr[i]);
		}
		else
		{
			fprintf(fp_cr, "%.5f,", cr[i]);
		}
	}
	
	for(i = 0; i < N_FEATURES; i++)
	{
		if(i == N_FEATURES-1)
		{
			fprintf(fp_mt, "%.5f\n", metrics[i]);
		}
		else
		{
			fprintf(fp_mt, "%.5f,", metrics[i]);
		}
	}
	
	free(cr);
	free(metrics);
	
	return NULL;
}


ENVIHeader * read_envi_header(char * path)
{
	ENVIHeader * hdr;
	char * 	base, * s;
	
	base = (char*)malloc(256);
	if(!base){
		return NULL;
	}
	
	strcpy(base, path);
	s = strstr(base, ".hdr");
	base[s - base] = '\0';

	hdr = getENVIHeaderInfo(base);
		if(!hdr) return NULL;
		if(hdr->n_wavelengths != hdr->bands)
		{
			printf("%d = %d\n", hdr->n_wavelengths, hdr->bands);
			return NULL;
		}
	free(base);
		
	return hdr;
}


/*******************************************************************************
 *
 * Continuum removal with static bounds.
 *
 *******************************************************************************/
char * continuum_removal_envi
(
	char * in_hdr_path,
	char * in_dat_path,
	char * out_dat_path,
	float wl_low,
	float wl_high		)
{
	
	int i, line, band, sample, lines, bands, samples, lowi, highi, c_bands;
	float * waves, * spectrum, * cr, * metrics, *** mt_out, *** cr_out;
	unsigned short *** image;
	ENVIHeader * in_hdr;
	
	
	//
	// open envi header
	//
	in_hdr = read_envi_header(in_hdr_path);
		if(!in_hdr)	return "read_envi_header failed.";
	
	//
	// buffer input envi image
	//
	image = (unsigned short ***)readENVIFile(in_dat_path, in_hdr);
		if(!image) return "Failed buffering ENVI image.";
	
	bands = in_hdr->bands;
	lines = in_hdr->lines;
	samples = in_hdr->samples;
	waves = in_hdr->wavelengths;
	
	i = 0;
	while(waves[i++] <= wl_low);
	lowi = ((waves[i] - wl_low < wl_low - waves[i-1]) ? i : i-1);
	
	while(waves[i++] <= wl_high);
	highi = ((waves[i] - wl_high < wl_high - waves[i-1]) ? i : i-1);
	
	c_bands = highi + 1 - lowi;
	
	
	
	spectrum = allocf1d(c_bands);
		if(!spectrum) return "allocf1d failed";
	
	cr = allocf1d(c_bands);
		if(!cr) return "allocf1d failed";
		
	metrics = allocf1d(c_bands);
		if(!metrics) return "allocf1d failed";
	
	cr_out = allocf3d(c_bands, lines, samples);
		if(!cr_out) return "allocf3d failed";
		
	mt_out = allocf3d(N_FEATURES, lines, samples);
		if(!mt_out) return "allocf3d failed";
	
	printf("\tabs:[%d,%d],[%f,%f]\n",lowi,highi,waves[lowi],waves[highi]);
		
	for(line = 0; line < lines; line++)
	{
		for(sample = 0; sample < samples; sample++)
		{
			for(band = 0; band < c_bands; band++)
			{
				spectrum[band] = (float)image[lowi+band][line][sample];	
			}
			
			char * res =
			process_feature(
				spectrum, 		//spectrum of current pixel
				waves+lowi,		//wavelengths spectrum
				cr,				//continuum removal for pixel
				metrics,		//continuum removal metrics
				c_bands,		//# bands in feature
				1);
			
			if(res != NULL)
				return res;
			
			for(band = 0; band < c_bands; band++)
			{
				cr_out[band][line][sample] = cr[band];
			}
			for(band = 0; band < N_FEATURES; band++)
			{
				mt_out[band][line][sample] = metrics[band];
			}
		}
	}
	
	free(spectrum);
	free(cr);
	free(metrics);
	
	FILE * fpout = fopen(out_dat_path, "wb");
		if(!fpout) return "Could not open output file.";
	
	for(line = 0; line < lines; line++)
	{
		for(band = 0; band < c_bands; band++)
		{
			if(fwrite(cr_out[band][line], sizeof(float), samples, fpout) != samples)
			{
				return "failed to write line to ouput file";
			}
		}
		
		for(band = 0; band < N_FEATURES; band++)
		{
			if(fwrite(mt_out[band][line], sizeof(float), samples, fpout) != samples)
			{
				return "failed to write line to ouput file";
			}
		}
	}
	
	fclose(fpout);
	freef3d(cr_out, c_bands, lines);
	freef3d(mt_out, N_FEATURES, lines);
	freeus3d(image, bands, lines);
	freeENVIHeader(in_hdr);
	
	return NULL;
}



/*******************************************************************************
 *
 * Continuum removal with dynamic bounds.
 *
 *******************************************************************************/
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
	int kernel)
{
	
	int i, line, band, sample, lines, bands, samples, low, high, f_width;
	float * waves, * spectrum, * cr, * metrics, *** mt_out, *** cr_out;
	unsigned short *** image;
	ENVIHeader * in_hdr;
	
	//
	// open envi header
	//
	in_hdr = read_envi_header(in_hdr_path);
		if(!in_hdr)	return "read_envi_header failed.";
	
	//
	// buffer input envi image
	//
	image = (unsigned short ***)readENVIFile(in_dat_path, in_hdr);
		if(!image) return "readENVIFile failed.";
	
	bands = in_hdr->bands;
	lines = in_hdr->lines;
	samples = in_hdr->samples;
	waves = in_hdr->wavelengths;
	
	//
	// find absolute bounds
	//
	i = 0;
	while(waves[i++] <= srange_min);
	low = ((waves[i] - srange_min < srange_min - waves[i-1]) ? i : i-1);
	
	while(waves[i++] <= lrange_max);
	high = ((waves[i] - lrange_max < lrange_max - waves[i-1]) ? i : i-1);
	
	f_width = high + 1 - low;
	
	
	spectrum = allocf1d(bands);
		if(!spectrum) return "allocf1d failed";
	
	cr = allocf1d(f_width);
		if(!cr) return "allocf1d failed";
		
	metrics = allocf1d(N_FEATURES);
		if(!metrics) return "allocf1d failed";
	
	cr_out = allocf3d(f_width, lines, samples);
		if(!cr_out) return "allocf3d failed";
		
	mt_out = allocf3d(N_FEATURES, lines, samples);
		if(!mt_out) return "allocf3d failed";
	
		
	printf("\tabs:[%d,%d],%d\n",low,high,f_width);
		
	for(line = 0; line < lines; line++)
	{
		for(sample = 0; sample < samples; sample++)
		{
			for(band = 0; band < f_width; band++)
				cr[band] = -999.0f;
			for(band = 0; band < bands; band++)
				spectrum[band] = (float)image[band][line][sample];
			
			//
			// extract the right bands here
			//
			int curr_low = 
			compute_point_of_interest_from_range(
				spectrum, 
				waves, 
				bands, 
				srange_min, 
				srange_max, 
				kernel, 
				s_strat,
				0);
			
			if(curr_low < low){
				puts("curr_low to low");
				return "curr_low to low";
			}
			if(curr_low > high){
				puts("curr_low to high");
				return "curr_low to high";
			}
			
			int curr_high = 
			compute_point_of_interest_from_range(
				spectrum, 
				waves, 
				bands, 
				lrange_min, 
				lrange_max, 
				kernel, 
				l_strat,
				1);
			
			if(curr_high < low){
				puts("curr_high to low");
				return "curr_high to low";
			}
			if(curr_high > high){
				puts("curr_high to high");
				return "curr_high to high";
			}
			
			int curr_bands = curr_high + 1 - curr_low;
			
			printf("\tcur:[%d,%d],[%f,%f]\n", curr_low, curr_high, waves[curr_low], waves[curr_high]);
			
			//
			// only process feature between curr_low and curr_high
			//
			char * res =
			process_feature(
				spectrum + curr_low,		//spectrum of current pixel
				waves + curr_low,			//wavelengths spectrum
				cr + (curr_low - low),		//continuum removal for pixel
				metrics,					//continuum removal metrics
				curr_bands,					//# bands in feature
				1);				
			
			if(res != NULL)
				return res;
			
			for(band = 0; band < f_width; band++)
			{
				cr_out[band][line][sample] = cr[band];
			}
			for(band = 0; band < N_FEATURES; band++)
			{
				mt_out[band][line][sample] = metrics[band];
			}
		}
	}
	
	free(spectrum);
	free(cr);
	free(metrics);
	
	FILE * fpout = fopen(out_dat_path, "wb");
		if(!fpout) return "Could not open output file.";
	
	for(line = 0; line < lines; line++)
	{
		for(band = 0; band < f_width; band++)
		{
			if(fwrite(cr_out[band][line], sizeof(float), samples, fpout) != samples)
			{
				return "failed to write line to ouput file";
			}
		}
		
		for(band = 0; band < N_FEATURES; band++)
		{
			if(fwrite(mt_out[band][line], sizeof(float), samples, fpout) != samples)
			{
				return "failed to write line to ouput file";
			}
		}
	}
	
	fclose(fpout);
	freef3d(cr_out, f_width, lines);
	freef3d(mt_out, N_FEATURES, lines);
	freeus3d(image, bands, lines);
	freeENVIHeader(in_hdr);
	
	return NULL;
}







