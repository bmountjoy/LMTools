

/**
 * To compile : export MACOSX_DEPLOYMENT_TARGET=10.6
 *            : python setup.py install
 *       then : python LMTools.py
 * To build   : python setup_app.py py2app
 **/

#include <Python/Python.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "FLD_module.h"

/**
 * Creates a vector of the given flight line.
 */
static PyObject* Py_DelineateFlightLine(PyObject * self, PyObject * args)
{
	puts("Py_DelineateFlightLine");
	
	int band_index;
	char * fl_basename, * fl_filepath, * ov_filepath;
	
	if(!PyArg_ParseTuple(args, "sssi", &fl_basename, &fl_filepath, &ov_filepath, &band_index))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	printf("%s\n%s\n", fl_filepath, ov_filepath);
	
	int i, j, n_lines, n_samples;
	double ul_easting, ul_northing;
	unsigned short ** image;
	char ** mask;
	SHPObject * contour;
	SHPHandle HSHP_flightline;
	DBFHandle HDBF_flightline;
	
	ENVIHeader *
	envi_hdr = getENVIHeaderInfo(fl_filepath);
	if(!envi_hdr){
		return NULL;
	}
	
	if(envi_hdr->datatype != 12)
	{
		PyErr_SetString(PyExc_ValueError, "ENVI data type must be 12 (unsigned short).");
		return NULL;
	}
	
	n_lines = envi_hdr->lines;
	n_samples = envi_hdr->samples;
	
	image = (unsigned short **)readENVIStandardBand(fl_filepath, band_index, envi_hdr);
	if(!image){
		return NULL;
	}
	
	mask = (char **)allocv2d(n_lines, n_samples, sizeof(char));
	if(!mask){
		return NULL;
	}
	
	//set edges to 0
	for(i = 0; i < n_lines; i++){
		image[i][0] = 0;
		image[i][n_samples-1] = 0;
	}

	for(j = 0; j < n_samples; j++){
		image[0][j] = 0;
		image[n_lines-1][j] = 0;
	}
	
	for(i = 0; i < n_lines; i++)
	for(j = 0; j < n_samples; j++)
	{
		if(image[i][j] > 0)
			mask[i][j] = 1;
		else
			mask[i][j] = 0;
	}
	
	freeus2d(image, n_lines);
	
	//todo: look into bug - flight line delineation is off by 1 pixel
	// probably abiguity in the envi header
	ul_easting = envi_hdr->info.easting - (envi_hdr->info.x_pixel-1) * envi_hdr->info.x_scale;
	ul_northing = envi_hdr->info.northing + (envi_hdr->info.y_pixel-1) * envi_hdr->info.y_scale;
	
	printf("n_lines = %d , n_samples = %d\n", n_lines, n_samples);
	
	contour = trace(mask, n_lines, n_samples, ul_easting, ul_northing, envi_hdr->info.x_scale, envi_hdr->info.y_scale);
	if(!contour){
		return NULL;
	}
	
	HSHP_flightline = SHPCreate(ov_filepath, SHPT_POLYGON);
	if(!HSHP_flightline){
		PyErr_SetString(PyExc_IOError, "SHPCreate failed. File could already exist.");
		return NULL;
	}
	HDBF_flightline = DBFCreate(ov_filepath);
	if(!HDBF_flightline){
		PyErr_SetString(PyExc_IOError, "DBFCreate failed.");
		return NULL;
	}
	if( DBFAddField(HDBF_flightline, "Flightline", FTString, 64, 0) == -1 ){
		PyErr_SetString(PyExc_IOError, "DBFAddField failed when adding field 'Flightline'.");
		return NULL;
	}
	
	SHPComputeExtents(contour);
	SHPWriteObject(HSHP_flightline, -1, contour);
	DBFWriteStringAttribute(HDBF_flightline, 0, 0, fl_basename);
	SHPDestroyObject(contour);
	SHPClose(HSHP_flightline);
	DBFClose(HDBF_flightline);
	free(envi_hdr);
	freec2d(mask, n_lines);
	
	return Py_None;
}

static PyObject* Py_InitMergedShapeFile(PyObject * self, PyObject * args)
{
	char * merged_filepath;
	
	if(!PyArg_ParseTuple(args, "s", &merged_filepath))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	printf("Py_InitMergedShapeFile\n");
	
	SHPHandle 
	merged = SHPCreate(merged_filepath, SHPT_POLYGON); 
	if(!merged){
		PyErr_SetString(PyExc_IOError, "Could not create shape file.\n");
		return NULL;
	}
	SHPClose(merged);
	
	DBFHandle
	dbf_merged = DBFCreate(merged_filepath);
	if(!dbf_merged){
		PyErr_SetString(PyExc_IOError, "Could not create dbf file.\n");
		return NULL;
	}
	DBFAddField(dbf_merged, "Flightline", FTString, 64, 0);
	DBFClose(dbf_merged);
	
	return Py_None;
}

static PyObject* Py_MergeShapeFiles(PyObject * self, PyObject * args)
{
	char * from, * to;
	
	if(!PyArg_ParseTuple(args, "ss", &from, &to))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
		
	printf("Py_MergedShapeFile\n");
	printf("%s, %s\n", from, to);
	
	if(!merge(from, to))
		return NULL;
	
	return Py_None;
}

static PyMethodDef FLD_module_methods[] = {
	{"delineateFlightLine", Py_DelineateFlightLine, METH_VARARGS},
	{"initMergedShapeFile", Py_InitMergedShapeFile, METH_VARARGS},
	{"mergeShapeFiles", Py_MergeShapeFiles, METH_VARARGS},
	{NULL, NULL}
};

void initFLD_module(void)
{
	(void) Py_InitModule("FLD_module", FLD_module_methods);
}
