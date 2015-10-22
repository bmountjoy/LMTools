

/**
 * To compile : export MACOSX_DEPLOYMENT_TARGET=10.6 ?? not sure about this line anymore
 *            : python setup.py install
 *       then : python LMTools.py
 * To build   : python setup_app.py py2app
 **/
 
#include <Python/Python.h>

#include <stdio.h>
#include "types.h"
#include "memory.h"
#include "cr_dynamic_range.h"
#include "continuum_removal.h"
#include "CR_module.h"

	
FILE * FP_MT_TEXT_OUT;
FILE * FP_CR_TEXT_OUT;

FILE * FP_SP_TEXT_OUT;
FILE * FP_1D_TEXT_OUT;
FILE * FP_2D_TEXT_OUT;
FILE * FP_MINMAX_TEXT_OUT;
FILE * FP_INFLECT_TEXT_OUT;

static PyObject* Py_cr_dynamic_range(PyObject * self, PyObject * args)
{	
	int size, s_strat, l_strat, kernel;
	float srange_min, srange_max, lrange_min, lrange_max;
	
	PyObject* spec_list_obj;
	PyObject* wave_list_obj;
	PyObject* spec_list_item, * wave_list_item;

	if(!PyArg_ParseTuple(args, "O!O!iiiffffi", &PyList_Type, &spec_list_obj, &PyList_Type, &wave_list_obj, &size,
		&s_strat, &l_strat, &srange_min, &srange_max, &lrange_min, &lrange_max, &kernel))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	//handle python list objects
	int i;
	float * spec = allocf1d(size);
	if(!spec)
	{
		puts("malloc failed.");
	}
	float * wave = allocf1d(size);
	if(!wave)
	{
		puts("malloc failed.");
	}
	
	for(i = 0; i < size; i++)
	{
		spec_list_item = PyList_GetItem(spec_list_obj, i);
		spec[i] = (float)PyFloat_AsDouble(spec_list_item);
		
		wave_list_item = PyList_GetItem(wave_list_obj, i);
		wave[i] = (float)PyFloat_AsDouble(wave_list_item);
	}
	
	printf("\tsr_min:%f, sr_max:%f\n", srange_min, srange_max);
	int low_i = 
	compute_point_of_interest_from_range(
		spec, 
		wave, 
		size, 
		srange_min, 
		srange_max, 
		kernel, 
		s_strat,
		1);
	
	if(low_i == -1){
		PyErr_SetString(PyExc_Exception, "compute_point_of_interest_from_range failed.");
		return NULL;
	}
	
	printf("\tlr_min:%f, lr_max:%f\n", lrange_min, lrange_max);
	int high_i = 
	compute_point_of_interest_from_range(
		spec, 
		wave, 
		size, 
		lrange_min, 
		lrange_max, 
		kernel, 
		l_strat,
		0);
	
	if(high_i == -1){
		PyErr_SetString(PyExc_Exception, "compute_point_of_interest_from_range failed.");
		return NULL;
	};
	
	return Py_BuildValue("ii", low_i, high_i);
}


static PyObject* Py_cr_text_setup(PyObject * self, PyObject * args)
{	
	int n_waves;
	char * mt_out_path, *cr_out_path;
	PyObject* wave_list_obj;
	PyObject* wave_list_item;

	if(!PyArg_ParseTuple(args, "O!iss", &PyList_Type, &wave_list_obj, &n_waves, &mt_out_path, &cr_out_path))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	puts("Py_cr_text_setup");
	
	int i;
	float * waves = allocf1d(n_waves);
	if(!waves)
	{
		puts("malloc failed.");
	}
	
	for(i = 0; i < n_waves; i++)
	{
		wave_list_item = PyList_GetItem(wave_list_obj, i);
		waves[i] = (float)PyFloat_AsDouble(wave_list_item);
	}
	
	FP_MT_TEXT_OUT = fopen(mt_out_path, "w");
	if(!FP_MT_TEXT_OUT)
	{
		PyErr_SetString(PyExc_IOError, "Couldn't open metrics output file");
	}
	
	FP_CR_TEXT_OUT = fopen(cr_out_path, "w");
	if(!FP_CR_TEXT_OUT)
	{
		PyErr_SetString(PyExc_IOError, "Couldn't open continuum removal output file");
	}
	
	//hacks!!!
	int N_FEATURES = 7;
	char * FEATURE_NAMES [] = {
		"slope", 
		"y-int", 
		"depth",
		"depth_position",
		"area",
		"area_left",
		"area_right"};
	
	fprintf(FP_MT_TEXT_OUT, " ,");
	for(i = 0; i < N_FEATURES; i++)
	{
		if(i == N_FEATURES-1)
		{
			fprintf(FP_MT_TEXT_OUT, "%s\n", FEATURE_NAMES[i]);
		}
		else
		{
			fprintf(FP_MT_TEXT_OUT, "%s,", FEATURE_NAMES[i]);
		}
	}
	
	fprintf(FP_CR_TEXT_OUT, " ,");
	for(i = 0; i < n_waves; i++)
	{
		if(i == n_waves-1)
		{
			fprintf(FP_CR_TEXT_OUT, "%.2f\n", waves[i]);
		}
		else
		{
			fprintf(FP_CR_TEXT_OUT, "%.2f,", waves[i]);
		}
	}
	
	return Py_None;
}


/**
 * spec_list - the reflectance values
 * wave_list - the corresponding wavelengths
 * size - size of the list
 */
static PyObject* Py_cr_text(PyObject * self, PyObject * args)
{	
	int size, depth_norm;
	char * spec_name;
	PyObject* spec_list_obj;
	PyObject* wave_list_obj;
	PyObject* spec_list_item, * wave_list_item;

	if(!PyArg_ParseTuple(args, "O!O!isi", 
		&PyList_Type, &spec_list_obj, 
		&PyList_Type, &wave_list_obj, 
		&size, 
		&spec_name, 
		&depth_norm))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	puts("Py_cr_text");
	
	int i;
	float * spec = allocf1d(size);
	if(!spec)
	{
		puts("malloc failed.");
	}
	float * wave = allocf1d(size);
	if(!wave)
	{
		puts("malloc failed.");
	}
	
	for(i = 0; i < size; i++)
	{
		spec_list_item = PyList_GetItem(spec_list_obj, i);
		spec[i] = (float)PyFloat_AsDouble(spec_list_item);
		
		wave_list_item = PyList_GetItem(wave_list_obj, i);
		wave[i] = (float)PyFloat_AsDouble(wave_list_item);
	}
	
	char * res = 
	continuum_removal_text(FP_MT_TEXT_OUT, FP_CR_TEXT_OUT, spec_name, wave, spec, size, depth_norm);
	
	free(spec);
	free(wave);
	
	if(res != NULL)
	{
		PyErr_SetString(PyExc_IOError, res);
		return NULL;
	}
	
	return Py_None;
}

static PyObject* Py_cr_text_dynamic(PyObject * self, PyObject * args)
{	
	int size, f_width, offset;
	PyObject* spec_list_obj;
	PyObject* wave_list_obj;
	PyObject* spec_list_item, * wave_list_item;

	if(!PyArg_ParseTuple(args, "O!O!iii", &PyList_Type, &spec_list_obj, &PyList_Type, &wave_list_obj, &size, &f_width, &offset))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	puts("Py_cr_text");
	
	int i;
	float * spec = allocf1d(size);
	if(!spec)
	{
		puts("malloc failed.");
	}
	float * wave = allocf1d(size);
	if(!wave)
	{
		puts("malloc failed.");
	}
	
	for(i = 0; i < size; i++)
	{
		spec_list_item = PyList_GetItem(spec_list_obj, i);
		spec[i] = (float)PyFloat_AsDouble(spec_list_item);
		
		wave_list_item = PyList_GetItem(wave_list_obj, i);
		wave[i] = (float)PyFloat_AsDouble(wave_list_item);
	}
	
	char * res = 
	continuum_removal_text_dynamic(FP_MT_TEXT_OUT, FP_CR_TEXT_OUT, wave, spec, size, f_width, offset);
	
	free(spec);
	free(wave);
	
	if(res != NULL)
	{
		PyErr_SetString(PyExc_IOError, res);
		return NULL;
	}
	
	return Py_None;
}

static PyObject* Py_cr_text_teardown(PyObject * self, PyObject * args)
{	
	fclose(FP_MT_TEXT_OUT);
	fclose(FP_CR_TEXT_OUT);
	
	return Py_None;
}


static PyObject* Py_cr_envi(PyObject * self, PyObject * args)
{
	float wl_low, wl_high;
	char * in_hdr_path, * in_dat_path, * out_dat_path;
	
	if(!PyArg_ParseTuple(args, "sssff", &in_hdr_path, &in_dat_path, &out_dat_path, &wl_low, &wl_high))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	char * status = 
	continuum_removal_envi(in_hdr_path, in_dat_path, out_dat_path, wl_low, wl_high);
	
	if(status != NULL)
	{
		PyErr_SetString(PyExc_IOError, status);
		return NULL;
	}
	return Py_None;
}

static PyObject* Py_cr_envi_dynamic(PyObject * self, PyObject * args)
{
	int s_strat, l_strat, kernel;
	float srange_min, srange_max, lrange_min, lrange_max;
	char * in_hdr_path, * in_dat_path, * out_dat_path;
	
	if(!PyArg_ParseTuple(args, "sssffffiii", 
		&in_hdr_path, 
		&in_dat_path, 
		&out_dat_path,
		&srange_min,
		&srange_max,
		&lrange_min,
		&lrange_max,
		&s_strat,
		&l_strat,
		&kernel))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	char * status = 
	continuum_removal_envi_dynamic(
		in_hdr_path, 
		in_dat_path, 
		out_dat_path,
		srange_min,
		srange_max,
		lrange_min,
		lrange_max,
		s_strat,
		l_strat,
		kernel);
	
	if(status != NULL)
	{
		PyErr_SetString(PyExc_IOError, status);
		return NULL;
	}
	return Py_None;
}


char * open_deriv_text_output_files(char * sp_out_path, char * d1_out_path, char * d2_out_path, char * mm_out_path, char * if_out_path)
{
	char * err_msg = (char *)malloc(256);
	
	FP_SP_TEXT_OUT = fopen(sp_out_path, "w");
	
	if(!FP_SP_TEXT_OUT){
		sprintf(err_msg, "Error. Couldn't open file %s\n", sp_out_path); 
		return err_msg;
	}
	
	FP_1D_TEXT_OUT = fopen(d1_out_path, "w");
	
	if(!FP_1D_TEXT_OUT){
		sprintf(err_msg, "Error. Couldn't open file %s\n", d1_out_path); 
		return err_msg;
	}
	
	FP_2D_TEXT_OUT = fopen(d2_out_path, "w");
	
	if(!FP_2D_TEXT_OUT){
		sprintf(err_msg, "Error. Couldn't open file %s\n", d2_out_path); 
		return err_msg;
	}
	
	FP_MINMAX_TEXT_OUT = fopen(mm_out_path, "w");
	
	if(!FP_MINMAX_TEXT_OUT){
		sprintf(err_msg, "Error. Couldn't open file %s\n", mm_out_path); 
		return err_msg;
	}
	
	FP_INFLECT_TEXT_OUT = fopen(if_out_path, "w");
	
	if(!FP_INFLECT_TEXT_OUT){
		sprintf(err_msg, "Error. Couldn't open file %s\n", if_out_path); 
		return err_msg;
	}
	
	return NULL;
}



/**
 * Only purpose is to open the file that will store the derivative metrics.
 */
static PyObject* Py_deriv_text_setup(PyObject * self, PyObject * args)
{	
	int n_waves;
	char * sp_out_path, * d1_out_path, * d2_out_path, * mm_out_path, * if_out_path;
	PyObject* wave_list_obj;
	PyObject* wave_list_item;

	if(!PyArg_ParseTuple(args, "O!isssss", &PyList_Type, &wave_list_obj, &n_waves, &sp_out_path, &d1_out_path, &d2_out_path, &mm_out_path, &if_out_path))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	puts("Py_deriv_text_setup");
	
	int i;
	float * waves = allocf1d(n_waves);
	if(!waves)
	{
		puts("malloc failed.");
	}
	
	for(i = 0; i < n_waves; i++)
	{
		wave_list_item = PyList_GetItem(wave_list_obj, i);
		waves[i] = (float)PyFloat_AsDouble(wave_list_item);
	}
	
	char * msg = open_deriv_text_output_files(sp_out_path, d1_out_path, d2_out_path, mm_out_path, if_out_path);
		if(msg != NULL){
			PyErr_SetString(PyExc_IOError, msg);
			return NULL;
		}

	fprintf(FP_SP_TEXT_OUT, ",");
	fprintf(FP_1D_TEXT_OUT, ",");
	fprintf(FP_2D_TEXT_OUT, ",");
	fprintf(FP_MINMAX_TEXT_OUT, ",");
	fprintf(FP_INFLECT_TEXT_OUT, ",");
	
	for(i = 0; i < n_waves; i++){
		if(i == n_waves - 1){
			fprintf(FP_SP_TEXT_OUT, "%f\n", waves[i]);
			fprintf(FP_1D_TEXT_OUT, "%f\n", waves[i]);
			fprintf(FP_2D_TEXT_OUT, "%f\n", waves[i]);
			fprintf(FP_MINMAX_TEXT_OUT, "%f\n", waves[i]);
			fprintf(FP_INFLECT_TEXT_OUT, "%f\n", waves[i]);
		}
		else{
			fprintf(FP_SP_TEXT_OUT, "%f,", waves[i]);
			fprintf(FP_1D_TEXT_OUT, "%f,", waves[i]);
			fprintf(FP_2D_TEXT_OUT, "%f,", waves[i]);
			fprintf(FP_MINMAX_TEXT_OUT, "%f,", waves[i]);
			fprintf(FP_INFLECT_TEXT_OUT, "%f,", waves[i]);
		}
	}
	
	return Py_None;
}


static PyObject* Py_deriv_text_static(PyObject * self, PyObject * args)
{	
	int size, window;
	float threshold;
	char * name;
	PyObject* spec_list_obj;
	PyObject* wave_list_obj;
	PyObject* spec_list_item, * wave_list_item;

	if(!PyArg_ParseTuple(args, "O!O!isif", &PyList_Type, &spec_list_obj, &PyList_Type, &wave_list_obj, &size, &name, &window, &threshold))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	puts("Py_cr_text");
	
	int i;
	float * spec = allocf1d(size);
	if(!spec)
	{
		puts("malloc failed.");
	}
	float * wave = allocf1d(size);
	if(!wave)
	{
		puts("malloc failed.");
	}
	
	for(i = 0; i < size; i++)
	{
		spec_list_item = PyList_GetItem(spec_list_obj, i);
		spec[i] = (float)PyFloat_AsDouble(spec_list_item);
		
		wave_list_item = PyList_GetItem(wave_list_obj, i);
		wave[i] = (float)PyFloat_AsDouble(wave_list_item);
	}
	
	compute_derivative_metrics(FP_SP_TEXT_OUT, FP_1D_TEXT_OUT, FP_2D_TEXT_OUT, FP_MINMAX_TEXT_OUT, FP_INFLECT_TEXT_OUT, name, wave, spec, size, window, threshold);
	
	free(spec);
	free(wave);
	
	return Py_None;
}


static PyObject* Py_deriv_text_teardown(PyObject * self, PyObject * args)
{	
	fclose(FP_SP_TEXT_OUT);
	fclose(FP_1D_TEXT_OUT);
	fclose(FP_2D_TEXT_OUT);
	fclose(FP_MINMAX_TEXT_OUT);
	fclose(FP_INFLECT_TEXT_OUT);
	return Py_None;
}


static PyMethodDef CR_module_methods[] = {
	
	{"cr_dynamic_range", Py_cr_dynamic_range, METH_VARARGS},
	{"cr_text_setup", Py_cr_text_setup, METH_VARARGS},
	{"cr_text", Py_cr_text, METH_VARARGS},
	{"cr_text_dynamic", Py_cr_text_dynamic, METH_VARARGS},
	{"cr_text_teardown", Py_cr_text_teardown, METH_VARARGS},
	{"cr_envi", Py_cr_envi, METH_VARARGS},
	{"cr_envi_dynamic", Py_cr_envi_dynamic, METH_VARARGS},
	{"deriv_text_setup", Py_deriv_text_setup, METH_VARARGS},
	{"deriv_text_static", Py_deriv_text_static, METH_VARARGS},
	{"deriv_text_teardown", Py_deriv_text_teardown, METH_VARARGS},
	{NULL, NULL}
};

PyMODINIT_FUNC 
initCR_module(void)
{
	(void) Py_InitModule("CR_module", CR_module_methods);
}
