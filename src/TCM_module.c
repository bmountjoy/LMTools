

/**
 * To compile : export MACOSX_DEPLOYMENT_TARGET=10.6
 *            : python setup.py install
 *       then : python LMTools.py
 * To build   : python setup_app.py py2app
 **/

#include "TCM_module.h"

/**
 * Computes....
 */
static PyObject* Py_TreeCrownMetrics(PyObject * self, PyObject * args)
{
	puts("Py_TreeCrownMetrics");
	
	char * las_path, * treecrown_path, * output_dir;
	
	if(!PyArg_ParseTuple(args, "sss", &las_path, &treecrown_path, &output_dir))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	//do stuff
	/*
	LASreadOpener lasreadopener;
	LASreader * lasreader;
	
	lasreadopener.set_file_name(las_path);
	lasreader = lasreadopener.open();
	if(!lasreader){
		PyErr_SetString(PyExc_IOError, "Could not open lidar file.");
		return NULL;
	}
	
	//open shapefile
	SHPHandle
	HSHP_treecrowns = SHPOpen(treecrown_path, "rb");
	if(!HSHP_treecrowns){
		PyErr_SetString(PyExc_IOError, "SHPOpen failed. Could not open tree crown file.");
		return NULL;
	}
	
	int nRecords, nShapeType;
	
	SHPGetInfo(HSHP_treecrowns, &nRecords, &nShapeType, NULL, NULL);
	if(nShapeType != SHPT_POLYGON){
		PyErr_SetString(PyExc_IOError, "Shape type not SHPT_POLYGON.");
		return NULL;
	}
	
	float * points = (float *)malloc(sizeof(float) * lasreader->npoints);
	int n_points;
	
	int record;
	for(record = 0; record < nRecords; record++)
	{
		SHPObject *
		pShape = SHPReadObject(HSHP_treecrowns, record);
		
		n_points = 0;
		
		while(lasreader->read_point())
		{
			double x_coord = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
			double y_coord = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
			float  z_coord = (float)( (lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset );
			
			if(pointInPolygon(pShape, x_coord, y_coord))
			{
				points[n_points++] = z_coord;
				break;
			}
			
		}
		printf("There are %d points in the %d-th tree crown.",n_points,record);
		//compute biometrics
	}
	
	lasreader->close();
	delete lasreader;
	
	free(points);
	SHPClose(HSHP_treecrowns);
	*/
	
	puts("This is a c file and it works!!!!!");
	
	return Py_None;
}

static PyMethodDef TCM_module_methods[] = {
	{"treeCrownMetrics", Py_TreeCrownMetrics, METH_VARARGS},
	{NULL, NULL}
};

void initTCM_module(void)
{
	(void) Py_InitModule("TCM_module", TCM_module_methods);
}
