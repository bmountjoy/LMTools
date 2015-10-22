
#include <Python/Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <TerraScanBin.h>


void printHdrInfo(ScanHdr * scnhdr)
{
	printf("hdrsize: %d\n", scnhdr->HdrSize);
	printf("HdrVersion: %d\n", scnhdr->HdrVersion);
	printf("RecogVal: %d\n", scnhdr->RecogVal);
	printf("RecogStr: %c%c%c%c\n", scnhdr->RecogStr[0], scnhdr->RecogStr[1], scnhdr->RecogStr[2], scnhdr->RecogStr[3]);
	printf("PntCnt: %d\n", scnhdr->PntCnt);
	printf("Units: %d\n", scnhdr->Units);
	printf("OrgX: %lf\n", scnhdr->OrgX);
	printf("OrgY: %lf\n", scnhdr->OrgY);
	printf("OrgZ: %lf\n", scnhdr->OrgZ);
	printf("Time: %d\n", scnhdr->Time);
	printf("Color: %d\n", scnhdr->Color);
}

void printInfo(int pnt, ScanPnt * spnt, double x, double y, double z)
{
	printf("Point:%d\n", pnt);
	printf("\tPnt(%d,%d,%d)\n",spnt->Pnt.x,spnt->Pnt.y,spnt->Pnt.z);
	printf("\tPnt(%lf,%lf,%lf)\n",x,y,z);
	printf("\tCode:%d\n",spnt->Code);
	printf("\tEcho:%d\n",spnt->Echo);
	printf("\tFlag:%d\n",spnt->Flag);
	printf("\tMark:%d\n",spnt->Mark);
	printf("\tLine:%d\n",spnt->Line);
	printf("\tIntensity:%d\n",spnt->Intensity);
}

char * renameBinFile(char * file_path, char * out_dir, double min_easting, double min_northing, int blk_size,
	float z_threshold, int outText, int outBin)
{
	printf("renameFile\n\tRenaming : %s \n", file_path);
	
	FILE * in_file, * otxt_fp, * obin_fp;
	
	in_file = fopen(file_path, "rb");
	if(!in_file){
		PyErr_SetString(PyExc_ValueError, "fopen failed. Could not open input file.");
		return NULL;
	}
	
	ScanHdr * shdr = readTerraScanHdr(in_file);
	if(!shdr){
		return NULL;
	}
	
	printHdrInfo(shdr);
	
	if(shdr->PntCnt <= 0){
		PyErr_SetString(PyExc_ValueError, "PntCnt = 0");
		return NULL;
	}
	
	//
	// get the first point then reset the file pointer
	//
	ScanPnt * spnt = getNextPnt(in_file);
	fseek(in_file, shdr->HdrSize, SEEK_SET);
	
	double x = (double)((spnt->Pnt.x - shdr->OrgX) / (double) shdr->Units);
	double y = (double)((spnt->Pnt.y - shdr->OrgY) / (double) shdr->Units);
	int x_pos = (int)((x - min_easting)/blk_size);
	int y_pos = (int)((y - min_northing)/blk_size);
	
	printf("out text: %d\nout bin: %d\n", outText, outBin);
	//
	// build the output path
	//
	char * out_path = (char*)malloc(256);
	sprintf(out_path, "%s/CHM_%d_%d", out_dir, x_pos, y_pos);
	
	char out_txt_path [256], out_bin_path [256];
	
	if(outText)
	{
		sprintf(out_txt_path, "%s/text/CHM_%d_%d.txt", out_dir, x_pos, y_pos);
		otxt_fp = fopen(out_txt_path, "w");
		if(!otxt_fp){
			PyErr_SetString(PyExc_ValueError, "fopen failed. Could not open output file.");
			return NULL;
		}
	}
	if(outBin)
	{
		sprintf(out_bin_path, "%s/bin/CHM_%d_%d.bin", out_dir, x_pos, y_pos);
		obin_fp = fopen(out_bin_path, "w");
		if(!obin_fp){
			PyErr_SetString(PyExc_ValueError, "fopen failed. Could not open output file.");
			return NULL;
		}
		
		fwrite(shdr, shdr->HdrSize, 1, obin_fp);
	}
	
	
	int bc = 0;
	int th = 0;
	double min_blk_e = min_easting + blk_size*x_pos;
	double max_blk_e = min_easting + blk_size*(x_pos + 1);
	double min_blk_n = min_northing + blk_size*y_pos;
	double max_blk_n = min_northing + blk_size*(y_pos+1);
	
	int pnt, n_pts = 0;
	for(pnt = 0; pnt < shdr->PntCnt; pnt++)
	{
		ScanPnt * spnt = getNextPnt(in_file);
		if(!spnt){
			printf("Error reading point %d\n", pnt+1);
			return NULL;
		}
		
		int time, color;
		if(shdr->Time)
			fread(&time, sizeof(int), 1, in_file);
		if(shdr->Color)
			fread(&color, sizeof(int), 1, in_file);
		
		double x = (double)((spnt->Pnt.x - shdr->OrgX) / (double) shdr->Units);
		double y = (double)((spnt->Pnt.y - shdr->OrgY) / (double) shdr->Units);
		double z = (double)((spnt->Pnt.z - shdr->OrgZ) / (double) shdr->Units);
		
		//print out point info
		if(pnt % 200000 == 0){printInfo(pnt, spnt, x, y, z);
			if(shdr->Time)
				printf("time:%d\n",time);
			if(shdr->Color)
				printf("color:%d\n",color);
		}
		
		//point out of bounds
		/*
		if(!(min_blk_e <= x && x <= max_blk_e && min_blk_n <= y && y <= max_blk_n)){
			printf("(%lf, %lf): (%lf,%lf)->(%lf,%lf): %d\n", 
				x,y,min_blk_e,min_blk_n,max_blk_e,max_blk_n,++bc);
		}
		*/
		if(spnt->Code != 1)
			continue;
		
		if(z < z_threshold)
		{
			if(outBin)
			{
				fwrite(spnt, sizeof(ScanPnt), 1, obin_fp);
				if(shdr->Time)
					fwrite(&time, sizeof(int), 1, obin_fp);
				if(shdr->Color)
					fwrite(&color, sizeof(int), 1, obin_fp);
			}
			if(outText)
			{	
				fprintf(otxt_fp, "%.2lf %.2lf %.2lf\n", x, y, z);
			}
			n_pts++;
		}
		else{
			//printf("Point too high: %lf (%d)\n",z,++th);
		}
		
		free(spnt);
	}
	printf("n_pts:%d\n", n_pts);
	
	//update header information in the output file with updated
	// number of points
	if(outBin){
		fseek(obin_fp, 0, SEEK_SET);
		shdr->PntCnt = n_pts;
		fwrite(shdr, shdr->HdrSize, 1, obin_fp);
	}
	
	free(shdr);
	fclose(in_file);
	
	if(outBin)
		fclose(obin_fp);
	if(outText)
		fclose(otxt_fp);
	
	return out_path;
}
/*
int main()
{
	char * path = "/Users/Ben/Desktop/test_bin/pt000007.bin";
	char * out_dir = "/Users/Ben/Desktop/test_bin/";
	
	renameBinFile(path, out_dir, 0.0, 0.0, 500, 0.0);
}
*/
