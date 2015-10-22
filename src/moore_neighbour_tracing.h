#ifndef MOORE_NEIGBOUR_TRACING_H
#define MOORE_NEIGBOUR_TRACING_H

#include <Python/Python.h>

#include <shapefil.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
	int x;
	int y;
}
Point2D;

SHPObject * trace (char ** mask, int length, int width, double ul_easting, double ul_northing, float x_scale, float y_scale);

#endif
