
#include <Python/Python.h>

//libtiff
#include <tiffio.h>

//libgeotiff
#include "geotiff.h"
#include "xtiffio.h"

//GTIFDefn
#include <geo_normalize.h>


#define ModelPixelScaleTag	33550	//geotiff tag id
#define ModelTiepointTag	33922	//geotiff tag id


float ** scanlineIO(TIFF * tif);
float ** tileIO(TIFF * tif);


double getUlEasting(double *mpsData, int mpsCount, double *mttData, int mttCount)
{
	if(mttCount < 6 || mpsCount < 3)
		return -1.0;
	
	printf("%lf\n", mttData[3] - (mttData[0] * mpsData[0]));
	return mttData[3] - (mttData[0] * mpsData[0]);
}

double getUlNorthing(double *mpsData, int mpsCount, double *mttData, int mttCount)
{
	if(mttCount < 6 || mpsCount < 3){
		printf("mmtcount:%d,mpscount:%d\n", mttCount, mpsCount);	
		return -1.0;
	}
	
	printf("%lf\n", mttData[4] + (mttData[1] * mpsData[1]));
	return mttData[4] + (mttData[1] * mpsData[1]);
}

void getModelTiePointTag(TIFF *tif, double ** mttData, int16 * mttCount)
{
	TIFFGetField(tif, ModelTiepointTag,	mttCount, mttData);
	printf("%lf, %lf, %lf, %lf, %lf, %lf\n", (*mttData)[0],(*mttData)[1],(*mttData)[2],(*mttData)[3],(*mttData)[4],(*mttData)[5]);
}

void getModelPixelScaleTage(TIFF *tif, double ** mpsData, int16 * mpsCount)
{
	TIFFGetField(tif, ModelPixelScaleTag, mpsCount, mpsData);
	printf("%lf, %lf, %lf\n", (*mpsData)[0],(*mpsData)[1],(*mpsData)[2]);
}

int extractGeoTiffTags(TIFF * tif, GTIFDefn * gtifDef, double ** mpsData, int16 *mpsCount,
	double ** mttData, int16 *mttCount, double *ulNorthing, double *ulEasting)
{
	GTIF * gtif = GTIFNew(tif);
	
	if(!gtif){
		PyErr_SetString(PyExc_IOError, "GTIFNew failed.");
		return 0;
	}
	
	GTIFGetDefn(gtif, gtifDef);
	GTIFFree(gtif);
	
	TIFFGetField(tif, ModelPixelScaleTag, mpsCount, mpsData);
	TIFFGetField(tif, ModelTiepointTag,	  mttCount, mttData);
	
	*ulNorthing = getUlNorthing(*mpsData, *mpsCount, *mttData, *mttCount);
	*ulEasting  = getUlEasting (*mpsData, *mpsCount, *mttData, *mttCount);
	
	return 1;
}

/**
 * Open and read tiff file.
 */
 /*
float ** openTIFF(char *path)
{
	TIFF *tiff = XTIFFOpen((const char*)path, "r");
	if(!tiff){
		PyErr_SetString(PyExc_ValueError, "XTIFFOpen failed.");
		return NULL;
	}
	
	return readTIFF(tiff);
}
*/
/**
 * Read opened TIFF file.
 */
float ** readTIFF(TIFF *tif)
{	
	printf("readTIFF...\n");
	
	float **image;
	
	if(TIFFIsTiled(tif))
		image = tileIO(tif);
	else
		image = scanlineIO(tif);
	
	return image;
}

/**
 * Read in image scanlines and set global variables.
 */
float ** scanlineIO(TIFF * tif)
{
	uint16 planarConfig, orientation, dataType;
	uint32 imageLength, imageWidth;
	long scanlineSize;
	float **image;
	
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarConfig);
	TIFFGetField(tif, TIFFTAG_ORIENTATION, &orientation);
	TIFFGetField(tif, TIFFTAG_DATATYPE, &dataType);
	
	scanlineSize = TIFFScanlineSize(tif);
	
	printf("imageWidth:%d\n", imageWidth);
	printf("imageLength:%d\n", imageLength);
	printf("planarConfig:%d\n", planarConfig);
	printf("scanlineSize:%d\n", (int)scanlineSize);
	printf("orientation:%d\n", orientation);
	printf("dataType: %d\n", dataType);
	
	if( (image = (float **) _TIFFmalloc(sizeof(float *) * imageLength)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return NULL;
	}
	
	int row;
	for(row = 0; row < imageLength; row++)
	{
		if( (image[row] = (float *) _TIFFmalloc(scanlineSize)) == NULL)
		{
			PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
			return 0;
		}
		
		TIFFReadScanline(tif, image[row], row, 0);
	}
	
	return image;
}

/**
 * Read in tiled image and set global variables.
 */ 
float ** tileIO(TIFF * tif)
{	
	uint32 tilesAcross, tilesDown, imageWidth, imageLength, tilesPerImage, tileLength, tileWidth;
	uint16 dataType, photoMetric, samplesPerPixel, bitsPerSample, planarConfig, orientation, resolutionUnit;
	uint32 * stripOffsets;
	float xresolution, yresolution, xposition, yposition;
	
	TIFFGetField(tif, TIFFTAG_TILEWIDTH, 		&tileWidth);
	TIFFGetField(tif, TIFFTAG_TILELENGTH, 		&tileLength);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, 		&imageWidth);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, 		&imageLength);
	TIFFGetField(tif, TIFFTAG_DATATYPE, 		&dataType);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, 		&photoMetric); 
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, 	&samplesPerPixel);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, 	&bitsPerSample);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, 	&planarConfig);
	TIFFGetField(tif, TIFFTAG_STRIPOFFSETS, 	&stripOffsets);
	TIFFGetField(tif, TIFFTAG_ORIENTATION, 		&orientation);
	TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT,	&resolutionUnit);
	TIFFGetField(tif, TIFFTAG_XRESOLUTION,		&xresolution);
	TIFFGetField(tif, TIFFTAG_YRESOLUTION,		&yresolution);
	TIFFGetField(tif, TIFFTAG_XPOSITION,		&xposition);
	TIFFGetField(tif, TIFFTAG_YPOSITION,		&yposition);
	
	tilesAcross   = (imageWidth + tileWidth - 1)/tileWidth;
	tilesDown     = (imageLength + tileLength - 1)/tileLength;
	tilesPerImage = tilesAcross * tilesDown;
	
	printf("imageWidth              : %d\n", imageWidth);
	printf("imageLength             : %d\n", imageLength);
	printf("photoMetric             : %d\n", photoMetric);
	printf("dataType                : %d\n", dataType);
	printf("samplesPerPixel         : %d\n", samplesPerPixel);
	printf("bitsPerSample           : %d\n", bitsPerSample);
	printf("photoMetric             : %d\n", photoMetric);
	printf("orientation             : %d\n", orientation);
	printf("tileWidth               : %d\n", tileWidth);
	printf("tileLength              : %d\n", tileLength);
	printf("tilesAcross             : %d\n", tilesAcross);
	printf("tilesDown               : %d\n", tilesDown);
	printf("tilesPerImage           : %d\n", tilesPerImage);
	printf("TIFFNumberOfTiles(tif)  : %d\n", TIFFNumberOfTiles(tif));
	printf("TIFFTileSize            : %d\n", (int)TIFFTileSize(tif));
	printf("resolutionUnit          : %d\n", resolutionUnit);
	printf("xresolution             : %f\n", xresolution);
	printf("yresolution             : %f\n", yresolution);
	printf("xposition               : %f\n", xposition);
	printf("yposition               : %f\n", yposition);
	
	uint32 numberOfTiles = TIFFNumberOfTiles(tif);
	long   tileSize      = TIFFTileSize(tif);
	float *tiles[numberOfTiles], **image;
	
	printf("\tReading in tiles ... \n");
	ttile_t tile;
	
	//ensure tiles are read row by row
	uint32 x,y;
	tile = -1;
	for(y = 0; y < imageLength; y+=tileLength){
		for(x = 0; x < imageWidth; x+=tileWidth){
			++tile;
			//printf("tile : %d %d %d\n", tile, x, y);
			if( (tiles[tile] = (float *) _TIFFmalloc(tileSize)) == NULL)
			{
				PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed reading tiles.");
				return NULL;
			}
			TIFFReadTile(tif, tiles[tile], x, y, 0, 0);
		}
	}
	
	if( (image = (float **) _TIFFmalloc(sizeof(float *) * imageLength)) == NULL )
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return NULL;
	}
	
	printf("\tConverting tiles to array ... \n");
	int row, col, i;
	for(row = 0; row < imageLength; row++)
	{
	 	int tileStart = ((int)(row/tileLength))*tilesAcross;
	 	int tileEnd   = tileStart + tilesAcross;
	 	
	 	if( (image[row] = (float *) _TIFFmalloc(imageWidth * sizeof(float))) == NULL )
	 	{
	 		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
	 		return NULL;
	 	}
		
	 	col = 0;
		for(tile = tileStart; tile < tileEnd; tile++)
		{
			int dataStart = (row % tileLength) * tileLength;
			int dataEnd   = dataStart + tileLength;
			
			//data range within each tile
			for(i = dataStart; i < dataEnd; i++)
			{
				image[row][col] = tiles[tile][i];
				
				if(++col >= imageWidth)break;
			}
			if(col >= imageWidth)break;
			
		}//end for
		
	}//end for
	
	for(tile = 0; tile < numberOfTiles; tile++)
		_TIFFfree(tiles[tile]);
	
	printf("\tDone converting tiles ... \n");
	return image;
}

void setGeoKeysFromGTIFDefn(GTIF * gtif, GTIFDefn gtifDef)
{
	GTIFKeySet(gtif, GTModelTypeGeoKey,		TYPE_SHORT, 1, gtifDef.Model);			printf("%d\n", gtifDef.Model);
	GTIFKeySet(gtif, ProjectedCSTypeGeoKey,		TYPE_SHORT, 1, gtifDef.PCS);		printf("%d\n", gtifDef.PCS);
	GTIFKeySet(gtif, GeographicTypeGeoKey,		TYPE_SHORT, 1, gtifDef.GCS);		printf("%d\n", gtifDef.GCS);
	GTIFKeySet(gtif, ProjLinearUnitsGeoKey,		TYPE_SHORT, 1, gtifDef.UOMLength);	printf("%d\n", gtifDef.UOMLength);
	GTIFKeySet(gtif, GeogLinearUnitsGeoKey, TYPE_SHORT,  1, 9001);
	GTIFKeySet(gtif, GeogGeodeticDatumGeoKey,	TYPE_SHORT, 1, gtifDef.Datum);		printf("%d\n", gtifDef.Datum);
	GTIFKeySet(gtif, GeogPrimeMeridianGeoKey,	TYPE_SHORT, 1, gtifDef.PM);			printf("%d\n", gtifDef.PM);
	GTIFKeySet(gtif, GeogEllipsoidGeoKey,		TYPE_SHORT, 1, gtifDef.Ellipsoid);	printf("%d\n", gtifDef.Ellipsoid);
	GTIFKeySet(gtif, ProjectionGeoKey,		TYPE_SHORT, 1, gtifDef.ProjCode);		printf("%d\n", gtifDef.ProjCode);
	
	int i, nParms, * ProjParmId;
	double * ProjParm;
	
	nParms 	   = gtifDef.nParms;
	ProjParm   = gtifDef.ProjParm;
	ProjParmId = gtifDef.ProjParmId;
	
	for(i = 0; i < nParms; i++)
		GTIFKeySet(gtif, ProjParmId[i], TYPE_DOUBLE, 1, ProjParm[i]);
	
	GTIFWriteKeys(gtif);
}

void applyExpandFactor(double *mpsData, int scaleX, int scaleY)
{
	mpsData[0] *= scaleX; 
	mpsData[1] *= scaleY;
	mpsData[2] = 0; 		//default
}

/**
 * Write 'outBuffer' to single band tiff file at 'filePath'.
 */
int src_writeTIFF(float ** outBuffer, char * filePath, int imageLength, int imageWidth, 
	GTIFDefn gtifDef, double *mpsData, int16 mpsCount, double *mttData, int16 mttCount)
{
	GTIF * outGtif;
	TIFF * outFile;
	
	if( (outFile = XTIFFOpen(filePath, "w")) == NULL )
	{
		PyErr_SetString(PyExc_IOError, "Could not open output TIFF file.");
		return 0;
	}
	if( (outGtif = GTIFNew(outFile)) == NULL )
	{
		PyErr_SetString(PyExc_IOError, "GTIFNew failed in function 'writeTIFF'.");
		return 0;
	}	

	TIFFSetField(outFile, TIFFTAG_IMAGEWIDTH, 		imageWidth);
	TIFFSetField(outFile, TIFFTAG_IMAGELENGTH, 		imageLength);
	TIFFSetField(outFile, TIFFTAG_BITSPERSAMPLE, 	32);
	TIFFSetField(outFile, TIFFTAG_SAMPLESPERPIXEL, 	1);
	TIFFSetField(outFile, TIFFTAG_COMPRESSION, 		COMPRESSION_NONE);
	TIFFSetField(outFile, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
	TIFFSetField(outFile, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
	TIFFSetField(outFile, TIFFTAG_DATATYPE, 		3);
	TIFFSetField(outFile, TIFFTAG_PLANARCONFIG, 	PLANARCONFIG_CONTIG);
	TIFFSetField(outFile, TIFFTAG_ORIENTATION, 		ORIENTATION_TOPLEFT);
	
	//geo info - tie point / pixel scale
	TIFFSetField(outFile,	ModelPixelScaleTag, mpsCount, mpsData);
	TIFFSetField(outFile,	ModelTiepointTag,   mttCount, mttData);
	
	/**
	 * For some reason this messes shit up.
	 */
	//setGeoKeysFromGTIFDefn(outGtif, gtifDef);
	
	int row;
	for(row = 0; row < imageLength; row++)
	{
		if(TIFFWriteScanline(outFile, outBuffer[row], row, 0) == -1)
		{
			PyErr_SetString(PyExc_IOError, "TIFFWriteScanline failed.");
			return 0;
		}
	}
	
	GTIFFree(outGtif);
	XTIFFClose(outFile);
	
	return 1;
}
