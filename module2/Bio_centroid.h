#ifndef BIO_CENTROID_H
#define BIO_CENTROID_H

#include <TerraScanBin.h>
#include "List.h"

// Bio_centroid.c
int centroidBiometrics
(
	int    point,
	double centroidEasting, 
	double centroidNorthing,
	int    radius,
	double minEasting,	//easting of bottom left corner 
	double minNorthing,	//northing of bottom left corner
	int    blockSize,
	int    zThreshold,
	char * chmDir,
	char * outDir
);

#endif
