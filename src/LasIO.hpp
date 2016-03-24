#ifndef LAS_IO_H
#define LAS_IO_H


#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
}
#endif

#include "MyLASLib.hpp"
// #include "lasreader.hpp"

double ** bufferLasFile(char * path, int * p_npts);

#endif
