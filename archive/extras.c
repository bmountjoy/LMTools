/****
*** From Bio_rename.c **/

/**
 * Returns the points with extreme z values removed.
 */
double ** byeByeBirdie(double ** points, int numPts, int * numOutPts)
{
	puts("byeByeBirdie : Removing points whose z-value >= 100 m");
	
	int i, count;
	
	count = 0;
	
	//count the number of valid points
	for(i = 0; i < numPts; i++)
	{
		if (points[2][i] < 100.0) {
			count++;
		}
	}
	*numOutPts = count;
	
	printf("Out points : %d, In points: %d \n", *numOutPts, numPts);
	
	double ** validPts;
	if((validPts = initDoubleBuffer2D(3, count, 0.0)) == NULL)
	{
		return validPts;
	}
	
	int ii = 0;
	
	//assign valid outpoint to pts
	for(i = 0; i < numPts; i++)
	{
		if (points[2][i] < 100.0) 
		{
			validPts[0][ii] = points[0][i];
			validPts[1][ii] = points[1][i];
			validPts[2][ii] = points[2][i];
			ii++;
		}
	} 
	
	return validPts;
}

void writeOut(char * outPath, double ** points, int numPts)
{
	puts("writeOut : Writing renamed file.");
	
	FILE * fp;
	
	if( (fp = fopen(outPath, "w")) == NULL ) return;
	
	char str [32];
	
	int i;
	for(i = 0; i < numPts; i++) 
	{
		sprintf(str, "%f", points[0][i]);//convert double to string
		fwrite(str, 1, strlen(str), fp); //write double to file
		
		fputc(' ', fp);
		
		sprintf(str, "%f", points[1][i]);
		fwrite(str, 1, strlen(str), fp);
		
		fputc(' ', fp);
		
		sprintf(str, "%f", points[2][i]);
		fwrite(str, 1, strlen(str), fp);
		
		fputc('\n', fp);
	}
	fclose(fp);
}

void renameFile(char * filePath, char * outDir, double minEasting, double minNorthing, int blkSize)
{
	printf("renameFile : \n\tPutting : %s\n\tIn : %s\n", filePath, outDir);
	
	char outPath [256], fName [32];
	
	FILE * fp;
	
	int numPts, x, y;
	
	if((fp = fopen(filePath, "r")) == NULL)
		return;
		
	double ** origPoints, ** filteredPoints;
	
	if((origPoints = bufferFile(fp, &numPts)) == NULL)
		return;
	
	if(numPts <= 0)
		return;
	
	filteredPoints =
	byeByeBirdie(origPoints, numPts, &numPts); //ensure this is legit
	
	freeDoubleBuffer2D(origPoints, 3);
	
	if(numPts <= 0) 
		return;
	
	getXandY(filteredPoints[0][0], filteredPoints[1][0], minEasting, minNorthing, blkSize, &x, &y);
	
	sprintf(fName, "CHM_%d_%d.txt", x, y);
	strcpy(outPath, outDir);
	strcat(outPath, "/");
	strcat(outPath, fName);
	
	writeOut(outPath, filteredPoints, numPts);
	
	freeDoubleBuffer2D(filteredPoints, 3);
}


/** End of Bio_rename.c **/

/**
 * points : 3 x num_points
 * points[0] : all x co-ords
 * points[1] : all y co-ords
 * points[2] : all z co-ords
 */
void calcBiometrics(
					
					double **points, 		// 3 x numPoints array
					int      numPoints, 	//the number of points in 'points'
					float    minEasting, 	//easting of bottome left corner of grid
					float    minNorthing, 	//norhting of bottom left corner of grid
					int      blockSize,		//block dimension in meters
					float    xBlock,		//x block position in grid
					float    yBlock,		//y block position in grid
					int      outRes, 		//resolution of output
					int      zThreshold		//z threshold value
					
					)
{
	float *** outArr;
	float *** lhqArr;
	float *** ccfArr;
	float blockEasting, blockNorthing;
	int dimensions;
	
	dimensions 	  = blockSize / outRes; //must be an integer
	blockEasting  = minEasting + (xBlock * blockSize);
	blockNorthing = minNorthing + (yBlock * blockSize);
	
	/** ouput arrays **/
	outArr = initFloatBuffer3D(8 , dimensions, dimensions, -999.0);
	lhqArr = initFloatBuffer3D(21, dimensions, dimensions, -999.0);
	ccfArr = initFloatBuffer3D(21, dimensions, dimensions, -999.0);
	
	float xStart, yStart, xEnd, yEnd;
	int j, i;
	
	for(j = 0; j < dimensions; j++){
		for(i = 0; i < dimensions; i++)
		{
			xStart = blockEasting + outRes * j;
			yStart = blockNorthing + outRes * i;
			xEnd   = xStart + outRes;
			yEnd   = yStart + outRes;
			
			Array1D A;
			
			A = filterByArea(points, numPoints, xStart, yStart, xEnd, yEnd);
			if(A.size == 0)
				continue;
			
			int     numAll = A.size;
			
			A = filterByZThreshold(A.points, A.size, zThreshold);
			
			float * z_threshold = A.points;
			int     numThresh   = A.size;
			
			if(numThresh == 0)
				continue;
			if(numThresh <= zThreshold)// why is this necessary
				continue;
			
			qsort(z_threshold, numThresh, sizeof(float), floatCompare);
			
			computeRugosity(z_threshold, numThresh, &outArr[0][j][i]);
			computeGap(numThresh, numAll, &outArr[1][j][i]);
			computePercentile(z_threshold, numThresh, 0.85, &outArr[2][j][i]);
			
			if(A.size < 75)
				continue;
			
			float * lMoments =
			computeLMoments(z_threshold,  numThresh);
			outArr[3][j][i] = lMoments[0];
			outArr[4][j][i] = lMoments[1];
			outArr[5][j][i] = lMoments[2];
			outArr[6][j][i] = lMoments[3];
			
			/*****************************************************************************
			 *********************** LHQ Statistics **************************
			 *****************************************************************/
			
			int band, rank1, rank2;
			float fRank, iRank, difRank;
			for(band = 0; band <= 20; band++)
			{
				fRank   = (band/20.0) * (numThresh + 1);
				iRank   = (int)fRank;			//Todo: make sure case truncates
				difRank = fRank - iRank;
				rank1   = iRank - 1;
				if(rank1 < 0)
					rank1 = 0;
				rank2 = iRank;
				
				if(band < 20)
				{
					lhqArr[band][j][i] = (1 - difRank) * z_threshold[rank1] + difRank * z_threshold[rank2];
				}
				else
				{
					lhqArr[band][j][i] = z_threshold[numThresh-1]; // max.. this sorted so it should be max
				}
				
				if( band == 17 ) 
				{
					outArr[7][j][i] = lhqArr[band][j][i];
				}
			}
			
			/*****************************************************************************
			 *********************** CCF Statistics **************************
			 *****************************************************************/
			
			int numDivs, ccfCount;
			float minHeight, maxHeight, curHeight, ccfPercent, htIncrement;
			
			minHeight = zThreshold;
			maxHeight = getMax(z_threshold, numThresh); // = z_threshold[numThresh-1]
			numDivs   = 20;
			htIncrement = (maxHeight - minHeight) / numDivs;
			
			for(band = 0; band <= 20; band++)
			{
				curHeight  = minHeight + (htIncrement * band);
				ccfCount   = getCCFCount(z_threshold, numThresh, curHeight);
				ccfPercent = ccfCount / numAll;
				
				ccfArr[band][j][i] = ccfPercent;
			}
		}
	}
}


/**
 * Used in TM_module.c to open input files. Assumes the input file may be either
 * a tiff or envi standard file. Function looks for the accompanying header file
 * and if found uses that to read the input file.
 */
int readFile(char * filePath)
{
	printf("readFile...\n");
	
	char hPath [128], type [5];
	
	if(strchr(filePath, '.') == NULL)
	{
		strcpy(hPath, filePath);
		strcat(hPath, ".hdr");
	}
	else
	{
		int offset = 
		strrchr(filePath, '.')-filePath;
		strncpy(hPath, filePath, offset);
		hPath[offset] = '\0';
		strcat(hPath, ".hdr");
		
		strcpy(type, (filePath + offset));
		if( (strcmp(type, ".tiff") != 0) && (strcmp(type, ".tif") != 0) )
		{
			PyErr_SetString(PyExc_ValueError, "File not a TIFF.");
			return 0;
		}
	}
	
	printf("Header path : %s \n", hPath);
	printf("access : %d\n", access(hPath, R_OK));
	
	// Potential problem with PC compatability
	if(access(hPath, R_OK) != -1)
	{
		if( (IMAGE_BUFFER = readENVIStandard(hPath, filePath)) == NULL)
		{
			return 0;
		}
	}
	else
	{
		if(!readTIFF(filePath))
			return 0;
	}
	return 1;
}

/****************** Will hopefully remove this function. ***********************
 * Read image strips and set global variablels.
 *	- NUMBER_OF_ROWS / IMAGE_LENGTH
 *	- NUMBER_OF_COLS / IMAGE_WIDTH
 *	- IMAGE
 */
void stripIO(TIFF * tif)
{
	uint16 config;
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	
	int stripSize, stripMax;
	stripSize = TIFFStripSize (tif);
	stripMax  = TIFFNumberOfStrips (tif);
	
	int bytesPerStrip;
	TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &bytesPerStrip);
	
	printf("stripSize : %d  stripMax : %d bytesPerStrip : %d\n", stripSize, stripMax, bytesPerStrip);
	
	unsigned long bufferSize;
	bufferSize = stripSize * stripMax;
	
	char * buffer;
	if((buffer = (char *) _TIFFmalloc(bufferSize)) == NULL){
	    puts("Not enough memory to allocate buffer.");
	    exit(42);
	}
	
	int stripCount, result;
	unsigned long imageOffset = 0;
	for(stripCount = 0; stripCount < stripMax; stripCount++)
	{
	    if((result = TIFFReadRawStrip(tif, stripCount, buffer + imageOffset, stripSize)) == -1){
		    puts("Read error on input strip.");
	    }
	}
	
	int i;
	for(stripCount = 0; stripCount < stripMax; stripCount++){
	    for(i = 0; i < stripSize; i++){
		    //printf("%d ", atoi(&buffer[stripCount*stripSize + stripSize]));
	    }
	    //printf("\n");
	}
	
	_TIFFfree(buffer);
}

/**
 * Unused statistics
 */

float calcSkewness(float * array, int size)
{
	float mean, variance, diff_sum = 0.0;
	int i;
	
	mean     = calcMean(array, size);
	variance = calcVariance(array, size);
	variance = pow( variance, 0.5 );
	
	for(i = 0; i < size; i++)
	{
		diff_sum += pow( (array[i] - mean) / variance, 3);
	}
	
	return (diff_sum / size) ;
}
float calcKurtosis(float * array, int size)
{
	float mean, variance, diff_sum = 0.0;
	int i;
	
	mean     = calcMean(array, size);
	variance = calcVariance(array, size);
	variance = pow( variance, 0.5 );
	
	for(i = 0; i < size; i++)
	{
		diff_sum += pow( (array[i] - mean) / variance, 4);
	}
	
	return (diff_sum / size) - 3;
}


/* Read in image tiles tile by tile ****************************/
for(tile = 0; tile < numberOfTiles; tile++)
{
	if( (tiles[tile] = (float *) _TIFFmalloc(tileSize)) == NULL )
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return 0;
	}
	TIFFReadEncodedTile(tif, tile, tiles[tile], tileSize);
}

/*******************************************************************************


printf("Writing scanline_out to file.\n");
	
	TIFF * scanline_out;
	if((scanline_out = TIFFOpen("/Users/Ben/Desktop/out/scanline_out.tif", "w")) == NULL){
		printf("Error opening %s.", "/Users/Ben/Desktop/out/scanline_out.tif");
		exit(0);
	}
	
	TIFFSetField(scanline_out, TIFFTAG_IMAGEWIDTH, 		IMAGE_WIDTH);
	TIFFSetField(scanline_out, TIFFTAG_IMAGELENGTH, 	IMAGE_LENGTH);
	TIFFSetField(scanline_out, TIFFTAG_BITSPERSAMPLE, 	32);
	TIFFSetField(scanline_out, TIFFTAG_COMPRESSION, 	COMPRESSION_NONE);
	TIFFSetField(scanline_out, TIFFTAG_PHOTOMETRIC, 	PHOTOMETRIC_MINISBLACK);
	TIFFSetField(scanline_out, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
	TIFFSetField(scanline_out, TIFFTAG_DATATYPE, 		3);
	
	for(row = 0; row < NUMBER_OF_ROWS; row++)
	{
		if( TIFFWriteScanline(scanline_out, IMAGE[row], row, 0) == -1){
			printf("Error detected writing scanline_out. Attempting to operate normally. \n");
			break;
		}
	}
	TIFFClose(scanline_out);
	
	

int setup(char * filePath, int numberOfBands)
{
	printf("Running setup...\n");
	
	NUMBER_OF_BANDS = numberOfBands; // may become obsolete
	
	MEDIAN_FLAG		= 0;
	
	IMAGE_WIDTH = IMAGE_LENGTH = NUMBER_OF_ROWS = NUMBER_OF_COLS = 0;
	
	if( (OUT_BUFFER = (float ***) _TIFFmalloc(sizeof(float **) * NUMBER_OF_BANDS)) == NULL)
	{
		free_OUTPUT_BUFFER(0,0);
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return 0;
	}
	
	if(!readFile(filePath))
		return 0;
	
	printf("\tInitializing output buffer ... \n");
	int band, row, col;
	for(band = 0; band < NUMBER_OF_BANDS; band++)
	{
		if( (OUT_BUFFER[band] = (float **) _TIFFmalloc(sizeof(float *) * NUMBER_OF_ROWS)) == NULL )
		{
			free_OUTPUT_BUFFER(band, NUMBER_OF_ROWS);
			PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
			return 0;
		}
		
		for(row = 0; row < NUMBER_OF_ROWS; row++)
		{
			if( (OUT_BUFFER[band][row] = (float *) _TIFFmalloc(sizeof(float) * NUMBER_OF_COLS)) == NULL)
			{
				free_OUTPUT_BUFFER(band+1, row);
				PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
				return 0;
			}
			
			for(col = 0; col < NUMBER_OF_COLS; col++)
			{
				if( FLOW_FLAG && (band == 0))
					OUT_BUFFER[band][row][col] = 1.0;
				else
					OUT_BUFFER[band][row][col] = 0.0;
			}
		}
	}
	return 1;
}

//TODO : get rid of this data structure
char * outputFiles [] = {
	"/Users/Ben/Desktop/out/TM_flow.tif",
	"/Users/Ben/Desktop/out/TM_flow_Dinf.tif",
	"/Users/Ben/Desktop/out/TM_grad.tif",
	"/Users/Ben/Desktop/out/TM_aspect.tif",
	"/Users/Ben/Desktop/out/TM_dcurv.tif",
	"/Users/Ben/Desktop/out/TM_acurv.tif"
};


static PyObject* writeTIFF(PyObject* self, PyObject* args)
{
	printf("Writing master file ... \n");
	TIFF * outFile;
	if((outFile = TIFFOpen("/Users/Ben/Desktop/out/master_out.tif", "w")) == NULL)
	{
		PyErr_SetString(PyExc_IOError, "Could not open output file.");
		return NULL;
	}

	int band;
	for(band = 0; band < NUMBER_OF_BANDS; band++)
	{
		printf("\tWriting band %d\n", band+1);
		
		TIFFSetField(outFile, TIFFTAG_IMAGEWIDTH, 		IMAGE_WIDTH);
		TIFFSetField(outFile, TIFFTAG_IMAGELENGTH, 		IMAGE_LENGTH);
		TIFFSetField(outFile, TIFFTAG_BITSPERSAMPLE, 	32);
		TIFFSetField(outFile, TIFFTAG_SAMPLESPERPIXEL, 	1);
		TIFFSetField(outFile, TIFFTAG_COMPRESSION, 		COMPRESSION_NONE);
		TIFFSetField(outFile, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
		TIFFSetField(outFile, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
		TIFFSetField(outFile, TIFFTAG_DATATYPE, 		3);
		TIFFSetField(outFile, TIFFTAG_PLANARCONFIG, 	PLANARCONFIG_CONTIG);
        TIFFSetField(outFile, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
        TIFFSetField(outFile, TIFFTAG_ORIENTATION, 		ORIENTATION_TOPLEFT);
        TIFFSetField(outFile, TIFFTAG_SUBFILETYPE, 		FILETYPE_PAGE);
		TIFFSetField(outFile, TIFFTAG_PAGENUMBER, band, NUMBER_OF_BANDS);
		
		int row;
		for(row = 0; row < NUMBER_OF_ROWS; row++)
		{
			if(TIFFWriteScanline(outFile, OUT_BUFFER[band][row], row, 0) == -1)
			{
				PyErr_SetString(PyExc_IOError, "Error writing scanline to output.");
				return NULL;
			}
		}
		if(!TIFFWriteDirectory(outFile)) 
		{
			PyErr_SetString(PyExc_IOError, "Error writing directory to output.");
			return NULL;
		}
	}
	TIFFClose(outFile);
	
	/** TESTING **
	TIFF * in;
	if((in = TIFFOpen("/Users/Ben/Desktop/out/master_out.tif", "r")) == NULL){
		PyErr_SetString(PyExc_IOError, "Could not open master out.");
		return NULL;
	}
	
	int dircount = 0;
	uint32 imglen, r;
	tdata_t buf;
	
	do{
		TIFF * out;
		if((out = TIFFOpen(outputFiles[dircount++], "w")) == NULL)return 0;
		TIFFSetField(out, TIFFTAG_IMAGEWIDTH, 		IMAGE_WIDTH);
		TIFFSetField(out, TIFFTAG_IMAGELENGTH, 		IMAGE_LENGTH);
		TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 	32);
		TIFFSetField(out, TIFFTAG_COMPRESSION, 		COMPRESSION_NONE);
		TIFFSetField(out, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
		TIFFSetField(out, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
		TIFFSetField(out, TIFFTAG_DATATYPE, 		3);
		
		TIFFGetField(in, TIFFTAG_IMAGELENGTH, &imglen);
		buf = _TIFFmalloc(TIFFScanlineSize(in));
		for(r = 0; r < imglen; r++){
			TIFFReadScanline(in, buf, r, 0);
			TIFFWriteScanline(out, buf, r, 0);
		}
		TIFFClose(out);
		
	}while(TIFFReadDirectory(in));
	printf("The output file has %d directories.\n", dircount);
	TIFFClose(in);
	/*************
	
	Py_INCREF(Py_None);
	return Py_None;
}

*******************************************************************************/
