#ifndef ENVI_STD_IO_H
#define ENVI_STD_IO_H

#include <Python/Python.h>

#include "memory.h"

typedef struct
{
	char proj_name [8];
	int x_pixel;
	int y_pixel;
	double easting;
	double northing;
	float x_scale;
	float y_scale;
	int proj_zone;
	int north;
}
MapInfo;

typedef struct
{
	char interleave [8];
	int samples;
	int lines;
	int bands;
	int header_offset;
	int datatype;
	int x_start;
	int y_start;
	int byte_order;
	float * wavelengths;
	int n_wavelengths;
	MapInfo info;
}
ENVIHeader;

void ** readENVIStandardBand(char * base_path, int band_index, ENVIHeader * header);
ENVIHeader * getENVIHeaderInfo(char * base_path);
int convert_ushort_bsq_to_bil(char * bsq_base_path, char * bil_path);

/**
 * - 'path' is the path to the .dat file
 * - all header information already given in EHVIHeader struct
 */
void *** readENVIFile(char * path, ENVIHeader * header);
void freeENVIHeader(ENVIHeader * header);

#endif

