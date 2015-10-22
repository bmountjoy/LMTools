/*
 *  Bio_rename.c
 *  
 *
 *  Created by Ben Mountjoy on 11-08-02.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <Python/Python.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "Bio_rename.h"

void getXandY
(	
	double easting, 
	double northing, 
	double minEasting, 
	double minNorthing,
	int blockSize,
	int * x, 
	int * y
)
{
	puts("getXandY : Determing x and y components of block position.");
	
	*x = (int)floor( (easting - minEasting)   / blockSize );
	*y = (int)floor( (northing - minNorthing) / blockSize );
}


char * renameFile(char * filePath, char * outDir, double minEasting, double minNorthing, int blkSize, float zThreshold)
{
	printf("renameFile\n\tRenaming : %s \n", filePath);
	
	FILE * inFile, * outFile;
	
	if((inFile = fopen(filePath, "r")) == NULL)
	{
		return NULL;
	}
	
	double x, y, z;
	int    xpos, ypos;
	
	if(fscanf(inFile, "%lf %lf %lf", &x, &y, &z) != 3)
	{
		return NULL;
	}
	
	fseek(inFile, 0, SEEK_SET);
	
	getXandY(x, y, minEasting, minNorthing, blkSize, &xpos, &ypos);
	
	char * outPath, fName [32];
	
	if((outPath = (char *)malloc(sizeof(char) * 128)) == NULL)
	{
		return NULL;
	}
	
	sprintf(fName, "CHM_%d_%d.txt", xpos, ypos);
	strcpy (outPath, outDir);
	strcat (outPath, "/");
	strcat (outPath, fName);
	
	printf("Renamed to : %s\n", outPath);
	
	if((outFile = fopen(outPath, "w")) == NULL)
	{
		return NULL;
	}
	
	char str [32];
	
	while(fscanf(inFile, "%lf %lf %lf", &x, &y, &z) == 3)
	{
		if(z >= zThreshold)
		{
			printf(" z : %lf \n", z);
			continue;
		}
		
		sprintf(str, "%.2f", x);//convert double to string
		fwrite(str, 1, strlen(str), outFile); //write double to file
		
		fputc(' ', outFile);
		
		sprintf(str, "%.2f", y);
		fwrite(str, 1, strlen(str), outFile);
		
		fputc(' ', outFile);
		
		sprintf(str, "%.2f", z);
		fwrite(str, 1, strlen(str), outFile);
		
		fputc('\n', outFile);
	}
	fclose(inFile);
	fclose(outFile);
	
	return outPath;
}