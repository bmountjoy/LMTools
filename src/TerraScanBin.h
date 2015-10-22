#ifndef TERRA_SCAN_BIN_H
#define TERRA_SCAN_BIN_H


#include "memory.h"

typedef unsigned char	BYTE;
typedef unsigned short	USHORT;
typedef unsigned int	UINT;

typedef struct
{
	int    	HdrSize;	// sizeof(ScanHdr)
	int     HdrVersion;	// Version 20020715, 20010712, 20010129 or 970404
	int     RecogVal;	// Always 970401
	char    RecogStr[4];	// CXYZ
	int		PntCnt;		// number of points in the file
	int     Units;		// x = (pnt.x - orgx) / (double) units
	double  OrgX;		// coordinate system origin
	double  OrgY;
	double  OrgZ;
	int     Time;		// 32 bit integer timestamp
	int     Color;		// color values
}
ScanHdr;

typedef struct
{
	int	x;
	int	y;
	int	z;
}
Point3d;


/**
 * Point record for version 20020715
 */
typedef struct
{
	Point3d	Pnt;
	BYTE    Code;		//classification code
	BYTE    Echo;		//echo information
	BYTE    Flag;		//view visibility
	BYTE	Mark;		
	USHORT  Line;		//flightline number
	USHORT  Intensity;	//intensity
}
ScanPnt;

ScanHdr * readTerraScanHdr(FILE *);
ScanPnt * getNextPnt(FILE *);
double ** bufferTerraScanBinFile(char * path, int * p_npts);

#endif
