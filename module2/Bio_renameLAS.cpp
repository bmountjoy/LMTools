


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



char * output_to_text(int x_blk, int y_blk, float z_threshold, Lasreader * lasreader, char * out_dir)
{
	char out_path [512];
	
	sprintf(out_path, "%s/text/CHM_%d_%d.txt", out_dir, x_blk, y_blk);
	
	FILE * fp_text = fopen(out_path, "w");
	if(!fp_text){
		return "Error: fopen failed on output bin file";
	}
	
	while(lasreader->read_point())
	{
		double x = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
		double y = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
		float  z = (float)((lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset);
		
		if(z < z_threshold)
		{
			fprintf(fp_text, "%.2lf %.2lf %.2f\n", x, y, z);
		}
	}
	
	fclose(fp_text);
	
	lasreader->close();
	delete lasreader;
}




char * output_to_bin(int x_blk, int y_blk, float z_threshold, Lasreader * lasreader, char * out_dir)
{
	char out_path [512];
	
	sprintf(out_path, "%s/bin/CHM_%d_%d.txt", out_dir, x_blk, y_blk);
	
	FILE * fp_bin = fopen(out_bin_path, "w");
	if(!fp_bin){
		return "Error: fopen failed on output bin file";
	}

/** get xcoord using las file

	x = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset
*/

	int units = (int)(1.0/lasreader->point.quantizer->x_scale_factor + 0.5);
	double xoff = lasreader->point.quantizer->x_offset;
	double yoff = lasreader->point.quantizer->y_offset;
	double zoff = lasreader->point.quantizer->z_offset;
	double orgx = -1.0 * units * xoff;
	double orgy = -1.0 * units * yoff;
	double orgz = -1.0 * units * zoff;
	
	printf("units:%d\nxoff:%lf\nyoff:%lf\nzoff:%lf\n", units, xoff, yoff, zoff);
	
	ScanHdr scan_hdr;
	scan_hdr.HdrSize = sizeof(ScanHdr);
	scan_hdr.HdrVersion = 20020715;
	scan_hdr.RecogVal = 970401;
	scan_hdr.RecogStr = {'C','X','Y','Z'};
	scan_hdr.PntCnt = 0;		//determined below
	scan_hdr.Units = units;
	scan_hdr.OrgX = orgx;		//see above calcs
	scan_hdr.OrgY = orgy;		//see above calcs
	scan_hdr.OrgZ = orgz;		//see above calcs
	scan_hdr.Time = 0;			//may be loosing some information
	scan_hdr.Color = 0;			//may be loosing some information
	
	fwrite(&scan_hdr, sizeof(ScanHdr), 1, fp_bin);
	
	
	unsigned int npoints = 0;
	while(lasreader->read_point())
	{
		double x = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
		double y = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
		float  z = (float)((lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset);
		
		//build a ScanPnt data structure
		
		Point3d point = {lasreader->point.x, lasreader->point.y, lasreader->point.z};
		ScanPnt spnt;
		spnt.Pnt  = point;
		spnt.Code = lasreader->classification;		//byte
		spnt.Echo = lasreader->return_number;		//byte
		spnt.Flag = 0;								//byte
		spnt.Mark = 0;								//byte
		spnt.Line = lasreader->point_source_ID;		//ushort
		spnt.Intensity = lasreader->intensity;		//ushort
		
		
		if(z < z_threshold)
		{
			npoints++;
			fwrite(&spnt, sizeof(ScanPnt), 1, fp_bin);
			if(shdr->Time)
					fwrite(&time, sizeof(int), 1, fp_bin);
				if(shdr->Color)
					fwrite(&color, sizeof(int), 1, fp_bin);
		}
	}
	
	
	fseek(fp_bin, 0, SEEK_SET);
	
	scan_hdr.PntCnt = npoints;
	fwrite(&scan_hdr, sizeof(ScanHdr), 1, fp_bin);
	
	lasreader->close();
	delete lasreader;
}




char * rename_LAS(char * file_path, char * out_dir, double min_easting, double min_northing, int blk_size,
	float z_threshold, int outText, int outBin)
{
	printf("renameFile\n\tRenaming : %s \n", file_path);
	
	LASreadOpener lasreadopener;
	LASreader * lasreader;
	
	lasreadopener.set_file_name(path);
	lasreader = lasreadopener.open();
	if(!lasreader){
		return NULL;
	}
	
	lasreader->read_point();
	
	double x = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
	double y = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
	double z = (lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset;
	
	lasreader->seek(0);
	
	int x_blk = (int)((x - min_easting)/blk_size);
	int y_blk = (int)((y - min_northing)/blk_size);
	
	
	if(outText)
	{
		output_to_text(x_blk, y_blk, lasreader, out_dir);
	}
	else
	{
		output_to_bin(x_blk, y_blk, lasreader, out_dir);
	}
	
	
	
	
	//build file name
	
	
	
	while(lasreader->read_point())
	{

		x = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
		y = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
		z = (lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset;
		
		if(z < z_threshold)
		{
			pts[0][pnt] = x_coord;
			pts[1][pnt] = y_coord;
			pts[2][pnt] = z_coord;
			pnt++;
		}
	}
	
	lasreader->close();
	delete lasreader
	
	
	
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
		if(pnt % 200000 == 0)printInfo(pnt, spnt, x, y, z);
		
		//point out of bounds
		if(!(min_blk_e <= x && x <= max_blk_e && min_blk_n <= y && y <= max_blk_n)){
			printf("(%lf, %lf): (%lf,%lf)->(%lf,%lf): %d\n", 
				x,y,min_blk_e,min_blk_n,max_blk_e,max_blk_n,++bc);
		}
		
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
			printf("Point too high: %lf (%d)\n",z,++th);
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
