

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
#include "TCD_module.h"

/**
 * Returns the number of bytes require to store the data from the
 * the tiff filed specified by 'inTiff'.
 */
static PyObject* Py_GetImageSize(PyObject * self, PyObject * args)
{
	puts("Py_GetImageSize");
	char * inTiff;
	
	if(!PyArg_ParseTuple( args, "s", &inTiff))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return Py_BuildValue("l", -1);
	}
	
	TIFF *tiff = XTIFFOpen((const char*)inTiff, "r");
	if(!tiff){
		PyErr_SetString(PyExc_ValueError, "XTIFFOpen failed.");
		return Py_BuildValue("l", -1);
	}
	
	uint32 imageWidth, imageLength;
	
	TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH,  &imageWidth);
	
	XTIFFClose(tiff);
	
	return Py_BuildValue("l", imageLength * imageWidth * 4);
}

/**
 * Finds the tree tops in a single TIFF image.
 */
static PyObject* Py_FindTreeTops(PyObject * self, PyObject * args)
{
	puts("Py_FindTreeTops");
	
	int isPartition, run_range3, run_range5, run_range7, run_range9, run_range11, smooth_type;
	float range3_min, range3_max, range5_min, range5_max, range7_min, range7_max, range9_min, range9_max, range11_min, range11_max;
	char *inTiff, *outFile;
	
	if(!PyArg_ParseTuple( args, "ssiffiffiffiffiffii", &inTiff, &outFile, 
		&run_range3,  &range3_min,  &range3_max, 
		&run_range5,  &range5_min,  &range5_max, 
		&run_range7,  &range7_min,  &range7_max,
		&run_range9,  &range9_min,  &range9_max,
		&run_range11, &range11_min, &range11_max,
		&isPartition, &smooth_type))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	printf("\tInput tiff: %s\n", inTiff);
	printf("\tOut treetop: %s\n", outFile);
	
	int16 mpsCount, mttCount;
	uint32 imageWidth, imageLength;
	double ulEasting, ulNorthing, *mpsData , *mttData;
	float ** image;
	
	
	/**
	 * Open output shapefile.
	 */
	SHPHandle
	hshp = SHPOpen(outFile, "rb+");
	if(!hshp){
		hshp = SHPCreate(outFile, SHPT_POINT);
		if(!hshp){
			PyErr_SetString(PyExc_IOError, "SHPCreate failed.");
		}
	}
	
	/**
	 * Open attribute file.
	 */
	DBFHandle 
	hdbf = DBFOpen(outFile, "rb+");
	if(!hdbf){
		hdbf = DBFCreate(outFile);
		if(!hdbf){
			PyErr_SetString(PyExc_IOError, "DBFCreate failed.");
			return NULL;
		}
		DBFAddField(hdbf, "Height", FTDouble, 5, 2);
		DBFAddField(hdbf, "TC Id", FTInteger, 10, 0);
	}
	
	/**
	 * Open input Tiff.
	 */
	TIFF *tiff = XTIFFOpen((const char*)inTiff, "r");
	if(!tiff){
		PyErr_SetString(PyExc_ValueError, "XTIFFOpen failed.");
		return NULL;
	}
	
	/*
	if(TIFFIsTiled(tiff)){
		PyErr_SetString(PyExc_IOError, "Software doesn't support tiled images.");
		return NULL;
	}
	*/
	
	/**
	 * Extract (geo)tiff tags.
	 */
	TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH,  &imageWidth);
	getModelTiePointTag   (tiff, &mttData, &mttCount);
	getModelPixelScaleTage(tiff, &mpsData, &mpsCount);
	ulEasting  = getUlEasting(mpsData, mpsCount, mttData, mttCount);
	ulNorthing = getUlNorthing(mpsData, mpsCount, mttData, mttCount);
	
	if(mpsCount < 3){
		PyErr_SetString(PyExc_ValueError, "mpsCount < 3.");
		return NULL;
	}
	
	
	/**
	 * Buffer image.
	 */
	image = readTIFF(tiff);
	if(!image)
		return NULL;
	
	if(isPartition)
	{
		if(!findInPartition(image, mpsData, (int)imageLength, (int)imageWidth, ulEasting, ulNorthing, 
			run_range3, range3_min, range3_max,
			run_range5, range5_min, range5_max,
			run_range7, range7_min, range7_max,
			hshp, hdbf)
		  )
			return NULL;
	}
	else
	{
		if(!findTreeTops(image, mpsData, (int)imageLength, (int)imageWidth, ulEasting, ulNorthing, 
			run_range3, range3_min, range3_max,
			run_range5, range5_min, range5_max,
			run_range7, range7_min, range7_max,
			run_range9, range9_min, range9_max,
			run_range11, range11_min, range11_max,
			hshp, hdbf, smooth_type)
		  )
			return NULL;
	}
	
	SHPClose(hshp);
	DBFClose(hdbf);
	XTIFFClose(tiff);
	
	return Py_None;
}

/**
 * Partitions the TIFF file specified by 'inTiff' into smaller TIFF files to 
 * be processed seperately.
 * 
 * Note: This function assumes that the directory 'partDir' already exists.
 */
static PyObject* Py_PartitionTiff(PyObject * self, PyObject * args)
{
	puts("Py_PartitionTiff");
	
	char * inTiff, * partDir;
	
	if(!PyArg_ParseTuple(args, "ss", &inTiff, &partDir))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	if( !partitionTiff(inTiff, partDir) ) return NULL;
	
	return Py_None;
}
 
 
 
static PyObject* Py_TreeCrownDelineation(PyObject * self, PyObject * args)
{
	puts("Py_TreeCrownDelineation");
	
	char * inShp, * outShp, * inTiff, * outFile;
	int rad_1, rad_2, rad_3, run_h1, run_h2, run_h3, smooth_type, shape_crown;
	float h1_min, h1_max, h2_min, h2_max, h3_min, h3_max, perc_1, perc_2, perc_3;
	
	if(!PyArg_ParseTuple(args, "ssssiffiffifffffiiiii", &inShp, &outShp, &inTiff, &outFile, 
		&run_h1, &h1_min, &h1_max,
		&run_h2, &h2_min, &h2_max,
		&run_h3, &h3_min, &h3_max,
		&perc_1, &perc_2, &perc_3, &rad_1, &rad_2, &rad_3,
		&smooth_type, &shape_crown))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}

	printf("Outfile is %s\n", outFile);
	
	uint32 imageWidth, imageLength;
	int16 mpsCount, mttCount;
	double * mpsData, * mttData, uln, ule;
	GTIFDefn gtifDefn;
	
	TIFF *tif = XTIFFOpen(inTiff, "r");
	if(!tif){
		PyErr_SetString(PyExc_IOError, "XTIFFOpen failed.");
		return NULL;
	}
	
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,  &imageWidth);
	extractGeoTiffTags(tif, &gtifDefn, &mpsData, &mpsCount, &mttData, &mttCount, &uln, &ule);
	
	if(mpsCount < 3){
		PyErr_SetString(PyExc_ValueError, "Insufficient pixel scale information.");
		return NULL;
	}
	if(mpsData[0] != mpsData[1]){
		PyErr_SetString(PyExc_ValueError, "Pixel scale x does not equal pixel scale y. Pixels must be square.");
		return NULL;
	}
	
	float **
	inBuf = readTIFF(tif);
	if(!inBuf)
		return NULL;
	
	/**
	 * Shapefile that contains tree top locations.
	 */
	SHPHandle
	HSHP_treetops = SHPOpen(inShp, "rb");
	if(!HSHP_treetops){
		PyErr_SetString(PyExc_IOError, "SHPOpen failed.");
		return NULL;
	}
	
	char indbf[256];
	strcpy(indbf, inShp);
	
	DBFHandle
	HDBF_treetops = DBFOpen(indbf, "rb+");
	if(!HDBF_treetops){
		printf("%s\n", indbf);
		PyErr_SetString(PyExc_IOError, "DBFOpen failed.");
		return NULL;
	}
	
	int res = tcd(HSHP_treetops, HDBF_treetops, outShp, inBuf, imageLength, imageWidth, ule, uln,
		run_h1, h1_min, h1_max, run_h2, h2_min, h2_max, run_h3, h3_min, h3_max, perc_1, perc_2, perc_3, rad_1, rad_2, rad_3, 
		mpsData, smooth_type, shape_crown);
	if(!res) {
		PyErr_SetString(PyExc_IOError, "Tree crown delineation has failed.");
		return NULL;
	}

	SHPClose(HSHP_treetops);
	DBFClose(HDBF_treetops);
	
//write smoothed image for testing	
/*
	float ** 
	smooth = allocf2d(imageLength, imageWidth);
	if(!smooth){
		printf("Couln't smoot yo!\n");
		return Py_None;
	}
	int i,j;
	//copy
	for(i=0;i<imageLength;i++)for(j=0;j<imageWidth;j++)smooth[i][j]=inBuf[i][j];
	//smooth
	for(i=2;i<imageLength-2;i++)
	for(j=2;j<imageWidth-2;j++)
	{
		int ii,jj;
		float sum;
		

		sum = 0.0;
		for(ii = i-1; ii<=i+1; ii++)
		for(jj = j-1; jj<=j+1; jj++)
			sum += inBuf[ii][jj];
		smooth[i][j] = sum / 9.0;

		
		float g3 [3][3] = {
		{0.0751136, 0.123841, 0.0751136},
		{0.1238410, 0.204180, 0.1238410},
		{0.0751136, 0.123841, 0.0751136}};

		sum = 0.0;
		for(ii = 0; ii < 3; ii++)
		for(jj = 0; jj < 3; jj++)
			sum += inBuf[i-1+ii][j-1+jj] * g3[ii][jj];		
		smooth[i][j] = sum;

		float g5 [5][5] = {
		{0.00296902, 0.0133062, 0.0219382, 0.0133062, 0.00296902},
		{0.01330620, 0.0596343, 0.0983203, 0.0596343, 0.0133062},
		{0.02193820, 0.0983203, 0.1621030, 0.0983203, 0.0219382},
		{0.01330620, 0.0596343, 0.0983203, 0.0596343, 0.0133062},
		{0.00296902, 0.0133062, 0.0219382, 0.0133062, 0.00296902}};
		

		sum = 0.0;
		for(ii = 0; ii < 5; ii++)
		for(jj = 0; jj < 5; jj++)
			sum += inBuf[i-2+ii][j-2+jj] * g5[ii][jj];
		smooth[i][j] = sum;	
		
		
	}
	TIFF * out = XTIFFOpen("/Users/Ben/Desktop/tt_test/tc_out/smoothed.tif", "w");
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, 		imageWidth);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, 		imageLength);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 	32);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 	1);
	TIFFSetField(out, TIFFTAG_DATATYPE, 		3);
	TIFFSetField(out, 33550,   mpsCount, mpsData);
	TIFFSetField(out, 33922,   mttCount, mttData);
	
	int row;
	for(row = 0; row < imageLength; row++){
		TIFFWriteScanline(out, smooth[row], row, 0);
	}
	XTIFFClose(out);
	freef2d(smooth, imageLength);
*/
		
	//freef2d(inBuf, imageLength);
	
	return Py_None;
}

static PyObject* Py_TreetopMetricsInit(PyObject * self, PyObject * args)
{
	puts("Py_TreetopMetricsInit");
	
	char * ref_file;
	
	if(!PyArg_ParseTuple(args, "s", &ref_file))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	if(!treetop_metrics_init(ref_file)) 
		return NULL;
	
	return Py_None;
}

static PyObject* Py_TreetopMetricsGrid(PyObject * self, PyObject * args)
{
	puts("Py_TreetopMetricsGrid");
	
	char * tt_path;
	
	if(!PyArg_ParseTuple(args, "s", &tt_path))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	if(!treetop_metrics_grid(tt_path))
		return NULL;
	
	return Py_None;
}

static PyObject* Py_TreetopMetricsFinish(PyObject * self, PyObject * args)
{
	puts("Py_TreetopMetricsFinish");
	
	char * ttm_path;
	
	if(!PyArg_ParseTuple(args, "s", &ttm_path))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	treetop_metrics_finish(ttm_path);
	
	return Py_None;
}

static PyMethodDef TCD_module_methods[] = {
	{"getImageSize", 			Py_GetImageSize, 			METH_VARARGS},
	{"findTreeTops",			Py_FindTreeTops,			METH_VARARGS},
	{"partitionTiff", 			Py_PartitionTiff, 			METH_VARARGS},
	{"treeCrownDelineation", 	Py_TreeCrownDelineation, 	METH_VARARGS},
	{"treetopMetricsInit", 		Py_TreetopMetricsInit, 		METH_VARARGS},
	{"treetopMetricsGrid", 		Py_TreetopMetricsGrid, 		METH_VARARGS},
	{"treetopMetricsFinish", 	Py_TreetopMetricsFinish, 	METH_VARARGS},
	{NULL, NULL}
};

void initTCD_module(void)
{
	(void) Py_InitModule("TCD_module", TCD_module_methods);
}
