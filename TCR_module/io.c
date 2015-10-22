
#include "TCR_module.h"
#include <geo_normalize.h>

#define ModelPixelScaleTag	33550	//geotiff tag id
#define ModelTiepointTag	33922	//geotiff tag id

double ** buffFile(FILE * fp, int * nPtsPtr)
{
	double x, y, z;
	int numLines, line;
	
	numLines = 0;
	
	while(fscanf(fp, " %lf %lf %lf", &x, &y, &z) == 3)
	{
		++numLines;
	}
	
	*nPtsPtr = numLines;
	fseek(fp, 0, SEEK_SET);
	
	double ** points;
	if((points = allocd2d(3, numLines)) == NULL) return NULL;
	
	line = 0;
	
	while(fscanf(fp, "%lf %lf %lf", &x, &y, &z) == 3)
	{
		points[0][line] = x;
		points[1][line] = y;
		points[2][line] = z;
		++line;
	}
	
	return points;
}

int writeFile(FILE * fp, double ** CHM, int nCols)
{
	printf("In write file\n");
	int i;
	for(i = 0; i < nCols; i++)
	{
		if((fprintf(fp, "%.2lf %.2lf %.2lf\n", CHM[0][i], CHM[1][i], CHM[2][i])) < 0)
		{
			printf("Error writing to file : %d,%d\n", i, nCols);
			PyErr_SetString(PyExc_IOError, "Error writing CHM data to file.");
			return 0;
		}
		
	}
	
	return 1;
}


int writeToFile(FILE * fp, char * pc, double ** CHM, int * pNPoints, float hThreshold)
{
	int i, j = 0;
	
	for(i = 0; i < *pNPoints; i++)
	{
		/**
		 * Quick fix. Want points in the crown below the threshold to remain to be used in
		 * the post tree crown removal difference ccf calculations.
		 */
		if(pc[i] == 1 /* cond added. */ && (float)CHM[2][i] >= hThreshold /* end cond add */)	//point i is within the tree crown
		{
			/** commented out for quick fix
			if((float)CHM[2][i] < hThreshold) //point i is within the tree crown and in the undergrowth
			{
				if((fprintf(fp, "%.2lf %.2lf %.2lf\n", CHM[0][i], CHM[1][i], CHM[2][i])) < 0)
				{
					PyErr_SetString(PyExc_IOError, "Error writing CHM data to file.");
					return 0;
				}
			}
			*/
		}
		else
		{
			CHM[0][j] = CHM[0][i];
			CHM[1][j] = CHM[1][i];
			CHM[2][j] = CHM[2][i];
			++j;
		}
	}
	
	*pNPoints = j;
	
	return 1;
}

/**
 * out - buffer to be written
 * filePath - tiff to write to
 * numBands - number of bands
 * numDims - image width and height
 * outRes - size of each pixel
 * xTiePoint - x coordinate of pixel (0,0)
 * yTiePoint - y coordinate of pixel (0,0)
 */
int writeTiff(float *** out, char * filePath, int numBands, int numDims, int outRes, double xTiePoint, double yTiePoint)
{
	printf("Writing TIFF to file. \n");
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
	
	TIFFSetField(outFile, TIFFTAG_IMAGEWIDTH, 		numDims);
	TIFFSetField(outFile, TIFFTAG_IMAGELENGTH, 		numDims);
	TIFFSetField(outFile, TIFFTAG_BITSPERSAMPLE, 	32);
	TIFFSetField(outFile, TIFFTAG_SAMPLESPERPIXEL, 	numBands);
	TIFFSetField(outFile, TIFFTAG_COMPRESSION, 		COMPRESSION_NONE);
	TIFFSetField(outFile, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
	TIFFSetField(outFile, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
	TIFFSetField(outFile, TIFFTAG_DATATYPE, 		3);
	TIFFSetField(outFile, TIFFTAG_PLANARCONFIG, 	PLANARCONFIG_SEPARATE);
	TIFFSetField(outFile, TIFFTAG_ORIENTATION, 		ORIENTATION_TOPLEFT);
	
	double mps [3] = {outRes, outRes, 1.0};
	double mtt [6] = {0.0, 0.0, 0.0, xTiePoint, yTiePoint, 0.0};
	
	TIFFSetField(outFile,	ModelPixelScaleTag, 3, mps);
	TIFFSetField(outFile,	ModelTiepointTag,	6, mtt);
	
	//still missing some geo tag info but will see if pixel scale and tiepoints will be enough
	
	int band, row, imgRow;
	
	for(band = 0; band < numBands; band++)
	{
		imgRow = 0;
		
		for(row = numDims-1; row >= 0; row--)
		{
			if(TIFFWriteScanline(outFile, out[band][row], imgRow++, band) == -1)
			{
				PyErr_SetString(PyExc_IOError, "Error writing scanline to file.");
				return 0;
			}
		}
	}
	
	GTIFFree(outGtif);
	TIFFClose(outFile);
	
	return 1;
}

