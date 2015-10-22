
#include <Python/Python.h>

#include <String.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Bio_biometrics.h"
#include "Bio_centroid.h"
#include "Memory.h"

/**
 * Add all points from file 'filePtr' that are within 'radius' distance from
 * the point specified by ('centroidEast', 'centroidNorth') to the list
 * specified by 'head'.
 */
void getPointsFromFile
(
	FILE * filePtr,
	Node **head,
	double centroidEasting,
	double centroidNorthing,
	int    radius,
	float  zThreshold, 
	int *  numZ,
	int *  numAll
)
{	
	puts("getPointsFromFile");
	
	double  dsqrd, rsqrd;
	
	rsqrd = radius * radius;
	
	
	ScanHdr * shdr = readTerraScanHdr(filePtr);
	int pnt;
	for(pnt = 0; pnt < shdr->PntCnt; pnt++){
		ScanPnt * spnt = getNextPnt(filePtr);
		double x = (double)((spnt->Pnt.x - shdr->OrgX) / (double)shdr->Units);
		double y = (double)((spnt->Pnt.y - shdr->OrgY) / (double)shdr->Units);
		double z = (double)((spnt->Pnt.z - shdr->OrgZ) / (double)shdr->Units);
		double x_dist = x - centroidEasting;
		double y_dist = y - centroidNorthing;
		dsqrd = x_dist * x_dist + y_dist * y_dist; 
		//printf("%lf,%lf,%lf\n", x,y,z);
		//printf("%lf\n",dsqrd);
		if(dsqrd <= rsqrd)
		{
			(*numAll)++;
			if(z > zThreshold){
				addFront(head, (float)z);
				(*numZ)++;
			}
		}
		
		if(shdr->Time)fseek(filePtr, 4, SEEK_CUR);
		if(shdr->Color)fseek(filePtr, 4, SEEK_CUR);
		free(spnt);
	}
	free(shdr);
	
	/*
	while(fscanf(filePtr, "%lf %lf %lf", &x, &y, &z) == 3)
	{
		float xdist = x - centroidEasting;
		float ydist = y - centroidNorthing;
		
		dsqrd = xdist * xdist + ydist * ydist;
		
		if(dsqrd <= rsqrd)
		{
			(*numAll)++;
			if(z > zThreshold)
			{
				addFront( head, (float) z );
				(*numZ)++;
			}
		}
	}
	*/
	printf("\t%d %d \n", *numZ, *numAll);
}

float * extractPoints
(
	char ** files,
	int     numFiles,
	double  centroidEasting,
	double  centroidNorthing,
	int     radius,
	float   zThreshold,
	int *   numZ,
	int *   numAll
)
{
	printf("extractPoints : numfiles %d \n", numFiles);
	
	*numZ   = 0;
	*numAll = 0;
	
	Node * head = NULL;
	int    i;
	
	for(i = 0; i < numFiles; i++)
	{
		printf("\t%d %s\n", i, files[i]);
		FILE * file;
		
		if(!(file = fopen(files[i], "r")))
		{
			continue;
		}
		getPointsFromFile(file, &head, centroidEasting, centroidNorthing, radius, zThreshold, numZ, numAll);
		if(head != NULL)
			printf("\textractPoints : %d %d %f\n", *numZ, *numAll, head->value);
		else
			printf("\textractPoints : %d %d\n", *numZ, *numAll);
		
		fclose(file);
	}
	
	/**
	 * Copy list to a float array.
	 */
	 
	float * zValues;
	
	if((zValues = initFloatBuffer1D(*numZ, -999.0)) == NULL)
	{
		return NULL;
	}
	
	Node * curr;
	
	curr = head;
	i = 0;
	
	puts("About to free the list");
	
	while(curr)
	{
		zValues[i++] = curr->value;
		curr = curr->next;
	}
	
	//free(head);
	freeList(head);
	
	return zValues;
}

char ** computeBlocksInRadius
( 
	double centroidEast, 
	double centroidNorth,
	int centroidXBlock,
	int centroidYBlock,
	double blockEast,	//easting of bottom left corner 
	double blockNorth,	//northing of bottom left corner
	int blockSize,
	int radius, 
	int * fileCount,	//used by the calling function
	char * chmDir
)
{
	puts("computeBlocksInRadius");
	
	float dSqrdArr [9];
	int xOffsets [9], yOffsets[9];
	
	xOffsets[0] = -1;
	xOffsets[1] = -1;
	xOffsets[2] = 0;
	xOffsets[3] = 1;
	xOffsets[4] = 1;
	xOffsets[5] = 1;
	xOffsets[6] = 0;
	xOffsets[7] = -1;
	xOffsets[8] = 0;
	
	yOffsets[0] = 0;
	yOffsets[1] = 1;
	yOffsets[2] = 1;
	yOffsets[3] = 1;
	yOffsets[4] = 0;
	yOffsets[5] = -1;
	yOffsets[6] = -1;
	yOffsets[7] = -1;
	yOffsets[8] = 0;
	
	float distMinEast, distMinNorth, distMaxEast, distMaxNorth, 
			distMinEastSqrd, distMinNorthSqrd, distMaxEastSqrd, distMaxNorthSqrd;
	
	distMinEast  = centroidEast - blockEast;
	distMaxEast  = (blockEast + blockSize) - centroidEast; 
	distMinNorth = centroidNorth - blockNorth; 
	distMaxNorth = (centroidNorth + blockSize) - centroidNorth;
	
	distMinEastSqrd  = distMinEast * distMinEast;
	distMaxEastSqrd  = distMaxEast * distMaxEast; 
	distMinNorthSqrd = distMinNorth * distMinNorth; 
	distMaxNorthSqrd = distMaxNorth * distMaxNorth;
	
	dSqrdArr[0] = distMinEastSqrd;
	dSqrdArr[1] = distMinEastSqrd + distMaxNorthSqrd;
	dSqrdArr[2] = distMinNorthSqrd;
	dSqrdArr[3] = distMaxEastSqrd + distMaxNorthSqrd;
	dSqrdArr[4] = distMaxEastSqrd;
	dSqrdArr[5] = distMaxEastSqrd + distMinNorthSqrd;
	dSqrdArr[6] = distMinNorthSqrd;
	dSqrdArr[7] = distMinEastSqrd + distMinNorthSqrd;
	dSqrdArr[8] = 0;
	
	float rSqrd = radius * radius;
		
	int i, count;
	
	count = 0;
	
	for(i = 0; i < 9; i++)
	{
		if(dSqrdArr[i] < rSqrd && centroidXBlock + xOffsets[i] >= 0 && centroidYBlock + yOffsets[i] >=0)
		{
			++count;
		}
	}
	*fileCount = count;
	
	char ** files;
	
	if( (files = init_char_Buffer2D(count, 128)) == NULL)
	{
		return NULL;
	}
	
	char fname[32];
	int  fIdx = 0;

	for(i = 0; i < 9; i++)
	{
		if(dSqrdArr[i] < rSqrd)
		{
			if(centroidXBlock + xOffsets[i] < 0 || centroidYBlock + yOffsets[i] < 0)
				continue;
			
			sprintf(fname, "CHM_%d_%d.bin", centroidXBlock + xOffsets[i], centroidYBlock + yOffsets[i]);
			printf("%s\n",fname);
			strcpy(files[fIdx], chmDir);
			strcat(files[fIdx], "/");
			strcat(files[fIdx], fname);
			printf("%s\n",files[fIdx]);
			
			fIdx++;
		}
	}
	
	return files;
	
}//end function

int writeBiometrics
(
	int     point,
	char  * outPath, 
	float * bio,
	double  centroidEasting, 
	double  centroidNorthing, 
	int     radius
)
{
	FILE * outFile;
	
	outFile = fopen(outPath, "a");
	if(!outFile)
	{
		PyErr_SetString(PyExc_IOError, "Could not open output file - fopen() failed.");
		return 0;
	}
	
	fprintf(outFile, "%d %.2lf %.2lf ", point, centroidEasting, centroidNorthing);
	
	int i;
	for(i = 0; i < 50; i++)
	{
		fprintf(outFile, "%f ", bio[i]);
	}
	
	fputc('\n', outFile);
	fclose(outFile);
	
	return 1;	
}

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
	char * outFile
)
{
	int xBlock, yBlock;
	
	xBlock = (int)floor((centroidEasting - minEasting) / blockSize);
	yBlock = (int)floor((centroidNorthing - minNorthing) / blockSize );
	
	double minBlockEasting, minBlockNorthing;
	
	minBlockEasting = minEasting + (xBlock * blockSize);
	minBlockNorthing = minNorthing + (yBlock * blockSize);
	
	char ** files;
	int     fileCount;
	
	if(!(
		files = 
		computeBlocksInRadius
		(
			centroidEasting, 
			centroidNorthing, 
			xBlock, 
			yBlock, 
			minBlockEasting, 
			minBlockNorthing, 
			blockSize, 
			radius,
			&fileCount,
			chmDir
		)
	))
		return 0;
	
	float * zValues, * bioArr;
	int     numZ, numAll;
	
	zValues = extractPoints(files, fileCount, centroidEasting, centroidNorthing, radius, zThreshold, &numZ, &numAll);	
	if(zValues == NULL)
	{
		return 0;
	}
	
	bioArr = computeBiometrics(zValues, numZ, numAll, zThreshold);
	if(bioArr == NULL)
	{
		return 0;
	}
	
	if(!writeBiometrics(point, outFile, bioArr, centroidEasting, centroidNorthing, radius))
	{
		return 0;
	}
	
	return 1;
}


