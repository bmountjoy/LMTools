
#include <Python/Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ENVIStandardIO.h"

int 	SIZE = 512;

char consumeWhitespace(FILE * fp, char c)
{
	while(c == ' ' || c == '\n' || c == '\t' || c == '\r')
		c = getc(fp);
	
	return c;
}

int consumeArrayWhitespace(char * array, int index)
{
	while(array[index] == ' ' || array[index] == '\t' || array[index] == '\n' || array[index] == '\r')
		index++;
	
	return index;
}

int getDatatypeSize(int datatype)
{
	switch(datatype)
	{
		case 1:
			return 1;
		case 2:
		case 12:
			return 2;
		case 3:
		case 4:
		case 13:
			return 4;
		case 5:
		case 6:
		case 14:
		case 15:
			return 8;
		case 9:
			return 16;
		default:
			return 4;
	}
}

MapInfo parseMapInfo(char * value)
{
	MapInfo map;
	
	int param_idx, value_idx;
	char param [128];
	
	param_idx = 0;
	value_idx = 0;
	
	//read proj_name
	while(value[value_idx] != ',')
		param[param_idx++] = value[value_idx++];
	param[param_idx] = '\0';
	strcpy(map.proj_name, param);
	
	value_idx = consumeArrayWhitespace(value, value_idx+1);
	param_idx = 0;
	
	//get x pixel
	while(value[value_idx] != ',')
		param[param_idx++] = value[value_idx++];
	param[param_idx] = '\0';
	map.x_pixel = atoi(param);
	
	value_idx = consumeArrayWhitespace(value, value_idx+1);
	param_idx = 0;
	
	//gety pixel
	while(value[value_idx] != ',')
		param[param_idx++] = value[value_idx++];
	param[param_idx] = '\0';
	map.y_pixel = atoi(param);
	
	value_idx = consumeArrayWhitespace(value, value_idx+1);
	param_idx = 0;
	
	//get easting of pixel
	while(value[value_idx] != ',')
		param[param_idx++] = value[value_idx++];
	param[param_idx] = '\0';
	map.easting = atof(param);
	
	value_idx = consumeArrayWhitespace(value, value_idx+1);
	param_idx = 0;
	
	//get northing of pixel
	while(value[value_idx] != ',')
		param[param_idx++] = value[value_idx++];
	param[param_idx] = '\0';
	map.northing = atof(param);
	
	value_idx = consumeArrayWhitespace(value, value_idx+1);
	param_idx = 0;
	
	//get x scale of pixel
	while(value[value_idx] != ',')
		param[param_idx++] = value[value_idx++];
	param[param_idx] = '\0';
	map.x_scale = (float)atof(param);
	
	value_idx = consumeArrayWhitespace(value, value_idx+1);
	param_idx = 0;
	
	//get y scale of pixel
	while(value[value_idx] != ',')
		param[param_idx++] = value[value_idx++];
	param[param_idx] = '\0';
	map.y_scale = (float)atof(param);
	
	value_idx = consumeArrayWhitespace(value, value_idx+1);
	param_idx = 0;
	
	//get projection zone of pixel
	while(value[value_idx] != ',')
		param[param_idx++] = value[value_idx++];
	param[param_idx] = '\0';
	map.proj_zone = atoi(param);
	
	value_idx = consumeArrayWhitespace(value, value_idx+1);
	param_idx = 0;
	
	//get north/south zone of pixel
	while(value[value_idx] != ',')
		param[param_idx++] = value[value_idx++];
	param[param_idx] = '\0';
	if(strcmp(param, "North")==0)
		map.north = 1;
	else
		map.north = 0;
	
	return map;
}

int parseWavelengths(char * value, ENVIHeader * header)
{
	char wave [32];
	int vid = 0, wid = 0, count = 0;
	
	while(1)
	{	
		//consume whitespace
		while(value[vid] == ' ' || value[vid] == '\n' || value[vid] == '\r' || value[vid] == '\t')
			vid++;
		
		//read value until encounter whitespace or terminator
		while(value[vid] != ' ' && value[vid] != '\n' && value[vid] != '\r' != value[vid] != '\t' &&
			value[vid] != ',' && value[vid] != '\0')
		{
			vid++;
		}
		
		//consume whitespace
		while(value[vid] == ' ' || value[vid] == '\n' || value[vid] == '\r' || value[vid] == '\t')
			vid++;
		
		count++;
		
		if(value[vid] == '\0')
			break;
		else
			vid++;
	}
	header->n_wavelengths = count;
	
	float * waves = (float *)malloc(sizeof(float) * count);
	if(!waves){
		puts("couldn't allocate waves");
		return 0;
	}
	
	vid = 0;
	count = 0;
	
	while(1)
	{	
		wid = 0;
		
		//consume whitespace
		while(value[vid] == ' ' || value[vid] == '\n' || value[vid] == '\r' || value[vid] == '\t')
			vid++;
		
		//read value until encounter whitespace or terminator
		while(value[vid] != ' ' && value[vid] != '\n' && value[vid] != '\r' != value[vid] != '\t' &&
			value[vid] != ',' && value[vid] != '\0')
		{
			wave[wid++] = value[vid++];
		}
		wave[wid] = '\0';
		
		waves[count++] = atof(wave);
		
		//consume whitespace
		while(value[vid] == ' ' || value[vid] == '\n' || value[vid] == '\r' || value[vid] == '\t')
			vid++;
		
		if(value[vid] == '\0')
			break;
		else
			vid++;
	}
	
	header->wavelengths = waves;
	return 1;
}

char * resize (char * buff)
{
	SIZE = SIZE + SIZE;
	return (char *) realloc(buff, SIZE);
}

/**
 * Reads the value of a (key, value) pair. Assumes the fp_header is
 * not pointing at whitespace.
 */
char readValue(FILE * fp_header, char ** p_value, int size, char c)
{	
	int index = 0;
	char * value = *p_value;
	
	if(c == '{')
	{
		c = getc(fp_header);
		
		while(c != '}'){
			value[index++] = c;
			if(index == SIZE){*p_value = resize(value); value = *p_value;}
			c = getc(fp_header);
		}
		value[index] = '\0';
		c = getc(fp_header);
		
		return c;
	}
	
	//not reading list
	while( c != '\n' && c != 13)
	{
		value[index++] = c;
		if(index == SIZE){*p_value = resize(value); value = *p_value;}
		c = getc(fp_header);
	}
	value[index] = '\0';
	
	return c;
}


/**
 * Compares two strings ignoring case.
 * Returns 1 if the strings are equal, else, returns 0.
 */
int compare_alpha(char * s1, char * s2)
{
	int l = (int)strlen(s1);
	
	if(l != strlen(s2))
	{
		return 0;
	}
	
	int i;
	for(i = 0; i < l; i++)
	{
		if(s1[i] >= 'A' && s1[i] <= 'Z')
		{
			if(s1[i] != s2[i] && s2[i] != 'a' + (s1[i]-'A'))
			{
				return 0;
			}
		}
		else
		{
			if(s1[i] != s2[i] && s2[i] != 'A' + (s1[i] - 'a'))
			{
				return 0;
			}
		}
	}
	return 1;
}

void initENVIHeader(ENVIHeader * header)
{
	strcpy(header->interleave, "bsq");
	header->samples = 0;
	header->lines = 0;
	header->bands = 0;
	header->header_offset = 0;
	header->datatype = 0;
	header->x_start = 0;
	header->y_start = 0;
	header->byte_order = 0;
	header->info.x_pixel = 0;
	header->info.y_pixel = 0;
	header->info.x_scale = 1;
	header->info.y_scale = 1;
}

ENVIHeader * readHeader(char * base_path)
{	
	ENVIHeader * 
	header = (ENVIHeader *) malloc(sizeof(ENVIHeader));
		if(!header)return NULL;
	
	initENVIHeader(header);
	
	int i,index;
	char c, key [128], header_path [256], * value;
	
	strcpy(header_path, base_path);
	strcat(header_path, ".hdr");
	
	FILE *
	fp_header = fopen(header_path, "r");
	
	if(!fp_header)
	{
		PyErr_SetString(PyExc_IOError, "Could not open ENVI header file.");
		return NULL;
	}
	
	SIZE  = 8196;
	value = (char *) malloc(sizeof(char) * SIZE);
	if(!value){
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	
	c = getc(fp_header);
	while( c != '\n') c = getc(fp_header);
	
	c = consumeWhitespace(fp_header, c);
	
	while( 1 )
	{
		//read key
		index = 0;
		while(c != '=')
		{
			key[index++] = c;
			c = getc(fp_header);
		}
		
		//strip whitespace
		for(i = index-1; i >= 0; i--)
		{
			if( !(key[i] == ' ' || key[i] == '\t' || key[i] == '\n' || key[i] == '\r') )
				break;
		}
		
		key[++i] = '\0';
		
		c = getc(fp_header);
		c = consumeWhitespace(fp_header, c);
		
		if(strcmp(key, "interleave") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			strcpy(header->interleave, value);
		}
		else if(strcmp(key, "samples") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			header->samples = atoi(value);
		}
		else if(strcmp(key, "lines") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			header->lines = atoi(value);
		}
		else if(strcmp(key, "bands") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			header->bands = atoi(value);
		}
		else if(strcmp(key, "header offset") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			header->header_offset = atoi(value);
		}
		else if(strcmp(key, "data type") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			header->datatype = atoi(value);
		}
		else if(strcmp(key, "x start") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			header->x_start = atoi(value);
		}
		else if(strcmp(key, "y start") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			header->y_start = atoi(value);
		}
		else if(strcmp(key, "byte order") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			header->byte_order = atoi(value);
		}
		else if(strcmp(key, "map info") == 0)
		{
			c = readValue(fp_header, &value, SIZE, c);
			MapInfo map = parseMapInfo(value);
			header->info = map;
		}
		//else if(strcmp(key, "wavelength") == 0)
		else if(compare_alpha(key, "wavelength"))
		{
			c = readValue(fp_header, &value, SIZE, c);
			if(!parseWavelengths(value, header)){
				return NULL;
			}
		}
		else
		{
			//read the value and throw it out
			c = readValue(fp_header, &value, SIZE, c);
			value[0] = '\0';
		}
		
		//consume whitespace + check for EOF
		while(c == ' ' || c == '\n' || c == '\t' || c == '\r'){
			if(c == EOF)
				break;
			c = getc(fp_header);
		}
			
		if(c == EOF)
			break;
	}	
	
	free(value);
	if(fclose(fp_header) == EOF){
		printf("fclose failed\n");
		return NULL;
	}
	if(header->n_wavelengths < header->bands){
		printf("Error. wavelengths(%d) < bands\n", header->n_wavelengths);
		return NULL;
	}

	return header;
}

//implement this some time
int readDataBSQ(FILE * fp_data, void ** buff, ENVIHeader * header, int line_size, int band_to_read, int dt_size)
{
	int line;
	int mem_offset = band_to_read * line_size * header->lines;
	
	fseek(fp_data, mem_offset, SEEK_SET);
	
	for(line = 0; line < header->lines; line++)
	{
		if(fread(buff[line], dt_size, header->samples, fp_data) != header->samples)
		{
			printf("readdatabsq error: %d,%d,%d\n", line, dt_size, header->samples);
			return 0;
		}
	}
	
	return 1;
}

int readDataBIL(FILE * fp_data, void ** buff, ENVIHeader * header, int line_size, int band_to_read, int dt_size)
{
	int line;
	int mem_offset = band_to_read * line_size;
	
	for(line = 0; line < header->lines; line++)
	{
		fseek(fp_data, mem_offset, SEEK_CUR);
		if( fread(buff[line], dt_size, header->samples, fp_data) != header->samples )
		{
			//PyErr_SetString(PyExc_IOError, "fread failed.");
			return 0;
		}
		mem_offset = line_size * (header->bands - 1);
	}
	
	return 1;
}

void ** readData(char * base_path, ENVIHeader * header, int band_to_read)
{
	
	int dt_size, line_size;
	char data_path [128];
	void ** buff;
	FILE * fp_data;
	
	if(band_to_read >= header->bands || band_to_read < 0)
		band_to_read = 0;
	
	strcpy(data_path,base_path);
	strcat(data_path,".dat");
	
	fp_data = fopen(data_path, "r");
	
	if(!fp_data){
		//PyErr_SetString(PyExc_IOError, "Could not open ENVI data file.");
		puts("Could not open ENVI data file.");
		return NULL;
	}
	
	
	dt_size   = getDatatypeSize(header->datatype);
	line_size = header->samples * dt_size;
	
	if(! (buff = allocv2d(header->lines, header->samples, dt_size)) ){
		puts("Could not allocate buffer.");
		return NULL;
	}
	
	
	if(strcmp(header->interleave, "bil") == 0) 
	{
		if( !readDataBIL(fp_data, buff, header, line_size, band_to_read, dt_size) ){
			puts("Read data BIL failed");
			return NULL;
		}
	}
	else if(strcmp(header->interleave, "bsq") == 0)
	{
		if( !readDataBSQ(fp_data, buff, header, line_size, band_to_read, dt_size) ){
			puts("read data BSQ failed.");
			return NULL;
		}
	}
	else{
		buff = NULL;
	}
	
	fclose(fp_data);
	
	return buff;
}

void ** readENVIStandardBand(char * base_path, int band_index, ENVIHeader * header)
{	
	void ** 
	buff = readData(base_path, header, band_index);
	
	return buff;
}

ENVIHeader * getENVIHeaderInfo(char * base_path)
{
	return readHeader(base_path);
}


int convert_ushort_bsq_to_bil(char * bsq_base_path, char * bil_path)
{
	ENVIHeader * bsq_hdr = getENVIHeaderInfo(bsq_base_path);
		if(!bsq_hdr) return 0;
		
	if(bsq_hdr->datatype != 12){
		printf("bsq_hdr datatype != 12\n");	
		return 0;
	}
	
	int band, line;
	int bands = bsq_hdr->bands;
	int lines = bsq_hdr->lines;
	int samples = bsq_hdr->samples;
	
	unsigned short *** bsq_img = (unsigned short ***) 
		malloc(sizeof(unsigned short **) * bands);	
	if(!bsq_img){
		printf("malloc failed\n");	
		return 0;
	}
		
	for(band = 0; band < bands; band++)
	{
		bsq_img[band] = (unsigned short **)
			readENVIStandardBand(bsq_base_path, band, bsq_hdr);
		
		if(!bsq_img[band]){
			printf("readENVIStandardBand failed\n");	
			return 0;
		}
	}
	
	FILE * fp_bil = fopen(bil_path, "wb");
	if(!fp_bil){
		printf("fopen: bil_path failed\n");
		return 0;
	}
	
	//write the buffer to file
	for(line = 0; line < lines; line++){
		for(band = 0; band < bands; band++)
		{
			if(fwrite(bsq_img[band][line], 2, samples, fp_bil) != samples){
				puts("fwrite failed");
				return 0;
			}
		}
	}
	
	freeus3d(bsq_img, bands, lines);
	fclose(fp_bil);
	
	return 1;
}

void *** readENVIFile(char * path, ENVIHeader * header)
{
	printf("readENVIFile:%s\n", path);
	int bands, lines, samples, band, line, n_lines, line_size, dt_size;
	
	bands = header->bands;
	lines = header->lines;
	samples = header->samples;
	dt_size = getDatatypeSize(header->datatype);
	n_lines = bands * lines;
	line_size = samples * dt_size;
	
	//
	//open file
	//
	FILE * fp = fopen(path, "rb");
	if(!fp)
	{
		printf("readENVIFile:couldn't open envi file\n");
		return NULL;
	}
	
	//
	//allocate 3D buffer
	//
	void *** image;
	
	image = (void ***)malloc(sizeof(void**)*bands);
	if(!image)
	{
		printf("readENVIFile: malloc failed\n");
		return NULL;
	}
	
	for(band = 0; band < bands; band++)
	{
		image[band] = (void **)malloc(sizeof(void*)*lines);
		if(!image[band])
		{
			return NULL;
		}
		
		for(line = 0; line < lines; line++)
		{
			image[band][line] = (void *)malloc(dt_size * samples);
			if(!image[band][line])
			{
				return NULL;
			}
		}
	}
	
	if(strcmp(header->interleave, "bil")==0)
	{
		for(line = 0; line < lines; line++){
		for(band = 0; band < bands; band++)
		{
			if(fread(image[band][line], dt_size, samples, fp) != samples)
			{
				printf("readENVIFile: fread failed.\n");
				return NULL;
			}
		}
		}
	}
	else if(strcmp(header->interleave, "bsq")==0)
	{
		for(band = 0; band < bands; band++){
		for(line = 0; line < lines; line++)
		{
			if(fread(image[band][line], dt_size, samples, fp) != samples)
			{
				printf("readENVIFile: fread failed.\n");
				return NULL;
			}
		}
		}
	}
	else{
		printf("readENVIFile: unknown file type\n");
		return NULL;
	}
	
	fclose(fp);
	
	return image;
}



void freeENVIHeader(ENVIHeader * header)
{
	if(!header)
		return;
	
	if(header->wavelengths)
		free(header->wavelengths);
	free(header);
}

