

/**
 * To compile : export MACOSX_DEPLOYMENT_TARGET=10.6
 *            : python setup.py install
 *       then : python LMTools.py
 * To build   : python setup_app.py py2app
 **/

#include <cstdlib>
#include <cstdio>
#include <map>
#include <vector>
#include <iostream>

#include <cfloat>

#include "SM_module.hpp"

using namespace std;


void compute_extent(vector<float>& wave, float wl_low, float wl_high, int * s_idx, int * e_idx)
{
	int i, low_i, high_i;
	float min_diff_low, min_diff_high;
	
	min_diff_low = min_diff_high = FLT_MAX;
	
	for(i = 0; i < (int)wave.size(); i++)
	{
		if(fabs(wave.at(i) - wl_low) < min_diff_low)
		{
			min_diff_low = fabs(wave.at(i) - wl_low);
			low_i = i;
		}
		
		if(fabs(wave.at(i) - wl_high) < min_diff_high)
		{
			min_diff_high = fabs(wave.at(i) - wl_high);
			high_i = i;
		}
	}
	
	*s_idx = low_i;
	*e_idx = high_i;
}


int write_envi_2d_bsq(char * path, unsigned short ** image, int lines, int samples)
{
	FILE * fpout = fopen(path, "wb");
	
	if(!fpout)
	{
		PyErr_SetString(PyExc_IOError, "Error. fopen failed.");
		return 0;
	}
	
	int line;
	
	for(line = 0; line < lines; line++)
	{
		if(fwrite(image[line], sizeof(unsigned short), samples, fpout) != samples)
		{
			PyErr_SetString(PyExc_IOError, "Error. fwrite failed.");
			return 0;
		}
	}
	
	fclose(fpout);
	
	return 1;
}



int write_envi_3d_bsq(char * path, float *** image, int bands, int lines, int samples)
{
	//write this to file
	FILE * fpout = fopen(path, "wb");
	
	if(!fpout)
	{
		PyErr_SetString(PyExc_IOError, "Error. fopen failed.");
		return 0;
	}
	
	int band, line;
	
	for(band = 0; band < bands; band++)
	{
		for(line = 0; line < lines; line++)
		{
			if(fwrite(image[band][line], sizeof(float), samples, fpout) != samples)
			{
				PyErr_SetString(PyExc_IOError, "Error. fwrite failed.");
				return 0;
			}
		}
	}
	
	fclose(fpout);
	
	return 1;
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

/**
 * Returns the cosine of the angle between the input vectors
 * xx and yy.
 */
float sam_local (float * xx, float * yy, int size)
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

/**
 * Returns the pearson correlation between the vectors xx and yy.
 */
float pc_local(float * xx, float * yy, int size)
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


/**
 * Computes a pearson score for each spectra in 'obs_spec' against every
 * library spectra 'lib_spec'.
 */
void compute_per_pixel_match(vector<vector<float> > lib_spec, unsigned short *** obs_spec, float *** rule_image, int n_lib_spec, int bands, int lines, int samples, int low_i, int high_i)
{
	int spec_i, line, sample, i;
	
	int c_size = high_i - low_i + 1;
	
	for(spec_i = 0; spec_i < n_lib_spec; spec_i++)
	{
		for(line = 0; line < lines; line++)
		{
			for(sample = 0; sample < samples; sample++)
			{	
				vector<double> ospec;
				vector<double> lspec;
				
				for(i = low_i; i <= high_i; i++)
				{
					ospec.push_back( (double)obs_spec[i][line][sample] 	); 
					lspec.push_back( (double)lib_spec.at(spec_i).at(i)	);
				}
				
				float angle = pearson_correlation(ospec, lspec);

				rule_image[spec_i][line][sample] = angle;
			}
		}
	}
}


/**
 * Computes the SAM score for each spectra in 'obs_spec' against every
 * library spectra 'lib_spec'.
 */
void compute_per_pixel_SAM(vector<vector<float> > lib_spec, unsigned short *** obs_spec, float *** rule_image, int n_lib_spec, int bands, int lines, int samples, int low_i, int high_i)
{	
	int spec_i, line, sample, i;
	
	int c_size = high_i - low_i + 1;
	
	for(spec_i = 0; spec_i < n_lib_spec; spec_i++)
	{
		for(line = 0; line < lines; line++)
		{
			for(sample = 0; sample < samples; sample++)
			{	
				vector<double> ospec;
				vector<double> lspec;
				
				for(i = low_i; i <= high_i; i++)
				{
					ospec.push_back( (double)obs_spec[i][line][sample] 	); 
					lspec.push_back( (double)lib_spec.at(spec_i).at(i)	);
				}
				
				float angle = spectral_angle(ospec, lspec);
				
				rule_image[spec_i][line][sample] = angle;
			}
		}
	}
}


/**
 * Classifies each spectra in 'rule_image' and stores the result in 'csfy_image'.
 */
void classify_image(float *** rule_image, unsigned short ** csfy_image, int bands, int lines, int samples)
{
	int band, line, sample;
	
	for(line = 0; line < lines; line++)
	{
		for(sample = 0; sample < samples; sample++)
		{
			//find the index of best match
			int best_match = 0;
			float max = rule_image[0][line][sample];
			
			for(band = 0; band < bands; band++)
			{		
				if(rule_image[band][line][sample] > max){
					max = rule_image[band][line][sample];
					best_match = band;
				}
			}
			
			csfy_image[line][sample] = (unsigned short)(best_match+1); //count from 1
		}
	}
}


const char * parse_csv_as_spectral_library(
	
	vector< vector<string> >& data, 
	vector<string>& cnames, 
	vector< vector<float> >& specs, 
	vector<float>& waves)
{
	
	if (data.size() == 0)
		return "Error: data.size() == 0, no data.";
	
	int i,j, nrows, ncols;
	nrows = (int)data.size();
	ncols = (int)data.at(0).size();
	
	printf("parse_csv_as_spectral_library: ncols=%d; nrows=%d\n", ncols, nrows);
	
	
	for(i = 1; i < ncols; i++)
	{
		waves.push_back(strtof(data.at(0).at(i).c_str(), NULL));
	}
	
	for(i = 1; i < nrows; i++)
	{	
		vector<float> spec;
		for(j = 0; j < ncols; j++){
			if(j == 0)
				cnames.push_back(data.at(i).at(j));
			else
				spec.push_back(strtof(data.at(i).at(j).c_str(), NULL));
		}
		specs.push_back(spec);
	}
	
	return NULL;
}


static PyObject* Py_SpectralMetrics_ENVI(PyObject * self, PyObject * args)
{	
	int run_sam, run_match, n_lib_samples;
	float wl_low, wl_high;
	char * lib_spec_path, * obs_base_path, * rule_path, * csfy_path, * match_rule_path, * match_csfy_path;
	PyObject* thresh_list_obj;
	PyObject* thresh_list_item;
	
	if(!PyArg_ParseTuple(args, "ssssssffiiO!i", 
		&lib_spec_path, 
		&obs_base_path, 
		&rule_path, 
		&csfy_path, 
		&match_rule_path, 
		&match_csfy_path, 
		&wl_low, 
		&wl_high,
		&run_sam,
		&run_match,
		&PyList_Type,
		&thresh_list_obj,
		&n_lib_samples
		))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	puts("Py_SpectralMetrics_ENVI");
	
	const char * ret;
	
	int i;
	float class_thr [n_lib_samples];
	for(i = 0; i < n_lib_samples; i++)
	{
		thresh_list_item = PyList_GetItem(thresh_list_obj, i);
		class_thr[i] = (float)PyFloat_AsDouble(thresh_list_item);
		//printf("%f\n", class_thr[i]);
	}
	
	/*************************************************************************
	 ** Buffer the library spectra
	 *************************************************************************/
	 
	vector<string> lib_cnames;
	vector< vector<float> > lib_spec;
	vector<float> lib_wave;
	
	vector< vector<string> > data;
	ret = buffer_csv(lib_spec_path, data);
	
		if(ret != NULL){
			PyErr_SetString(PyExc_IOError, ret);
			return NULL;
		}
		
	parse_csv_as_spectral_library(data, lib_cnames, lib_spec, lib_wave);
	
	int n_lib_wave = (int)lib_wave.size();
	int n_lib_spec = (int)lib_spec.size();
	
	
	/*************************************************************************
	 ** Buffer the observed spectra
	 *************************************************************************/
	 
	int rows, cols, bands;
	char obs_dat_path [512];
	unsigned short *** obs_spec;
	
	ENVIHeader * obs_hdr = getENVIHeaderInfo(obs_base_path);
	
		if(obs_hdr->datatype != 12){
			PyErr_SetString(PyExc_ValueError, "Datatype of observed spectra must be 12.");
			return NULL;
		}
	
	sprintf(obs_dat_path, "%s%s", obs_base_path, ".dat");
	obs_spec = (unsigned short ***)readENVIFile(obs_dat_path, obs_hdr);
	
	rows  = obs_hdr->lines;
	cols  = obs_hdr->samples;
	bands = obs_hdr->bands;
	
	
	/*************************************************************************
	 ** Ensure the number of wavelengths for each library and observed spectra
	 ** are the same.
	 *************************************************************************/
	 
	if(bands != n_lib_wave)
	{
		PyErr_SetString(PyExc_IOError, "Error. # of bands in observed wavelengths != # of wavelengths in library spectra");
		return NULL;
	}
	
	int low_i, high_i, c_size;
	compute_extent(lib_wave, wl_low, wl_high, &low_i, &high_i);
	c_size = high_i - low_i + 1;
	
	if(c_size <= 1)
	{
		PyErr_SetString(PyExc_ValueError, "Error: bad range.");
		return NULL;
	}
	
	
	/*************************************************************************
	 ** Allocate output buffers
	 *************************************************************************/
	 
	float *** rule_image = allocf3d(n_lib_spec,rows,cols);
	
		if(!rule_image)
		{
			PyErr_SetString(PyExc_ValueError, "Error. allocus3d failed.");
			return NULL;
		}
		
	unsigned short **  csfy_image = allocH2d(rows,cols);
	
		if(!csfy_image)
		{
			PyErr_SetString(PyExc_ValueError, "Error. allocus3d failed.");
			return NULL;
		}
	
	
	/*************************************************************************
	 ** Run SAM and Matching
	 *************************************************************************/
	 
	if(run_sam)
	{
		compute_per_pixel_SAM(lib_spec, obs_spec, rule_image, n_lib_spec, bands, rows, cols, low_i, high_i);
		
		classify_image(rule_image,  csfy_image, n_lib_spec, rows, cols);
		
		if(!write_envi_3d_bsq(rule_path, rule_image, n_lib_spec, rows, cols))
			return NULL;
	
		if(!write_envi_2d_bsq(csfy_path, csfy_image, rows, cols))
			return NULL;
	
	}

	if(run_match)
	{
		compute_per_pixel_match(lib_spec, obs_spec, rule_image, n_lib_spec, bands, rows, cols, low_i, high_i);
		
		classify_image(rule_image,  csfy_image, n_lib_spec, rows, cols);
		
		if(!write_envi_3d_bsq(match_rule_path, rule_image, n_lib_spec, rows, cols))
			return NULL;
	
		if(!write_envi_2d_bsq(match_csfy_path, csfy_image, rows, cols))
			return NULL;
	}
	
	
	//
	// free memory
	//
	freeENVIHeader(obs_hdr);
	freeus3d(obs_spec, bands, rows);
	freef3d(rule_image, n_lib_spec, rows);
	freeus2d(csfy_image, rows);
	
	return Py_None;
}


int min_index(vector<float>& x)
{
	int   i, mini = 0;
	float min = FLT_MAX;
	
	for(i = 0; i < x.size(); i++){
		if(min > x.at(i)){
			min  = x.at(i);
			mini = i;
		}	
	}
	return mini;
}


int max_index(vector<float>& x)
{
	int   i, maxi = 0;
	float max = FLT_MIN;
	
	for(i = 0; i < x.size(); i++){
		if(max < x.at(i)){
			max  = x.at(i);
			maxi = i;
		}	
	}
	return maxi;
}


const char * parse_csv_as_observed_spectra(
	
	vector< vector<string> >& data,
	string& obs_shdr,
	vector<string>& obs_sname,
	vector<float>& wave, 
	vector< vector<float> >& spec,
	int col_to_skip,
	int col_as_id)

{
	if (data.size() == 0)
		return "Error: data.size() == 0";
	
	int i, j, nrows, ncols;
	
	nrows = (int)data.size();
	ncols = (int)data.at(0).size();
	
	obs_shdr = data.at(0).at(col_as_id);
	
	for(i = col_to_skip; i < ncols; i++){
		wave.push_back(strtof(data.at(0).at(i).c_str(), NULL));
	}
	
	for(i = 1; i < nrows; i++)
	{
		obs_sname.push_back(data.at(i).at(col_as_id));
		
		vector<float> t;
		for(j = col_to_skip; j < ncols; j++)
		{
			t.push_back(strtof(data.at(i).at(j).c_str(), NULL));
		}
		spec.push_back(t);
	}
	
	return NULL;
}


char * is_error(
	
	vector<string>& lib_cname, 
	vector<float>& lib_wave, 
	vector<float>& obs_wave,
	vector< vector<float> >& lib_spec,
	vector< vector<float> >& obs_spec)
{
	char * err = (char*)malloc(512);
	
	if( lib_wave.size() != obs_wave.size() ){
		sprintf(err, "Observed and library spectra have a different number of wavelengths: %ld, %ld", obs_wave.size(), lib_wave.size());
		return err;
	}
	
	int i;
	for(i = 0; i < lib_wave.size(); i++){
		if(lib_wave.at(i) != obs_wave.at(i)){
			sprintf(err, "Library and observed spectra have different wavelength values: %f != %f.", lib_wave.at(i), obs_wave.at(i));
			return err;
		}
	}
	
	if(lib_spec.size() == 0){
		sprintf(err, "No library spectra");
		return err;
	}
	
	if(obs_spec.size() == 0){
		sprintf(err, "No observed spectra");
		return err;
	}
	
	if(lib_cname.size() != lib_spec.size()){
		sprintf(err, "Different number of library classes and library spectra: %ld %ld", lib_cname.size(), lib_spec.size());
		return err;
	}
	
	int n_wave = (int)lib_wave.size();
	
	for(i = 0; i < lib_spec.size(); i++){
		int size = (int)lib_spec.at(i).size();
		if(size != n_wave){
			sprintf(err, "Number of library spectra not consistent with number of wavelengths: %d %d", size, n_wave);
			return err;
		}
	}
	
	for(i = 0; i < obs_spec.size(); i++){
		int size = (int)obs_spec.at(i).size();
		if(size != n_wave){
			sprintf(err, "Number of observed spectra not consistent with number of wavelengths: %d %d", size, n_wave);
			return err;
		}
	}

	return NULL;	
}


const char * run_sam_csv(
	
	char * sam_path,
	string& obs_shdr,
	vector<string>& lib_cnames,
	vector<string>& obs_snames,
	vector<vector<double> >& lib_spec, int n_lib_spec, //won't make a copy
	vector<vector<double> >& obs_spec, int n_obs_spec,
	float class_thr [])
{
	int i, j;
	
	FILE * fp = fopen(sam_path, "w");
	if(!fp) return "run_sam_csv error: couldn't open output file";
	
	//
	//write header
	//
	fprintf(fp, "%s,", obs_shdr.c_str());
	for(i = 0; i < n_lib_spec; i++)
	{
		fprintf(fp, "%s,", lib_cnames.at(i).c_str());
	}
	fprintf(fp, "Best_Match\n");
	
	
	//
	//write scores
	//
	vector<float> scores;
	
	for(i = 0; i < n_obs_spec; i++)
	{
		for(j = 0; j < n_lib_spec; j++)
		{
			scores.push_back(spectral_angle(obs_spec.at(i), lib_spec.at(j)));
		}
		
		//write classification to file
		fprintf(fp, "%s,", obs_snames.at(i).c_str());
			
		for(j = 0; j < n_lib_spec; j++){
			fprintf(fp, "%f," , scores.at(j));
		}
		
		int maxi = max_index(scores);
		if(scores.at(maxi) > class_thr[maxi])
			fprintf(fp, "%s\n", lib_cnames.at(maxi).c_str());
		else
			fprintf(fp, "none\n");
		
		scores.clear();
	}
	fclose(fp);
	
	return NULL;
}


const char * run_match_csv(
	
	char * match_path,
	string& obs_shdr,
	vector<string>& lib_cnames,
	vector<string>& obs_snames,
	vector<vector<double> >& lib_spec, int n_lib_spec, //won't make a copy
	vector<vector<double> >& obs_spec, int n_obs_spec,
	float class_thr [])
{
	int i, j;
	
	FILE * fp = fopen(match_path, "w");
	if(!fp) return "run_match_csv error: couldn't open output file";
	
	//
	// write header
	//
	fprintf(fp, "%s,", obs_shdr.c_str());
	for(i = 0; i < n_lib_spec; i++)
	{
		fprintf(fp, "%s,", lib_cnames.at(i).c_str());
	}
	fprintf(fp, "Best_Match\n");
	
	
	//
	// write scores
	//
	vector<float> scores;
	
	for(i = 0; i < n_obs_spec; i++)
	{
		for(j = 0; j < n_lib_spec; j++)
		{
			scores.push_back(pearson_correlation(obs_spec.at(i), lib_spec.at(j)));
		}
		
		//write classification to file
		fprintf(fp, "%s,", obs_snames.at(i).c_str());
			
		for(j = 0; j < n_lib_spec; j++){
			fprintf(fp, "%f," , scores.at(j));
		}
		
		int maxi = max_index(scores);
		
		if(scores.at(maxi) > class_thr[maxi])
			fprintf(fp, "%s\n", lib_cnames.at(maxi).c_str());
		else
			fprintf(fp, "none\n");
		
		scores.clear();
	}
	fclose(fp);
	
	return NULL;
}


static PyObject* Py_SpectralMetrics_CSV(PyObject * self, PyObject * args)
{	
	int run_sam, run_match, n_samples, col_to_skip, col_as_id;
	float wl_low, wl_high;
	char * lib_spec_path, * obs_path, * sam_path, * match_path;
	PyObject* thresh_list_obj;
	PyObject* thresh_list_item;
	
	if(!PyArg_ParseTuple(args, "ssssffiiO!iii", 
		&lib_spec_path, 
		&obs_path, 
		&sam_path, 
		&match_path,
		&wl_low, 
		&wl_high,
		&run_sam,
		&run_match,
		&PyList_Type,
		&thresh_list_obj,
		&n_samples,
		&col_to_skip,
		&col_as_id))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	puts("Py_SpectralMetrics_CSV");
	
	int i,j;
	float class_thr [n_samples];
	
	for(i = 0; i < n_samples; i++)
	{
		thresh_list_item = PyList_GetItem(thresh_list_obj, i);
		class_thr[i] = (float)PyFloat_AsDouble(thresh_list_item);
		
		//printf("%f\n", class_thr[i]);
	}
	
	char * err = (char*)malloc(512);
	const char * ret;
	
	
	/*************************************************************************
	 ** Buffer library spectra
	 *************************************************************************/
	
	vector<string> lib_cnames;
	vector< vector<float> > lib_spec;
	vector<float> lib_wave;
	
	vector< vector<string> > data;
	ret = buffer_csv(lib_spec_path, data);
	
		if(ret != NULL){
			PyErr_SetString(PyExc_IOError, ret);
			return NULL;
		}

	parse_csv_as_spectral_library(data, lib_cnames, lib_spec, lib_wave);

	int n_lib_spec = lib_spec.size();
	
	if(n_lib_spec != n_samples){
		sprintf(err,"n lib spec(%d) != n samples error(%d)\n", n_lib_spec, n_samples);
		PyErr_SetString(PyExc_IOError, err);
		return NULL;
	}
	
	
	/*************************************************************************
	 ** Buffer observed spectra
	 *************************************************************************/
	 
	data.clear();
	ret = buffer_csv(obs_path, data);
	
		if(ret != NULL){
			PyErr_SetString(PyExc_IOError, ret);
			return NULL;
		}
	
	string obs_shdr;
	vector<string> obs_sname;
	vector<float> obs_wave;
	vector< vector<float> > obs_spec;
	
	puts("buffer observed");
	
	parse_csv_as_observed_spectra(data, obs_shdr, obs_sname, obs_wave, obs_spec, col_to_skip, col_as_id);

	int n_obs_spec = obs_spec.size();
	
	
	/*************************************************************************
	 ** Check for errors.
	 *************************************************************************/
	 
	if((ret = is_error(lib_cnames, lib_wave, obs_wave, lib_spec, obs_spec)) != NULL)
	{
		sprintf(err, "Error: %s\n", ret);
		PyErr_SetString(PyExc_IOError, ret);
		return NULL;
	}
	
	
	/*************************************************************************
	 ** Compute extent of comparison range and clip spectra to that range.
	 *************************************************************************/
	
	int low_i, high_i;
	compute_extent(lib_wave, wl_low, wl_high, &low_i, &high_i);
	
	if(high_i - low_i <= 1)
	{
		sprintf(err, "Error: bad wavelength range [%d, %d]", low_i, high_i);
		PyErr_SetString(PyExc_IOError, err);
		return NULL;
	}
	
	printf("lowi: %d, highi: %d\n", low_i, high_i);
	
	//trim
	vector< vector<double> > copy_lib_spec;
	vector< vector<double> > copy_obs_spec;
	
	for(i = 0; i < n_lib_spec; i++){
		vector<double> t;
		for(j = low_i; j <= high_i; j++){
			t.push_back((double)lib_spec.at(i).at(j));
		}
		copy_lib_spec.push_back(t);
	}
	for(i = 0; i < n_obs_spec; i++){
		vector<double> t;
		for(j = low_i; j <= high_i; j++){
			t.push_back((double)obs_spec.at(i).at(j));
		}
		copy_obs_spec.push_back(t);
	}
	
	
	/*************************************************************************
	 ** Run PC and SAM
	 *************************************************************************/
	
	puts("run sam");
	if(run_sam)
	{	
		run_sam_csv(sam_path, obs_shdr, lib_cnames, obs_sname, copy_lib_spec, n_lib_spec, copy_obs_spec, n_obs_spec, class_thr);
	}

	puts("run match");
	if(run_match)
	{
		run_match_csv(match_path, obs_shdr, lib_cnames, obs_sname, copy_lib_spec, n_lib_spec, copy_obs_spec, n_obs_spec, class_thr);
	}
	
	free(err);
	
	return Py_None;
}


/*************************************************************************
 **
 ** SM_MODULE METHOD DEFINITIONS FOR PYTHON EXTENSION
 **
 *************************************************************************/

static PyMethodDef SM_module_methods[] = {

	{"SpectralMetrics", Py_SpectralMetrics_ENVI, METH_VARARGS},
	{"SpectralMetrics_CSV", Py_SpectralMetrics_CSV, METH_VARARGS},
	{NULL, NULL}
};

PyMODINIT_FUNC
initSM_module(void)
{
	(void) Py_InitModule("SM_module", SM_module_methods);
}















































