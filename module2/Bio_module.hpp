#ifndef BIO_MODULE_H
#define BIO_MODULE_H

#include <Python/Python.h>

#ifdef __cplusplus
extern "C"{
#endif

//libtiff
#include <tiffio.h>

//libgeotiff - dont need as of right now
#include "geotiff.h"
#include "xtiffio.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <dirent.h>
#include <string.h>

#include <TerraScanBin.h>
#include <Bio_renameBinary.h>
#include "Bio_biometrics.h"
#include "Memory.h"
#include "list.h"
#include "Bio_centroid.h"
#include "Bio_square.h"
#include "Bio_rename.h"
#include "Bio_LMoments.h"
#include "Bio_IO.h"

#ifdef __cplusplus
}
#endif

// c++ declarations
#include "MyLASLib.hpp"
#include <LasIO.hpp>

#endif
