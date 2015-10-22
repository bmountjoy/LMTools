



#ifndef TREETOP_METRICS_H
#define TREETOP_METRICS_H

#include <shapefil.h>
#include <tiffio.h>
#include "geotiff.h"
#include "xtiffio.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "memory.h"

int treetop_metrics_init(char * ref_file);
int treetop_metrics_grid(char * tt_path);
void treetop_metrics_finish(char * ttm_path);

#endif
