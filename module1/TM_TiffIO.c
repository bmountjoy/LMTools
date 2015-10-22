
#include "TM_module.h"
#include <geo_normalize.h> //GTIFDefn

#define ModelPixelScaleTag	33550	//geotiff tag id
#define ModelTiepointTag	33922	//geotiff tag id

extern int IMAGE_WIDTH;
extern int IMAGE_LENGTH;
extern int NUMBER_OF_ROWS;
extern int NUMBER_OF_COLS;
extern float ** IMAGE;

GTIFDefn GTIF_DEFN;					//struct that holds geotiff tags

/**
 * Contains relevant georeferencing information for the output tiff file.
 * - model pixel scale
 * - model tiepoint 
 */
int		MPS_COUNT, 
	  	MTT_COUNT;
		  
double 	* MPS_DATA, 
		* MTT_DATA;
		

double getUlEasting()
{
	if(MTT_COUNT < 6 || MPS_COUNT < 3)
		return -1.0;
	
	return MTT_DATA[3] - (MTT_DATA[0] * MPS_DATA[0]);
}

double getUlNorthing()
{
	if(MTT_COUNT < 6 || MPS_COUNT < 3)
		return -1.0;
	
	return MTT_DATA[4] + (MTT_DATA[1] * MPS_DATA[1]);
}

double getScaleX()
{
	if(MPS_COUNT < 3)
		return -1.0;
	
	return MPS_DATA[0];
}

double getScaleY()
{
	if(MPS_COUNT < 3)
		return -1.0;
	
	return MPS_DATA[1];
}

void setMttData(double i, double j, double k, double x, double y, double z)
{
	printf("setMTTData\n\t %lf %lf %lf %lf %lf %lf\n", i, j, k, x, y, z);
	
	_TIFFfree(MTT_DATA);
	
	if((MTT_DATA = (double *)_TIFFmalloc(sizeof(double) * 6)) == NULL)
	{
		MTT_COUNT = 0;
		return;
	}
	
	MTT_DATA[0] = i;
	MTT_DATA[1] = j;
	MTT_DATA[2] = k;
	MTT_DATA[3] = x;
	MTT_DATA[4] = y;
	MTT_DATA[5] = z;
	
	MTT_COUNT = 6;
}

/**
 * Extract privat tags ModelTiePointTag and ModelPixelScaleTag from input Tiff.
 */
void extractPrivateTiffTags(TIFF * tif)
{
	printf("extractPrivateTiffTags\n");
	double * mpsData, * mttData;
	
	TIFFGetField(tif, ModelPixelScaleTag,	&MPS_COUNT, &mpsData);
	TIFFGetField(tif, ModelTiepointTag,	&MTT_COUNT, &mttData);
	
	MPS_DATA = _TIFFmalloc(sizeof(double) * MPS_COUNT);
	MTT_DATA = _TIFFmalloc(sizeof(double) * MTT_COUNT);
	
	printf("%d , %d \n", MPS_COUNT, MTT_COUNT);
	
	int i;
	
	for(i = 0; i < MPS_COUNT; i++)
	{
		MPS_DATA[i] = mpsData[i];
		printf("%f ", MPS_DATA[i]);
	}
	puts("");
	for(i = 0; i < MTT_COUNT; i++){
		MTT_DATA[i] = mttData[i];
		printf("%f ", MTT_DATA[i]);
	}
	puts("");
}

/**
 * Open input tiff file and invoke appropriate IO method.
 */
int readTIFF(char * filePath)
{	
	printf("readTIFF...\n");
	
	TIFF * tif;
	if( (tif = XTIFFOpen((const char *)filePath, "r")) == NULL )
	{
		PyErr_SetString(PyExc_IOError, "Could not open input file. TIFFOpen failed.");
		return 0;
	}
	GTIF * gtif;
	if( (gtif = GTIFNew(tif)) == NULL )
	{
		PyErr_SetString(PyExc_IOError, "GTIFNew failed. Could not create extract geo keys.");
		return 0;
	}
	
	GTIFGetDefn(gtif, &GTIF_DEFN);
	
	extractPrivateTiffTags(tif);
	
	
	if(TIFFIsTiled(tif))
	{
		puts("Tile IO.\n");
		if(!tileIO(tif))
			return 0;
	}
	else
	{
		puts("Scanline IO.\n");
		if(!scanlineIO(tif))
			return 0;
	}
	
	GTIFFree(gtif);
	XTIFFClose(tif);
	return 1;
}

/**
 * Read in image scanlines and set global variables.
 *	- NUMBER_OF_ROWS / IMAGE_LENGTH
 *	- NUMBER_OF_COLS / IMAGE_WIDTH
 *	- IMAGE
 */
int scanlineIO(TIFF * tif)
{
	uint16 planarConfig, orientation;
	
	uint32 imageLength, imageWidth, scanlineSize, rowsPerStrip, nRowsPerStrip;
	
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarConfig);
	TIFFGetField(tif, TIFFTAG_ORIENTATION, &orientation);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsPerStrip, &nRowsPerStrip);
	
	scanlineSize = TIFFScanlineSize(tif);
	
	printf("imageWidth		:%d\n", imageWidth);
	printf("imageLength		:%d\n", imageLength);
	printf("planarConfig	:%d\n", planarConfig);
	printf("scanlineSize	:%d\n", scanlineSize);
	printf("orientation		:%d\n", orientation);
	printf("rowsPerStrip	:%d\n", rowsPerStrip);
	printf("nRowsPerStrip	:%d\n", nRowsPerStrip);
	
	IMAGE_LENGTH = NUMBER_OF_ROWS = imageLength;
	IMAGE_WIDTH  = NUMBER_OF_COLS = imageWidth;
	
	if( (IMAGE = (float **) _TIFFmalloc(sizeof(float *) * IMAGE_LENGTH)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return 0;
	}
	
	printf("Reading TIFF into memory.\n");
	
	int row;
	for(row = 0; row < IMAGE_LENGTH; row++)
	{
		if(row%500 == 0)printf("   row: %d\n", row);
		if( (IMAGE[row] = (float *) _TIFFmalloc(scanlineSize)) == NULL)
		{
			printf("Failed to allocate memory for image buffer\n");
			PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
			return 0;
		}
		
		TIFFReadScanline(tif, IMAGE[row], row, 0);
	}
	
	return 1;
}

/**
 * Read in tiled image and set global variables.
 *	- NUMBER_OF_ROWS / IMAGE_LENGTH
 *	- NUMBER_OF_COLS / IMAGE_WIDTH
 *	- IMAGE
 */ 
int tileIO(TIFF * tif)
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
	
	NUMBER_OF_COLS = IMAGE_WIDTH = imageWidth;
	NUMBER_OF_ROWS = IMAGE_LENGTH = imageLength;
	
	tilesAcross   = (imageWidth + tileWidth - 1)/tileWidth;
	tilesDown     = (imageLength + tileLength - 1)/tileLength;
	tilesPerImage = tilesAcross * tilesDown;
	
	printf("imageWidth              : %d\n", IMAGE_WIDTH);
	printf("imageLength             : %d\n", IMAGE_LENGTH);
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
	
	int    tileSize      = TIFFTileSize(tif);
	
	float * tiles[numberOfTiles];
	
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
				return 0;
			}
			TIFFReadTile(tif, tiles[tile], x, y, 0, 0);
		}
	}
	
	
	if( (IMAGE = (float **) _TIFFmalloc(sizeof(float *) * NUMBER_OF_ROWS)) == NULL )
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return 0;
	}
	
	printf("\tConverting tiles to array ... \n");
	int row, col, i;
	for(row = 0; row < NUMBER_OF_ROWS; row++)
	{
	 	int tileStart = ((int)(row/tileLength))*tilesAcross;
	 	int tileEnd   = tileStart + tilesAcross;
	 	
	 	if( (IMAGE[row] = (float *) _TIFFmalloc(NUMBER_OF_COLS * sizeof(float))) == NULL )
	 	{
	 		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
	 		return 0;
	 	}
		
	 	col = 0;
		for(tile = tileStart; tile < tileEnd; tile++)
		{
			int dataStart = (row % tileLength) * tileLength;
			int dataEnd   = dataStart + tileLength;
			
			//data range within each tile
			for(i = dataStart; i < dataEnd; i++)
			{
				IMAGE[row][col] = tiles[tile][i];
				
				if(++col >= IMAGE_WIDTH)break;
			}
			if(col >= IMAGE_WIDTH)break;
			
		}//end for
		
	}//end for
	
	for(tile = 0; tile < numberOfTiles; tile++)
		_TIFFfree(tiles[tile]);
	
	printf("\tDone converting tiles ... \n");
	return 1;
}

void setGeoKeysFromGTIFDefn(GTIF * gtif)
{
	GTIFKeySet(gtif, GTModelTypeGeoKey,		TYPE_SHORT, 1, GTIF_DEFN.Model);    	printf("GTModelTypeGeoKey : %d\n", GTIF_DEFN.Model);
	GTIFKeySet(gtif, ProjectedCSTypeGeoKey,		TYPE_SHORT, 1, GTIF_DEFN.PCS);      	printf("ProjectedCSTypeGeoKey : %d\n", GTIF_DEFN.PCS);
	GTIFKeySet(gtif, GeographicTypeGeoKey,		TYPE_SHORT, 1, GTIF_DEFN.GCS);      	printf("GeographicTypeGeoKey : %d\n", GTIF_DEFN.GCS);
	GTIFKeySet(gtif, ProjLinearUnitsGeoKey,		TYPE_SHORT, 1, GTIF_DEFN.UOMLength);	printf("ProjLinearUnitsGeoKey : %d\n", GTIF_DEFN.UOMLength);
	GTIFKeySet(gtif, GeogGeodeticDatumGeoKey,	TYPE_SHORT, 1, GTIF_DEFN.Datum);    	printf("GeogGeodeticDatumGeoKey : %d\n", GTIF_DEFN.Datum);
	GTIFKeySet(gtif, GeogPrimeMeridianGeoKey,	TYPE_SHORT, 1, GTIF_DEFN.PM);       	printf("GeogPrimeMeridianGeoKey : %d\n", GTIF_DEFN.PM);
	GTIFKeySet(gtif, GeogEllipsoidGeoKey,		TYPE_SHORT, 1, GTIF_DEFN.Ellipsoid);	printf("GeogEllipsoidGeoKey : %d\n", GTIF_DEFN.Ellipsoid);
	GTIFKeySet(gtif, ProjectionGeoKey,		TYPE_SHORT, 1, GTIF_DEFN.ProjCode); 	printf("ProjectionGeoKey : %d\n", GTIF_DEFN.ProjCode);
	
	int i, nParms, * ProjParmId;
	double * ProjParm;
	
	nParms 	   = GTIF_DEFN.nParms;
	ProjParm   = GTIF_DEFN.ProjParm;
	ProjParmId = GTIF_DEFN.ProjParmId;
	
	for(i = 0; i < nParms; i++)
	{
		//printf(" ppid : %d \n", ProjParmId[i]);
		GTIFKeySet(gtif, ProjParmId[i], TYPE_DOUBLE, 1, ProjParm[i]);
	}
	
	GTIFWriteKeys(gtif);
}

double * applyExpandFactor(int xFactor, int yFactor)
{
	double * buf = (double*)_TIFFmalloc(sizeof(double) * MPS_COUNT);
	
	buf[0] = MPS_DATA[0] * xFactor; 
	buf[1] = MPS_DATA[1] * yFactor;
	buf[2] = MPS_DATA[3];	// not supported - defualt is 0
	
	return buf;
}

/**
 * Write 'outBuffer' to single band tiff file at 'filePath'.
 */
int writeTIFF(float ** outBuffer, char * filePath, int imageLength, int imageWidth, int xFactor, int yFactor)
{
	printf("writeTIFF...\nMPS_COUNT : %d  MTT_COUNT : %d \n", MPS_COUNT, MTT_COUNT);
	
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
	//TIFFSetField(outFile, TIFFTAG_COMPRESSION, 		COMPRESSION_NONE);
	TIFFSetField(outFile, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
	TIFFSetField(outFile, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
	TIFFSetField(outFile, TIFFTAG_DATATYPE, 		3);
	TIFFSetField(outFile, TIFFTAG_PLANARCONFIG, 	PLANARCONFIG_CONTIG);
	TIFFSetField(outFile, TIFFTAG_ORIENTATION, 		ORIENTATION_TOPLEFT);
	
	//geo keys
	
	double * mpsData;
	mpsData = applyExpandFactor(xFactor, yFactor);
	
	TIFFSetField(outFile,	ModelPixelScaleTag,MPS_COUNT, mpsData);
	TIFFSetField(outFile,	ModelTiepointTag,  MTT_COUNT, MTT_DATA);
	
	setGeoKeysFromGTIFDefn(outGtif);
	
	int row;
	for(row = 0; row < imageLength; row++)
	{
		if(TIFFWriteScanline(outFile, outBuffer[row], row, 0) == -1)
		{
			PyErr_SetString(PyExc_IOError, "Error writing scanline to file.");
			return 0;
		}
	}
	
	GTIFFree(outGtif);
	XTIFFClose(outFile);
	
	return 1;
}
