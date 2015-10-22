
#include <Python/Python.h>

#include <String.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Memory.h"
#include "Bio_biometrics.h"
#include "Bio_square.h"

void getPointsFromFileSquare
(
	FILE *  filePtr,
	Node ** head,
	double  centroidEasting,
	double  centroidNorthing,
	int     sideLength,
	float   zThreshold,
	int *   numZ,
	int *   numAll
)
{
	puts("extractPointsFromFile");
	
	float s2 = sideLength / 2.0;
	
	double xmin, ymin, xmax, ymax;
	
	xmin = centroidEasting - s2;
	ymin = centroidNorthing - s2;
	xmax = centroidEasting + s2; 
	ymax = centroidNorthing + s2;
	
	ScanHdr * shdr = readTerraScanHdr(filePtr);
	int pnt;
	for(pnt = 0; pnt < shdr->PntCnt; pnt++){
		ScanPnt * spnt = getNextPnt(filePtr);
		double x = (double)((spnt->Pnt.x - shdr->OrgX) / (double)shdr->Units);
		double y = (double)((spnt->Pnt.y - shdr->OrgY) / (double)shdr->Units);
		double z = (double)((spnt->Pnt.z - shdr->OrgZ) / (double)shdr->Units);
		
		if(x > xmin && x < xmax && y > ymin && y < ymax)
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
		if(x > xmin && x < xmax && y > ymin && y < ymax)
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


float * getPointsFromFiles
(
	char ** files,
	int     numFiles,
	double  centroidEasting,
	double  centroidNorthing,
	int     sideLength,
	float   zThreshold,
	int *   numZ,
	int *   numAll
)
{
	printf("getPointsFromFiles: numfiles %d \n", numFiles);
	
	*numZ   = 0;
	*numAll = 0;
	
	Node * head = NULL;
	int    i;
	
	for(i = 0; i < numFiles; i++)
	{
		printf("\t%d %s\n", i, files[i]);
		FILE * file;
		
		if(!(file = fopen(files[i], "rb")))
		{
			continue;
		}
		getPointsFromFileSquare(file, &head, centroidEasting, centroidNorthing, sideLength, zThreshold, numZ, numAll);
		printf("\tnumz:%d\n",*numZ);
		printf("\tnuma:%d\n",*numAll);
		//printf("\thead->value:%f",head->value);
		//printf("\textractPoints : %d %d %f\n", *numZ, *numAll, head->value);
		
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
	freeList(head);
	
	return zValues;
}


char ** getFilesInBoundingSquare
(
	double centroidEasting, 
	double centroidNorthing,
	double minEasting,
	double minNorthing,
	int    sideLength,	
	int    blockSize,
	char * chmDir, 
	int  * fileCount
)
{
	puts("getFilesInBoundingSquare");
	float halfSide = sideLength / 2.0;
	
	double brMinE, brMinN, brMaxE, brMaxN;
	
	brMinE = centroidEasting - halfSide;
	brMinN = centroidNorthing - halfSide;
	brMaxE = centroidEasting + halfSide;
	brMaxN = centroidNorthing + halfSide;
	
	int xBlkMin, xBlkMax, yBlkMin, yBlkMax;
	
	xBlkMin = (int)floor( (brMinE - minEasting)  / blockSize );
	xBlkMax = (int)floor( (brMaxE - minEasting)  / blockSize );
	yBlkMin = (int)floor( (brMinN - minNorthing) / blockSize );
	yBlkMax = (int)floor( (brMaxN - minNorthing) / blockSize );
	
	*fileCount  = ((xBlkMax + 1) - xBlkMin) * ((yBlkMax + 1) - yBlkMin);
	
	char ** files, fname [256];
	
	if((files = init_char_Buffer2D(*fileCount, 256)) == NULL)
	{
		return NULL;
	}
	
	int x, y, idx;
	
	idx = 0;
	
	for(x = xBlkMin; x <= xBlkMax; x++)
	{
		for(y = yBlkMin; y <= yBlkMax; y++)
		{
			sprintf(fname, "CHM_%d_%d.bin", x, y);
				
			strcpy(files[idx], chmDir);
			strcat(files[idx], "/");
			strcat(files[idx], fname);
			
			printf("\t%d %s\n", idx, files[idx]);
			
			idx++;
		}
	}
	
	return files;
}

int writeSquareBiometrics
(
	int     point,
	char  * outPath, 
	float * bio,
	double  centroidEasting, 
	double  centroidNorthing, 
	int     sideLength
)
{
	printf("writeSquareBiometrics : start\n");
	
	FILE * outFile;
	
	if((outFile = fopen(outPath, "a")) == NULL)
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
)
{
	printf("boundingBoxBiometrics : start\n");
	int xBlockPosition, yBlockPosition;
	
	xBlockPosition = (int)floor((centroidEasting - minEasting) / blockSize);
	yBlockPosition = (int)floor((centroidNorthing - minNorthing) / blockSize );
	
	double blockEasting, blockNorthing;
	
	blockEasting  = minEasting  + (xBlockPosition * blockSize);
	blockNorthing = minNorthing + (yBlockPosition * blockSize);
	
	int     fileCount;
	char ** files = 
	getFilesInBoundingSquare
	(
		centroidEasting, 
		centroidNorthing,
		minEasting,
		minNorthing,
		sideLength,	
		blockSize,
		chmDir, 
		&fileCount
	);
	
	if(fileCount == 0)
	{
		return 0;
	}
	
	float * zValues, * bioArr;
	int     numZ, numAll;
	if(!(zValues = 
	getPointsFromFiles
	(
		files,
		fileCount,
		centroidEasting,
		centroidNorthing,
		sideLength,
		zThreshold,
		&numZ,
		&numAll
	)))
	{
		return 0;
	}
	
	//free the files buffer that was allocated
	
	if(!(bioArr = computeBiometrics(zValues, numZ, numAll, zThreshold)))
	{
		return 0;
	}
	
	if(!writeSquareBiometrics(point, outDir, bioArr, centroidEasting, centroidNorthing, sideLength))
	{
		return 0;
	}
	
	return 1;
}

