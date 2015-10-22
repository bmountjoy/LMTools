

#include <stdio.h>
#include <dirent.h>
#include <shapefil.h>
#include "PointInPolygon.h"
#include <string.h>

int main(int argc, char * argv [])
{
	int n_records, i, j;
	char * tc_path = argv[1];
	char * fl_dir  = argv[2];
	char fl_path [256];
	
	printf("%s\n", tc_path);
	printf("%s\n", fl_dir);
	
	SHPHandle
	HSHP_polygon = SHPOpen(tc_path, "rb");
	if(!HSHP_polygon){
		puts("Couldn't open tc_path");
		return 0;
	}
	SHPGetInfo(HSHP_polygon, &n_records, NULL, NULL, NULL);
	printf("n_records: %d\n", n_records);
	
	SHPObject* shapes [n_records];
	for(i = 0; i < n_records; i++){
		shapes[i] = SHPReadObject(HSHP_polygon, i);
		if(!shapes[i]){
			puts("Failed reading shape object.");
			return 0;
		}
	}
	SHPClose(HSHP_polygon);
	
	struct dirent *entry;
	DIR *dp;
	
	dp = opendir(fl_dir);
	if (dp == NULL) {
		perror("opendir: Path does not exist or could not be read.");
		return 0;
	}
	
	while ((entry = readdir(dp)))
	{
		sprintf(fl_path, "%s/%s", fl_dir, entry->d_name);
		printf("%s\n",fl_path);
		char end [8];
		                 
		strncpy(end, fl_path + strlen(fl_path) - 3, 3);
		printf("END -- %s\n", end);
		if( strcmp(end, "shp") != 0)
			continue;
		
		
		
		SHPHandle 
		HSHP_fl = SHPOpen(fl_path, "rb");
		if(!HSHP_fl){
			puts("Couldn't open fl_path");
			continue;
		}
		SHPObject * fline = SHPReadObject(HSHP_fl, 0);
		if(!fline){
			puts("Error reading flight line.");
			return 0;
		}
		
		for(i = 0; i < n_records; i++){
			SHPObject * shape = shapes[i];
			
			for(j = 0; j < shape->nVertices; j++){
				if(pointInPolygon(fline, shape->padfX[j], shape->padfY[j])){
					printf("\t%d\n", i);
					break;
				}
			}
		}
		
		SHPDestroyObject(fline);
		SHPClose(HSHP_fl);
	}
	
	closedir(dp);
	return 0;
}

