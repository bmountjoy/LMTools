
#include "LasIO.hpp"


double ** allocd2d(int d1, int d2)
{
	double **buff;
	
	buff = (double**)malloc(sizeof(double*) * d1);
	if(!buff){	
		return NULL;
	}
	
	int i;
	for(i = 0; i < d1; i++)
	{
		buff[i] = (double*)malloc(sizeof(double) * d2);
		if(!buff[i]){
			return NULL;
		}
	}
	
	return buff;
}

/**
 * Buffers all of the points in the file skipping points whose code
 * (classification) is not equal to 1.
 */
double ** bufferLasFile(char * path, int * p_npts)
{
	
	MyLASLib lasreader (path);

	int npts = 0, pnt = 0;

	//count the number of default points
	while(lasreader.readPoint())
	{
		// if(lasreader->point.classification == 1)
		if(lasreader.point->classification == 1)
			npts++;
		else
			printf("point class: %d\n", lasreader.point->classification);
	}
	
	*p_npts = npts;
	
	//initialize buffer for each default point
	double ** pts = allocd2d(3, npts);
		if(!pts) return NULL;
	
	while(lasreader.readPoint())
	{
		if(lasreader.point->classification == 1)
		{
			double x_coord = lasreader.getPntX();
			double y_coord = lasreader.getPntY();
			double z_coord = lasreader.getPntZ();
			pts[0][pnt] = x_coord;
			pts[1][pnt] = y_coord;
			pts[2][pnt] = z_coord;
			pnt++;
		}
	}
	
	printf("%d = %d\n", npts, pnt);
	
	return pts;
}



/*
void readLas(char * path, char * outpath)
{
	LASreadOpener lasreadopener;
	LASreader * lasreader;
	
	lasreadopener.set_file_name(path);
	lasreader = lasreadopener.open();
	
	FILE * out = fopen(outpath, "w");
	
	while(lasreader->read_point())
	{
		if(lasreader->point.classification == 1){
			double x_coord = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
			double y_coord = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
			double z_coord = (lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset;
			
			fprintf(out, "%.2lf, %.2lf, %.2lf\n", x_coord, y_coord, z_coord);
		}
	}
	
	lasreader->close();
	delete lasreader;
	fclose(out);
}

int main()
{
	int i, npts;
	double ** pts =	
	bufferLasFile("/Users/Ben/Desktop/test_las/00500350_rs.las", &npts);
	
	puts("buffered pts");
	if(pts == NULL){
		puts("pts null");
		return 1;
	}
	
	FILE * out = fopen("/Users/Ben/Desktop/testing/out1.txt", "w");
	for(i = 0; i < npts; i++){
		fprintf(out, "%.2lf, %.2lf, %.2lf\n", pts[0][i], pts[1][i], pts[2][i]);
	}
	fclose(out);
	
	puts("points rit");
	
	for(i = 0; i < 3; i++)
		free(pts[i]);
	free(pts);
	
	puts("free pts");
	
	readLas("/Users/Ben/Desktop/test_las/00500350_rs.las", "/Users/Ben/Desktop/testing/out2.txt");
	
	puts("readlas");
	
	return 0;
}
*/
