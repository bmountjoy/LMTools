#ifndef BIO_SQUARE_H
#define BIO_SQUARE_H

#include <TerraScanBin.h>
#include "List.h"

int boundingBoxBiometrics
(
	int    point,
	double centroidEasting, 
	double centroidNorthing,
	int    sideLength,
	double minEasting,	//easting of bottom left corner 
	double minNorthing,	//northing of bottom left corner
	int    blockSize,
	int    zThreshold,
	char * chmDir,
	char * outDir
);

#endif
