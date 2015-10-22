

#include <Python/Python.h>
#include "TCD_Module.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


float max( float x, float y ) { return x > y ? x : y; }
float min( float x, float y ) { return x < y ? x : y; }


int writeTile(float **image, int length, int width, GTIFDefn gtifDef, int16 mpsCount, double *mpsData,
	double ulEasting, double ulNorthing, int startLine, int startSample, char *outDir)
{	
	char path [256], file [64];
	double * tile_mttData;
	 
	tile_mttData = allocd1d(6);
	
	if(!tile_mttData)
		return 0;
	
	tile_mttData[0] = 0;
	tile_mttData[1] = 0;
	tile_mttData[2] = 0;
	tile_mttData[3] = ulEasting  + startSample * mpsData[0];
	tile_mttData[4] = ulNorthing - startLine   * mpsData[1];
	tile_mttData[5] = 0;
	
	sprintf(file, "/%d_%d.tif", startSample, startLine);
	strcpy(path, outDir);
	strcat(path, file);
	printf("\t%f\n", image[10][10]);
	
	if(!src_writeTIFF(image, path, length, width, gtifDef, mpsData, mpsCount, tile_mttData, 6))
		return 0;
	
	free(tile_mttData);
	
	return 1;
}



/**
 * Assumption is that we would like the program to take up less than
 * 500 MB of memory.
 * - add variable for partition size
 *
 * - consider making as modular as possible
 */
int partitionTiff(char * tiffPath, char * outDir)
{
	puts("partitionTiff");
	int tileSize = 5000;

	TIFF *tif = XTIFFOpen((const char *)tiffPath, "r");
	if(!tif){
		PyErr_SetString(PyExc_IOError, "XTIFFOpen failed.");
		return 0;
	}
	
	if(TIFFIsTiled(tif)){
		printf("TIFF is tiled! Don't know what to do!\n");
		return 0;
	}
	
	int16 mpsCount,  mttCount, dataType;
	uint32 imageLength, imageWidth, scanlineSize;
	int i, curLine, startLine, endLine, startSample, endSample, tileLength, tileWidth;
	float **image, *scanlineBuff;
	double ulNorthing, ulEasting, * mpsData, * mttData;
	GTIFDefn gtifDef;
	
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,  &imageWidth);
	TIFFGetField(tif, 339, &dataType);
	
	printf("\tdataType: %d\n", dataType);
	printf("\timageWidth: %d\n\timageLength: %d\n", imageWidth, imageLength);
	
	if(!extractGeoTiffTags(tif, &gtifDef, &mpsData, &mpsCount, &mttData, &mttCount, &ulNorthing, &ulEasting))
		return 0;

	scanlineSize = TIFFScanlineSize(tif);
	
	printf("\tScanline size: %d\n", scanlineSize);
	printf("\tNum samples: %d\n", scanlineSize/4);
	
	scanlineBuff = allocf1d(scanlineSize/sizeof(float));
	if(!scanlineBuff)
		return 0;
	
	for(startLine = 0; startLine < imageLength; startLine += tileSize-4)
	{
		endLine = min(startLine + tileSize, imageLength);
		
		for(startSample = 0; startSample < imageWidth; startSample += tileSize-4)
		{
			endSample  = min(startSample + tileSize, imageWidth);
			tileLength = endLine - startLine;
			tileWidth  = endSample - startSample;
			
			image = allocf2d(tileLength, tileWidth);
			if(!image){
				return 0;
			}
			/**
			 * Read 'tileSize' lines
			 */
			for(curLine = startLine; curLine < endLine; curLine++)
			{
				TIFFReadScanline(tif, scanlineBuff, curLine, 0);
				
				for(i = 0; i < tileWidth; i++){
					//if(scanlineBuff[i+startSample]!= 0)printf("\t%f\n",scanlineBuff[i+startSample]);
					image[curLine-startLine][i] = scanlineBuff[i + startSample];
				}
			}
			
			printf("\t(%d,%d)->(%d,%d)\n", startSample, startLine, endSample, endLine);
			
			
			if(! writeTile(image, tileLength, tileWidth, gtifDef, mpsCount, mpsData, 
				ulEasting, ulNorthing, startLine, startSample, outDir))
			{
				return 0;
			}
			
			freef2d(image, tileLength);
		}
	}
	
	free(scanlineBuff);
	XTIFFClose(tif);
	
	return 1;
}


/*
typedef struct {
    // From GTModelTypeGeoKey tag.  Can have the values ModelTypeGeographic
        or ModelTypeProjected.
    short	Model;

    // From ProjectedCSTypeGeoKey tag.  For example PCS_NAD27_UTM_zone_3N
    short	PCS;

    // From GeographicTypeGeoKey tag.  For example GCS_WGS_84 or
        GCS_Voirol_1875_Paris.  Includes datum and prime meridian value.
    short	GCS;	      

    // From ProjLinearUnitsGeoKey.  For example Linear_Meter.
    short	UOMLength;

    // One UOMLength = UOMLengthInMeters meters.
    double	UOMLengthInMeters;

    // The angular units of the GCS.
    short       UOMAngle;

    // One UOMAngle = UOMLengthInDegrees degrees.
    double      UOMAngleInDegrees;
    
    // Datum from GeogGeodeticDatumGeoKey tag. For example Datum_WGS84
    short	Datum;

    // Prime meridian from GeogPrimeMeridianGeoKey.  For example PM_Greenwich
        or PM_Paris.
    short	PM;

    // Decimal degrees of longitude between this prime meridian and
        Greenwich.  Prime meridians to the west of Greenwich are negative.
    double	PMLongToGreenwich;

    // Ellipsoid identifier from GeogELlipsoidGeoKey.  For example
        Ellipse_Clarke_1866.
    short	Ellipsoid;

    // The length of the semi major ellipse axis in meters.
    double	SemiMajor;

    // The length of the semi minor ellipse axis in meters.
    double	SemiMinor;

    // Projection id from ProjectionGeoKey.  For example Proj_UTM_11S.
    short	ProjCode;

    // EPSG identifier for underlying projection method.  From the EPSG
        TRF_METHOD table. 
    short	Projection;

    // GeoTIFF identifier for underlying projection method.  While some of
      these values have corresponding vlaues in EPSG (Projection field),
      others do not.  For example CT_TransverseMercator.
    short	CTProjection;   

    //Number of projection parameters in ProjParm and ProjParmId.
    int		nParms;

    //Projection parameter value.  The identify of this parameter
        is established from the corresponding entry in ProjParmId.  The
        value will be measured in meters, or decimal degrees if it is a
        linear or angular measure.
    double	ProjParm[MAX_GTIF_PROJPARMS];

    //Projection parameter identifier.  For example ProjFalseEastingGeoKey.
        The value will be 0 for unused table entries. 
    int		ProjParmId[MAX_GTIF_PROJPARMS]; // geokey identifier,
                                                   eg. ProjFalseEastingGeoKey

    //Special zone map system code (MapSys_UTM_South, MapSys_UTM_North,
        MapSys_State_Plane or KvUserDefined if none apply. 
    int		MapSys;

    //UTM, or State Plane Zone number, zero if not known. 
    int		Zone;

    //Do we have any definition at all?  0 if no geokeys found
    int         DefnSet;

} GTIFDefn;
*/
