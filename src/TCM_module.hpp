#ifndef TCM_MODULE_H
#define TCM_MODULE_H

#include <Python/Python.h>



#ifdef __cplusplus
extern "C"{
#endif

#include <pthread.h>

#include <shapefil.h>

//libtiff
#include <tiffio.h>

//libgeotiff
#include <geotiff.h>
#include <xtiffio.h>
//GTIFDefn
#include <geo_normalize.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "PointInPolygon.h"
#include "Biometrics.h"
#include "ENVIStandardIO.h"
#include "Quantize.h"
#include "List.h"
#include "memory.h"
#include "tiffio.h"

#ifdef __cplusplus
}
#endif


#include <lasreader.hpp>

typedef struct
{
	int x_idx, y_idx, rad_sum, tc_id;
	float lid_avg, lid_max, rank;
	double easting, northing;
	unsigned short * hspec_values;
	int hspec_size;
}
HSpecRecord;

/*
typedef struct
{
	List * hspec_records [];
	SHPObject * shapes [];
	int start_crown;
	int end_crown;
	ENVIHeader * envi_hdr; 
	SHPObject * fline; 
	double ul_easting; 
	double ul_northing; 
	int n_records;
	float half_pix;
}
Param;
*/


/**
 * Global variables
 */
ENVIHeader * envi_hdr;
List *** gridded_zvalues;
List **  hspec_records;
int n_records;


#endif
