
#include <Python/Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TerraScanBin.h"


/**
 * Read the the header from the TerraScan binary file pointed to by 
 * 'fptr' and sets the 'fptr' to point to the first point in the file.'
 */
ScanHdr * readTerraScanHdr(FILE * fptr)
{
	fseek(fptr, 0, SEEK_SET);
	ScanHdr temp;
	ScanHdr * 
	shdr = (ScanHdr *)malloc(sizeof(ScanHdr));
	
	memset(shdr, 0, sizeof(ScanHdr));
	
	if( fread(&temp, sizeof(ScanHdr), 1, fptr) != 1 ){
		PyErr_SetString(PyExc_IOError, "Failed to read terrascan header.");
		return 0;
	}
	
	memcpy(shdr, &temp, temp.HdrSize);
	fseek(fptr, shdr->HdrSize, SEEK_SET);
	
	//print header info
	puts("readTerraScanHdr");
	printf("\tSize of Header:%d\n", (int)sizeof(ScanHdr));
	printf("Header Info:\n");
	printf("\tHdrSize:%d\n",shdr->HdrSize);
	printf("\tHdrVersion:%d\n",shdr->HdrVersion);
	printf("\tRecogVal:%d\n",shdr->RecogVal);
	printf("\tRecogStr:%c%c%c%c\n",shdr->RecogStr[0],shdr->RecogStr[1],shdr->RecogStr[2],shdr->RecogStr[3]);
	printf("\tPntCnt:%d\n",shdr->PntCnt);
	printf("\tUnits:%d\n",shdr->Units);
	printf("\tOrgX:%lf\n",shdr->OrgX);
	printf("\tOrgY:%lf\n",shdr->OrgY);
	printf("\tOrgZ:%lf\n",shdr->OrgZ);
	printf("\tTime:%d\n",shdr->Time);
	printf("\tColor:%d\n",shdr->Color);
	
	return shdr;	
}



/**
 * Reads the next pont in the binary TerranScan file pointed to by 'fptr'.
 * Note, this function does not consume the optional time stamp and color
 * after the point record.
 * 
 * Parameters:
 * - fptr -- expected to be pointing to a valid entry
 * - shdr -- valid header returned by 'readTerraScanHdr'
 *
 * Return Value:
 * If this function returns null then either there was an error or the 
 * end of the file was reached.
 */
ScanPnt * getNextPnt(FILE * fptr)
{
	ScanPnt * 
	spnt = (ScanPnt *)malloc(sizeof(ScanPnt));
	if(!spnt){
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	if( fread(spnt, sizeof(ScanPnt), 1, fptr) != 1 ){
		PyErr_SetString(PyExc_IOError, "failed to read point.");
		return NULL;
	}
	
	return spnt;
}

/**
 * Buffers all of the points in the file skipping points whose code
 * (classification) is not equal to 1.
 */
double ** bufferTerraScanBinFile(char * path, int * p_npts)
{
	puts("bufferTerraScanBinFile");
	
	FILE * fp_bin = fopen(path, "rb");
	if(!fp_bin){
		PyErr_SetString(PyExc_IOError, "fopen failed.");
		return NULL;
	}
	
	ScanHdr * shdr = readTerraScanHdr(fp_bin);
		if(!shdr)return NULL;
		
	int pnt, n_pts = 0, curr = 0;
	
	//count the number of default points
	for(pnt = 0; pnt < shdr->PntCnt; pnt++)
	{
		ScanPnt spnt;
		fread(&spnt, sizeof(ScanPnt), 1, fp_bin);
		
		if(spnt.Code == 1)
			n_pts++;
		
		if(shdr->Time)
			fseek(fp_bin, 4, SEEK_CUR);
		if(shdr->Color)
			fseek(fp_bin, 4, SEEK_CUR);
	}
	printf("\tn_pts:%d\n", n_pts);
	*p_npts = n_pts;
	
	//initialize buffer for each default point
	double ** pts = allocd2d(3, n_pts);
		if(!pts) return NULL;
	
	//reset file pointer
	fseek(fp_bin, shdr->HdrSize, SEEK_SET);
		
	//buffer default points
	for(pnt = 0; pnt < shdr->PntCnt; pnt++)
	{
		ScanPnt spnt;
		if( fread(&spnt, sizeof(ScanPnt), 1, fp_bin) != 1 ){
			PyErr_SetString(PyExc_IOError, "Could not read terrascan point.");
			return NULL;
		}
		
		if(spnt.Code == 1)
		{
			pts[0][curr] = (double)((spnt.Pnt.x - shdr->OrgX) / (double) shdr->Units);
			pts[1][curr] = (double)((spnt.Pnt.y - shdr->OrgX) / (double) shdr->Units);
			pts[2][curr] = (double)((spnt.Pnt.z - shdr->OrgX) / (double) shdr->Units);
			curr++;
		}
		
		if(shdr->Time)
			fseek(fp_bin, 4, SEEK_CUR);
		if(shdr->Color)
			fseek(fp_bin, 4, SEEK_CUR);
	}
	
	fclose(fp_bin);
	printf("\t%d = %d\n", n_pts, curr);
	
	return pts;
}


/*
int main()
{
	char * path = "/Users/Ben/Desktop/test_bin/pt000007.bin";
	FILE * fptr = fopen(path, "rb");
	if(!fptr){
		puts("Couldn't open the bin file.");
		return 0;
	}
	
	ScanHdr shdr, temp;
	
	memset(&shdr, 0, sizeof(shdr));
	
	if( fread(&temp, sizeof(temp), 1, fptr) != 1 ){
		puts("Failed to read the header.");
		return 0;
	}
	
	memcpy(&shdr, &temp, temp.HdrSize);
	fseek(fptr, shdr.HdrSize, SEEK_SET);
	
	printf("%d\n%d\n%d\n%c%c%c%c\n%d\n%d\n%lf\n%lf\n%lf\n%d\n%d\n\n", 
		shdr.HdrSize,
		shdr.HdrVersion,
		shdr.RecogVal,
		shdr.RecogStr[0], shdr.RecogStr[1], shdr.RecogStr[2], shdr.RecogStr[3],
		shdr.PntCnt,
		shdr.Units,
		shdr.OrgX,
		shdr.OrgY,
		shdr.OrgZ,
		shdr.Time,
		shdr.Color);
	
	int pnt;
	for(pnt = 0; pnt < shdr.PntCnt; pnt++){
		ScanPnt spnt;
		fread(&spnt, sizeof(spnt), 1, fptr);
		
		printf("Point %d:\n", pnt);
		printf("(%.2lf,%.2lf,%.2lf)\n%d\n%d\n%d\n%d\n%hd\n%hd\n\n",
			(double)((spnt.Pnt.x - shdr.OrgX) / (double) shdr.Units),
			(double)((spnt.Pnt.y - shdr.OrgY) / (double) shdr.Units),
			(double)((spnt.Pnt.z - shdr.OrgZ) / (double) shdr.Units),
			//spnt.Pnt.x,
			//spnt.Pnt.y,
			//spnt.Pnt.z,
			(int)spnt.Code,
			(int)spnt.Echo,
			(int)spnt.Flag,
			(int)spnt.Mark,
			spnt.Line,
			spnt.Intensity);
		
		if(shdr.Time)
			fseek(fptr, 4, SEEK_CUR);
		if(shdr.Color)
			fseek(fptr, 4, SEEK_CUR);
	}
	
	fclose(fptr);
	
	return 1;
}
*/
