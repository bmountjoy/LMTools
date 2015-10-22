

#include "asd_io.h"

#define	MAX_LINE	16000

static int WS [3] = {' ', '\t', ','};
static int NWS = 3;

int is_whitespace(char c)
{
	int i;
	for(i = 0; i < NWS; i++)
	{
		if( c == WS[i] )
			return 1;
	}
	return 0;
}


void skip_line(FILE * fp)
{
	fscanf(fp, "%*[^\n\r]");
}


int read_line(FILE * fp, char * buffer)
{
	puts("read_line");
	
	int i = 0;
	int c = getc(fp);
	
	printf("%c\n", c);
	
	while(c != '\n' && c != '\r')
	{
		buffer[i++] = c;
		c = getc(fp);
	}
	buffer[i] = '\0';
	
	printf("read_line:%s\n", buffer);
	
	return i;
}



/**
 * assume no whitespace the beggining or end of the line
 */
int count_cols(FILE * fp)
{
	fseek(fp, 0, SEEK_SET);
	
	int i, size, n_cols;
	char line [MAX_LINE];
	int c;
	
	size = 0;
	c = fgetc(fp);
	
	while(c != '\n' && c != 13){
		line[size++] = c;
		c = fgetc(fp);
	}
	
	n_cols = 1;
	
	for(i = 0; i < size; )
	{
		if(is_whitespace(line[i]))
		{
			n_cols++;
			while(is_whitespace(line[i]))
			{
				if(++i == size)
					break;
			}
		}
		else
		{
			++i;
		}
	}
	
	fseek(fp, 0, SEEK_SET);
	
	return n_cols;
}



int count_lines(FILE * fp)
{
	fseek(fp, 0, SEEK_SET);
	
	int  c, n_lines;
	
	n_lines = 0;
	
	while((c = getc(fp)) != EOF){
		if(c == '\n' || c == '\r')
			n_lines++;
	}
	
	fseek(fp, 0, SEEK_SET);
	
	return n_lines;
}


/**
 * Buffers the spectra names skipping over the wavelengths heading.
 */
char * buffer_asd_header(char * path, char *** p_header, int * p_ncols)
{
	puts("buffer_asd_header");
	
	FILE * fp = fopen(path, "r");
		if(!fp)return "Error in buffer_asd_file: fopen failed";
		
	int ncols = count_cols(fp);
	
	printf("ncols:%d\n", ncols);
	
	char ** header = (char **)malloc(sizeof(char*)*(ncols-1));
	char *  buff   = (char *) malloc(MAX_LINE);
	read_line(fp, buff);
	
	puts("read line complete");
	
	int i;
	char * c_ptr;
	
	c_ptr =  strtok(buff, " \t\n\r");
	
	for(i = 0; i < ncols; i++)
	{
		//skip wavelengths	
		if(i > 0)
			header[i-1] = c_ptr;
		c_ptr = strtok(NULL, " \t\n\r");
	}
		
	*p_ncols = ncols;
	*p_header = header;
	
	fclose(fp);
	
	return NULL;
}


char * buffer_asd_file(char * path, float ** p_waves, float *** p_spec, int * pn_waves, int * pn_spec)
{
	FILE * fp = fopen(path, "r");
		if(!fp)return "Error in buffer_asd_file: fopen failed";
	
	puts("buffer_asd_file");
		
	int n_cols  = count_cols(fp);
	int n_waves = count_lines(fp)-1;	//1st line is a header
	int n_spec  = n_cols-1;
	
	printf("%d,%d,%d\n", n_cols, n_waves, n_spec);
	
	if(n_cols < 1 || n_waves < 1 || n_spec < 1)
		return "Bad dimensions in asd file.";
	
	float * waves = (float*)malloc(sizeof(float)*n_waves);
		if(!waves) return "Error in buffer_asd_file: malloc";
		
	float ** spec  = allocf2d(n_spec, n_waves);
		if(!spec) return "Error in buffer_asd_file: malloc";
		
	//skip header
	skip_line(fp);
	
	int i,j;
	
	for(i = 0; i < n_waves; i++)
	{
		for(j = 0; j < n_cols; j++)
		{
			if(j == 0){
				//reading waveslenth
				if( fscanf(fp, "%f", &waves[i]) < 0 )
					return "Error in buffer_asd_file: fscanf failed";
			}
			else{
				//reading spectra
				if( fscanf(fp, "%f", &spec[j-1][i]) < 0 )
					return "Error in buffer_asd_file: fscanf failed";
			}
		}
	}
	
	/*
	for(i = 0; i < n_waves; i++){
		printf("%f\n", spec[0][i]);
	}
	*/
	fclose(fp);
	
	*p_spec  = spec;
	*p_waves = waves;
	*pn_spec  = n_spec;
	*pn_waves = n_waves;
	
	return NULL;
}





















































