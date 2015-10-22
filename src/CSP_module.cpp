

/**
 * To compile : export MACOSX_DEPLOYMENT_TARGET=10.6
 *            : python setup.py install
 *       then : python LMTools.py
 * To build   : python setup_app.py py2app
 **/

#include <cstdlib>
#include <map>
#include "csv_io.hpp"
#include "CSP_module.hpp"

#define G_CONST	2.3548200450309494

using namespace std;

int float_compare(const void *a, const void *b)
{
	return (*(int*)a - *(int*)b);
}

void reverse(float * x, int size)
{
	int i = 0;
	float temp;
	for(i = 0; i < size/2; i++)
	{
		temp = x[i];
		x[i] = x[size-i-1];
		x[size-i-1] = temp;
	}
}


void convolve_spectra
	(
	map<float,float>& gauss_curves, 
	vector<float>& lib_spec, vector<float>& lib_wave, int n_lib_waves, 
	float * obs_wave, float * obs_fwhm, int n_obs_waves,
	float * lib_conv
	)
{
	int i,j;
	
	/**************************************************************************
	 ** Need a value for the library spectra at each observed spectra
	 ** wavelength.
	 **************************************************************************/
	
	for(i = 0; i < n_obs_waves; i++)
	{
		float stdev    = gauss_curves[obs_fwhm[i]];
		float lambda_c = obs_wave[i];
		float lambda_s = lambda_c - stdev * 5;
		float lambda_e = lambda_c + stdev * 5;
		
		
	/**************************************************************************
	 ** To close to the edge, set library spectra at the corresponding
	 ** wavelength to 0.
	 **************************************************************************/
		
		if(lambda_s < obs_wave[0] || lambda_e >= obs_wave[n_obs_waves-1])
		{
			lib_conv[i] = 0.0f;
			continue;
		}
		
	/**************************************************************************
	 ** Generate a coefficient for each library wavelength within 5 standard
	 ** deviations from the observed spectra.
	 **************************************************************************/
	 
		map<float, float> conv_const;
		
		for(j = 0; j < n_lib_waves; j++)
		{
			if(lib_wave.at(j) >= lambda_s && lib_wave.at(j) < lambda_e)
			{
				conv_const[lib_wave.at(j)] = 
				exp(-1.0 * pow(lambda_c - lib_wave.at(j), 2) / (2 * pow(stdev, 2)));
			}
		}
		
	/**************************************************************************
	 ** Normalize the coefficients as to not distort the spectra.
	 **************************************************************************/
		float sum = 0.0;
		for(map<float,float>::iterator ii=conv_const.begin(); ii!=conv_const.end(); ++ii)
		{
			sum += (*ii).second;
		}
		
		for(map<float,float>::iterator ii=conv_const.begin(); ii!=conv_const.end(); ++ii)
		{
			conv_const[(*ii).first] /= sum;
		}
		
		
	/**************************************************************************
	 ** Perform convolution.
	 **************************************************************************/
		float conv_value = 0.0;
		for(j = 0; j < n_lib_waves; j++)
		{
			if(lib_wave.at(j) >= lambda_s && lib_wave.at(j) < lambda_e)
			{
				conv_value += conv_const[lib_wave.at(j)] * lib_spec.at(j);
			}
		}
		
		lib_conv[i] = conv_value;
		
		conv_const.clear();
	}
}


const char * parse_csv_as_spectral_library(
	
	vector< vector<string> >& data, 
	vector<string>& cnames, 
	vector< vector<float> >& specs, 
	vector<float>& waves)
{
	//wavelengths
	int i,j, nrows, ncols;
	
	nrows = (int)data.size();
	ncols = (int)data.at(0).size();
	
	
	for(i = 0; i < ncols; i++)
	{
		waves.push_back(strtof(data.at(0).at(i).c_str(), NULL));
		//waves.push_back(atof(data.at(0).at(i).c_str()));
	}
	
	for(i = 1; i < nrows; i++){
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


/**
 */
static PyObject* Py_Convolve(PyObject * self, PyObject * args)
{	
	int n_obs_waves;
	char * lib_path, * obs_path, * out_path;
	PyObject* wave_list_obj;
	PyObject* fwhm_list_obj;
	PyObject* list_item;
	
	if(!PyArg_ParseTuple(args, "O!O!isss", &PyList_Type, &wave_list_obj, &PyList_Type, &fwhm_list_obj, &n_obs_waves, &lib_path, &obs_path, &out_path))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	puts("Py_Convolve");
	
	/**************************************************************************
	 ** Buffer observed wavelength and fwhm data.
	 *************************************************************************/
	float * obs_fwhm = (float*)malloc(sizeof(float) * n_obs_waves);
	float * obs_wave = (float*)malloc(sizeof(float) * n_obs_waves);
	int i,j ;
	
	for(i = 0; i < n_obs_waves; i++)
	{
		list_item = PyList_GetItem(wave_list_obj, i);
		obs_wave[i] = (float)PyFloat_AsDouble(list_item);
		
		list_item = PyList_GetItem(fwhm_list_obj, i);
		obs_fwhm[i] = (float)PyFloat_AsDouble(list_item);
	}
	
	if(obs_wave[0] > obs_wave[1])
	{
		reverse(obs_wave, n_obs_waves);
		reverse(obs_fwhm, n_obs_waves);
	}
	
	/**************************************************************************
	 ** Buffer library data.
	 *************************************************************************/
	
	printf("buffer library: %s\n", lib_path);
	                                
	vector< vector<string> > data;
	buffer_csv(lib_path, data);
	
	printf("data size: %ld\n", data.size());
	printf("data(0) size: %ld\n", data.at(0).size());
	
	vector<string> cnames;
	vector<float> lib_wave;
	vector<vector<float> > lib_spec;
	
	parse_csv_as_spectral_library(data, cnames, lib_spec, lib_wave);
	
	//todo: might have to implement reverse for library spectra aswell
	
	/**************************************************************************
	 ** Create a map of gaussian curves for each fwhm value of the observed 
	 ** data.
	 **************************************************************************/
	
	int n_lib_spec = (int)lib_spec.size();
	int n_lib_waves= (int)lib_wave.size();
	
	printf("n_lib_spec:%d\n", n_lib_spec);
	printf("n_lib_spec:%d\n", n_lib_waves);
	
	float ** lib_conv = allocf2d(n_lib_spec, n_obs_waves);
	
	map<float,float> gauss_curves;
	
	for(i = 0; i < n_obs_waves; i++)
	{
		if(gauss_curves.find(obs_fwhm[i]) == gauss_curves.end())
		{
			gauss_curves[obs_fwhm[i]] = obs_fwhm[i] / G_CONST;
		}
	}
	
	
	/**************************************************************************
	 ** Perform convolution on each library spectra.
	 **************************************************************************/
	
	for(i = 0; i < n_lib_spec; i++)
	{
		convolve_spectra(gauss_curves, lib_spec[i], lib_wave, n_lib_waves, obs_wave, obs_fwhm, n_obs_waves,	lib_conv[i]);
	}
	
	
	/**************************************************************************
	 ** Write the convolved spectra to file.
	 **************************************************************************/
	 
	FILE * fp_out = fopen(out_path, "w");
		if(!fp_out){
			PyErr_SetString(PyExc_IOError, "Error: could not open output file");
				return NULL;
		}
	
	//print header
	fprintf(fp_out, "wavelength,");
	for(i = 0; i < n_obs_waves; i++){
		if(i == n_obs_waves-1)
			fprintf(fp_out, "%f\n", obs_wave[i]);
		else
			fprintf(fp_out, "%f,", obs_wave[i]);
	}
	
	//print spectra name and spectra
	for(i = 0; i < n_lib_spec; i++)
	{
		fprintf(fp_out, "%s,", cnames.at(i).c_str());
		for(j = 0; j < n_obs_waves; j++)
		{
			if(j == n_obs_waves-1)
				fprintf(fp_out, "%f\n", lib_conv[i][j]);
			else
				fprintf(fp_out, "%f,", lib_conv[i][j]);
		}
	}
	
	fclose(fp_out);
	
	Py_INCREF(Py_None);
	return Py_None;
}





static PyMethodDef CSP_module_methods[] = {

	{"convolve", Py_Convolve, METH_VARARGS},
	{NULL, NULL}
};



PyMODINIT_FUNC
initCSP_module(void)
{
	(void) Py_InitModule("CSP_module", CSP_module_methods);
}















































