#include "TM_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void updateHeader(char * key, char * value);
void cleanup(float **, int);
int readHeader( char * filePath );
char parseKeyValue( FILE * file );
float ** readFileWithHeader(char * filePath);

extern int IMAGE_WIDTH;
extern int IMAGE_LENGTH;
extern int NUMBER_OF_ROWS;
extern int NUMBER_OF_COLS;

typedef struct {
	char * description;
	int samples;
	int lines;
	int bands;
	int header_offset;
	char * file_type;
	int data_type;
	char * interleave;
	char * sensor_type;
	int byte_order;
	char * map_info;
	char * wavelength_units;
	char * band_names;
	int x_start;
	int y_start;
	char * coordinate_system_string;
	//map_info
	//projection_info
	//..
} Header;

Header hdr;

#define LINE 256


float ** readENVIStandard(char * hp, char * fp)
{
	if(!readHeader(hp))
		return NULL;
	
	return readFileWithHeader(fp);
}

/**
 * Assumption is that there is only one band of input otherwise may need to 
 * return float ***.
 */ 
float ** readFileWithHeader(char * filePath)
{
	FILE * file;
	if( (file = fopen(filePath, "r")) == NULL ) 
	{
		PyErr_SetString(PyExc_IOError, "Couldn't open : filePath.");
		return NULL;
	}
	
	outputHeader();
	
	int line, sample, lines, samples;
	
	samples = hdr.samples;
	lines = hdr.lines;
	
	float ** IMAGE_BUFFER;
	
	if( (IMAGE_BUFFER = (float **) malloc(sizeof(float *) * lines)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return NULL;
	}
	
	for(line = 0; line < lines; line++)
	{
		if( (IMAGE_BUFFER[line] = (float *) malloc(sizeof(float) * lines)) == NULL)
		{
			PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
			return NULL;
		}
		
		if(fread(IMAGE_BUFFER[line], sizeof(float), samples, file) < samples)
		{
			cleanup(IMAGE_BUFFER, line + 1);
			PyErr_SetString(PyExc_IOError, "File did not match header (less bytes read than specified in call to fread()).");
			return NULL;
		}
	}
	return IMAGE_BUFFER;
}

/**
 * Error reading image. Clean up memory.
 */
void cleanup(float ** IMAGE_BUFFER, int rowMax)
{
	int row;
	for(row = 0; row < rowMax; row++)
	{
		_TIFFfree(IMAGE_BUFFER[row]);
	}
	_TIFFfree(IMAGE_BUFFER);
}

int readHeader(char * filePath)
{
	FILE * header;
	if( (header = fopen(filePath, "r")) == NULL )
	{
		PyErr_SetString(PyExc_IOError, "Couldn't open : filePath.");
		return 0;
	}
	
	char line [128], c;
	
	fgets(line, 128, header);
	
	while( (c = parseKeyValue(header)) != EOF )
	{
		parseKeyValue(header);
	}
	fclose(header);	
	
	return 1;
}

char parseKeyValue(FILE * header)
{
	int idx;
	char * line, * key, * value, c;
	
	idx   =  0;
	key   = (char *) malloc(LINE);
	value = (char *) malloc(LINE);
	
	// consume any non-alphabetic character before key
	// return if 'EOF' is reached
	c = getc(header);
	while( !(c >= 'A' && c <= 'z') )
	{
		if( c == EOF )
		{
			return c;
		}
		c = getc(header);
	}
	
	// key '=' value
	while( c != '=' )
	{
		key[idx++] = c;
		c = getc(header);
	}
	// add terminating character
	key[idx] = '\0';
	
	// remove white space after key and before '='
	while( key[--idx] == ' ' )
	{
		key[idx] = '\0';
	}
	
	// consume white space after '='
	while( (c = getc(header)) == ' ' );
	
	char end = '\n';
	
	// end of value is represented with '}'
	if( c == '{' )
	{
		end = '}';
		// consume '{' and proceding characters not in A...Za...z
		c = getc(header);
		while( c < 'A' || c > 'z' )
		{
			c = getc(header);
		}
	}
	
	idx = 0;
	
	value[idx++] = c;
	
	//read in value
	while( (c = getc(header)) != end && c != EOF )
	{
		if(c == '\n' || c == '\r')
			continue;
		
		value[idx++] = c;
	}
	//append terminating character to string
	value[idx] = '\0';
	
	// remove whitepace after value
	while( value[idx] == ' ' )
	{
		value[idx--] = '\0';
	}
	
	updateHeader(key, value);
	
	return c;
}

void updateHeader(char * key, char * value)
{
	if(strcmp(key, "description") == 0)
	{
		hdr.description = value;
	}
	else if(strcmp(key, "samples") == 0)
	{
		hdr.samples = atoi(value);
		NUMBER_OF_COLS = IMAGE_WIDTH = atoi(value);
	}
	else if(strcmp(key, "lines") == 0)
	{
		hdr.lines = atoi(value);
		NUMBER_OF_ROWS = IMAGE_LENGTH = atoi(value);
	}
	else if(strcmp(key, "bands") == 0)
	{
		hdr.bands = atoi(value);
	}
	else if(strcmp(key, "header offset") == 0)
	{
		hdr.header_offset = atoi(value);
	}
	else if(strcmp(key, "file type") == 0)
	{
		hdr.file_type = value;
	}
	else if(strcmp(key, "data type") == 0)
	{
		hdr.data_type = atoi(value);
	}
	else if(strcmp(key, "interleave") == 0)
	{
		hdr.interleave = value;
	}
	else if(strcmp(key, "sensor type") == 0)
	{
		hdr.sensor_type = value;
	}
	else if(strcmp(key, "byte order") == 0)
	{
		hdr.byte_order = atoi(value);
	}
	else if(strcmp(key, "map info") == 0)
	{
		hdr.map_info = value;
	}
	else if(strcmp(key, "wavelength units") == 0)
	{
		hdr.wavelength_units = value;
	}
	else if(strcmp(key, "band names") == 0)
	{
		hdr.band_names = value;
	}
}

void outputHeader()
{
	printf("Header.description :      %s\n", hdr.description);
	printf("Header.samples :          %d\n", hdr.samples);
	printf("Header.lines :            %d\n", hdr.lines);
	printf("Header.bands :            %d\n", hdr.bands);
	printf("Header.header_offset :    %d\n", hdr.header_offset);
	printf("Header.file_type :        %s\n", hdr.file_type);
	printf("Header.data_type :        %d\n", hdr.data_type);
	printf("Header.interleave :       %s\n", hdr.interleave);
	printf("Header.sensor_type :      %s\n", hdr.sensor_type);
	printf("Header.byte_order :       %d\n", hdr.byte_order);
	printf("Header.map_info :         %s\n", hdr.map_info);
	printf("Header.wavelength_units : %s\n", hdr.wavelength_units);
	printf("Header.band_names :       %s\n", hdr.band_names);
}
