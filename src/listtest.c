

#include "List.h"
#include <stdio.h>
#include <stdlib.h>
#include <lasreader.hpp>

int main()
{
	
	//las testing tooo....
	LASreadOpener lasreadopener;
	LASreader * lasreader;
	
	lasreadopener.set_file_name(las_path);
	lasreader = lasreadopener.open();
	if(!lasreader){
		PyErr_SetString(PyExc_IOError, "Could not open lidar file.");
		return NULL;
	}
	
	while(lasreader->read_point())
	{	
		//printf("%u of %u\n", pcnt++, npts);
		
		if(lasreader->point.classification != 1)
			continue;
		
		double x_coord = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
		double y_coord = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
		double z_coord = (lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset;
		
		printf("(%lf, %lf, %lf)(%lf, %lf, %lf)", x_coord, y_coord, z_coord,
			lasreader->point.x, lasreader->point.quantizer->x_scale_factor,lasreader->point.quantizer->x_offset);
	}
	
	lasreader->close();
	delete lasreader;
	
	/*
	
	int j;
	for(j = 0; j < 10000; j++)
	{
		List * 
		float_list = initList();
		
		int i;
		for(i = 0; i < 100; i++)
		{
			float fdata = (float) i;
			
			float * data = (float *) malloc(sizeof(float));
			*data = fdata;
			
			if(!addFront(float_list, data)){
				printf("Error\n");
				return 0;
			}
		}
		freeList(float_list);
	}
	return 1;
	*/
}
