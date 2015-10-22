#ifndef ASD_IO_H
#define ASD_IO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memory.h"

char * buffer_asd_header(char * path, char *** p_header, int * p_ncols);
char * buffer_asd_file(char * path, float ** p_waves, float *** p_spec, int * pn_waves, int * pn_spec);

#endif
