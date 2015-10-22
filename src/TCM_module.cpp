

/**
 * To compile : export MACOSX_DEPLOYMENT_TARGET=10.6
 *            : python setup.py install
 *       then : python LMTools.py
 * To build   : python setup_app.py py2app
 **/

#include "TCM_module.hpp"

#include <algorithm>
#include <vector>
#include <cmath>

char * buildCSVPath(char * output_dir, const char * base_name)
{
	char * fpath = (char *)malloc(sizeof(char) * 256);
	
	strcpy(fpath, output_dir);
	strcat(fpath, "/");
	strcat(fpath, base_name);
	strcat(fpath, ".csv");
	
	return fpath;
}

char * buildPathForTCBio(char * out_dir, char * base_name)
{
	char * fpath = (char *)malloc(sizeof(char) * 256);
	sprintf(fpath, "%s/bio_%s.csv", out_dir, base_name);
	return fpath;
}

char * buildPathForSpectralMetrics(char * out_dir, const char * base_name, int tc_id, char * fl_fname)
{
	char * fpath = (char *)malloc(sizeof(char) * 256);
		if(!fpath)return NULL;
	sprintf(fpath, "%s/%s_%d_%s.csv", out_dir, base_name, tc_id, fl_fname);
	
	return fpath;
}

char * buildPathForSpectralExtraction(char * out_dir, const char * base_name, int tc_id)
{
	char * fpath = (char *)malloc(sizeof(char) * 256);
		if(!fpath)return NULL;
	sprintf(fpath, "%s/%s_%d.csv", out_dir, base_name, tc_id);
	
	return fpath;
}

float ** calculateMean(List * hspec_records [], int n_records, int n_bands)
{
	int band_idx, record, n_intersect;
	
	float ** mean = allocf2d(n_records, n_bands);
		if(!mean) return NULL;
	
	unsigned int sums [n_bands];
	
	for(record = 0; record < n_records; record++)
	{
		n_intersect = hspec_records[record]->size;
		Node * curr = hspec_records[record]->head;
		
		for(band_idx = 0; band_idx < n_bands; band_idx++)
			sums[band_idx] = 0;
		
		while(curr != NULL)
		{
			HSpecRecord * temp = (HSpecRecord *) curr->data;
			
			for(band_idx = 0; band_idx < n_bands; band_idx++)
			{
				sums[band_idx] += temp->hspec_values[band_idx];	
			}
			
			curr = curr->next;
		}
		
		for(band_idx = 0; band_idx < n_bands; band_idx++)
		{
			if(n_intersect == 0)
				mean[record][band_idx] = 0.0;
			else
				mean[record][band_idx] = (float)sums[band_idx] / n_intersect;	
		}
	}
	
	return mean;
}

float ** calculateVariance(List * hspec_records [], int n_records, int n_bands, float ** mean)
{
	int band_idx, record, n_intersect;
	
	float ** var = allocf2d(n_records, n_bands);
		if(!var) return NULL;
	
	float * diff_sums = (float *)malloc(sizeof(float) * n_bands);
		if(!diff_sums) return NULL;
		
	for(record = 0; record < 10; record++)
	{	
		float * means = mean[record];
		
		n_intersect = hspec_records[record]->size;
		Node *curr = hspec_records[record]->head;
		
		for(band_idx = 0; band_idx < n_bands; band_idx++)
			diff_sums[band_idx] = 0.0;
		
		while(curr != NULL)
		{
			HSpecRecord * temp = (HSpecRecord *) curr->data;
			
			for(band_idx = 0; band_idx < n_bands; band_idx++)
			{
				diff_sums[band_idx] += pow(temp->hspec_values[band_idx] - means[band_idx], 2);	
			}
			
			curr = curr->next;
		}
		
		for(band_idx = 0; band_idx < n_bands; band_idx++)
		{
			if(n_intersect == 0 || n_intersect == 1)
				var[record][band_idx] = 0;
			else
				var[record][band_idx] = diff_sums[band_idx] / (n_intersect - 1);	
		}
	}
	
	free(diff_sums);
	
	return var;
}

int computePointsAboveThreshold(float * points, int n_points, float z_threshold)
{
	int i, count = 0;
	
	for(i = 0; i < n_points; i++)
		if(points[i] > z_threshold)
			count++;
	
	return count;
}

int writeBiometricsAsCSV(int record, float * biometrics, int n_metrics, float height_to_crown, FILE * fp_bio)
{
	fprintf(fp_bio, "%d,%f,", record, height_to_crown);
	
	int i;
	for(i = 0; i < n_metrics-1; i++)
		fprintf(fp_bio, "%f,", biometrics[i]);
	fprintf(fp_bio, "%f\n", biometrics[n_metrics-1]);
	
	return 1;
}

int writeSpectralSubsetToCSV(List * hspec_records[], int n_records, int n_bands, char * out_dir, char * fl_fname)
{
	puts("wrtieSpectralSubsetToCSV");
	
	int record, band;
	char err_msg [256];
	
	for(record = 0; record < n_records; record++)
	{
		//
		//Only create the file if the specific record intersects the flight line
		//
		if(hspec_records[record]->size == 0)
			continue;
		
		//todo:added
		HSpecRecord * hrec = (HSpecRecord *) hspec_records[record]->head->data;
		char * spec_path = buildPathForSpectralExtraction(out_dir, "Spectral_Extraction", hrec->tc_id);//record);
		FILE * fp_spec;
		if(access(spec_path, F_OK) < 0)
		{
			fp_spec = fopen(spec_path, "w");
			if(!fp_spec){
				sprintf(err_msg, "fopen() failed: %s", spec_path);
				PyErr_SetString(PyExc_IOError, err_msg);
				return 0;
			}
			//
			//write the header
			fprintf(fp_spec, "Easting,Norhting,Flight Line,Lidar Pixel Avg.,Lidar Pixel Max,Albedo,Rank");
			for(band = 0; band < n_bands; band++){
				fprintf(fp_spec, ",%f", envi_hdr->wavelengths[band]);
			}
			fprintf(fp_spec, "\n");
		}
		else{
			fp_spec = fopen(spec_path, "a");
			if(!fp_spec){
				sprintf(err_msg, "fopen() failed: %s", spec_path); 
				PyErr_SetString(PyExc_IOError, err_msg);
				return 0;
			}
		}
		
		Node * curr = hspec_records[record]->head;
		while(curr != NULL)
		{
			HSpecRecord * temp = (HSpecRecord *) curr->data;
			
			fprintf(fp_spec, "%lf,%lf,%s,%f,%f,%d,%f,", temp->easting, temp->northing, fl_fname,
				temp->lid_avg, temp->lid_max, temp->rad_sum, temp->rank);
			for(band = 0; band < temp->hspec_size-1; band++)
				fprintf(fp_spec, "%hu,", temp->hspec_values[band]);
			fprintf(fp_spec, "%hu\n", temp->hspec_values[band]);
			
			curr = curr->next;
		}
		
		free(spec_path);
		fclose(fp_spec);
	}
	return 1;
}


int writeSpectralMetricToCSV(char * out_dir, char * metric_name, float ** metric, int n_records, int n_bands)
{
	int record, i;
	char * path = buildCSVPath(out_dir, metric_name);
	
	FILE * fp = fopen(path, "w");
	if(!fp){
		PyErr_SetString(PyExc_IOError, "Could not open output metric CSV file.");
		return 0;
	}
	
	//write header
	//
	fprintf(fp, "Record ID");
	for(i = 0; i < n_bands; i++){
		fprintf(fp, ",Band %d (%f)", i+1, envi_hdr->wavelengths[i]);
	}
	fprintf(fp,"\n");
		
	for(record = 0; record < n_records; record++)
	{
		fprintf(fp, "%d,", record);
		for(i = 0; i < n_bands-1; i++)
			fprintf(fp, "%.2f,", metric[record][i]);
		fprintf(fp, "%.2f\n", metric[record][i]);
	}
	
	free(path);
	fclose(fp);
	
	return 1;
}

void swapBandBytes(unsigned short ** band, int lines, int samples)
{
	int line, sample;
	unsigned short mask = 65280;
	
	for(line = 0; line < lines; line++){
		for(sample = 0; sample < samples; sample++)
		{
			//swap bytes
			band[line][sample] = 
				((band[line][sample] & mask) >> 8) | (band[line][sample] << 8);
		}
	}
}

//todo: can be sped up by skipping files that don't intersect the hyperspectral
// flight line
int gridLasPoints(char * lidar_path, double ul_easting, double ul_northing, 
	int lines, int samples, float pix_size)
{	
	int x, y;
	float z_coord;
	double x_coord, y_coord;
	
	LASreadOpener lasreadopener;
	LASreader * lasreader;
	
	lasreadopener.set_file_name(lidar_path);
	lasreader = lasreadopener.open();
		if(!lasreader){
			PyErr_SetString(PyExc_IOError, "Could not open lidar file.");
			return 0;
		}
	//
	// Assign z values to appropiate pixel locations.
	//
	while(lasreader->read_point())
	{	
		if(lasreader->point.classification != 1)
			continue;
		
		x_coord = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
		y_coord = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
		z_coord = (float)( (lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset );
			
		x = (int)((x_coord - ul_easting) / pix_size);
		y = (int)((ul_northing - y_coord) / pix_size);
		
		if(x < 0 || x >= samples || y < 0 || y >= lines) 
			continue;
		
		float * data = (float*)malloc(sizeof(float));
		if(!data){
			PyErr_SetString(PyExc_IOError, "malloc failed");
			return 0;
		}
		*data = z_coord;
			
		if(!addFront(gridded_zvalues[y][x], data))
			return 0;
	}
	
	lasreader->close();
	delete lasreader;
	
	return 1;
}

float calculateLidarAverage(List * float_list)
{
	float sum = 0.0;
	Node * curr = float_list->head;
	
	while(curr != NULL)
	{
		sum += *( (float *)curr->data );
		curr = curr->next;
	}
	if(sum == 0.0)
		return 0.0;
	else
		return sum / float_list->size;
}

float calculateLidarMax(List * float_list)
{
	float max = 0.0, temp;
	Node * curr = float_list->head;
	
	while(curr != NULL)
	{
		temp = *( (float *)curr->data );
		if(temp > max)
			max = temp;
		curr = curr->next;
	}
	return max;
}

void freeGriddedZValues(List *** gridded_zvalues, int lines, int samples)
{
	int line, sample;
	
	for(line = 0; line < lines; line++){
		for(sample = 0; sample < samples; sample++)
		{
			if(gridded_zvalues[line][sample])
				freeList(gridded_zvalues[line][sample]);
		}
		if(gridded_zvalues[line])
			free(gridded_zvalues[line]);
	}
	if(gridded_zvalues)
		free(gridded_zvalues);
}

int floatCompare(const void * a, const void * b)
{
	float 
	result = *(float*)a - *(float*)b;
	
	if(result > 0)
		return 1;
	else if(result == 0)
		return 0;
	else	
		return -1;
}


/**
 * Build an index on each crown to the lidar points referenced by 'lasreader' when no spatial index file is present. Write
 * the lidar points with z values > 'z_thresh' to the index file.
 */
const char * index_lidar_no_index(SHPObject ** shape_buffer, int nRecords, LASreader * lasreader, List ** buffered_points, float z_threshold, FILE * fp_lid, int * num_all)
{
	puts("index_lidar_no_index");
	
	long pt = 0 ;
	long n_points = (long)(lasreader->npoints);
	double min_x = lasreader->get_min_x();
	double min_y = lasreader->get_min_y();
	double max_x = lasreader->get_max_x();
	double max_y = lasreader->get_max_y();
	
	double * xcoords = (double *)malloc(sizeof(double)*n_points); if(!xcoords) return "Error. malloc failed.";
	double * ycoords = (double *)malloc(sizeof(double)*n_points); if(!ycoords) return "Error. malloc failed.";
	float  * zcoords = (float  *)malloc(sizeof(float )*n_points); if(!zcoords) return "Error. malloc failed.";
	
	//buffer .las file
	while(lasreader->read_point())
	{
		xcoords[pt] = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
		ycoords[pt] = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
		zcoords[pt] = (float)((lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset);
		pt++;
	}
	
	int record;
	for(record = 0; record < nRecords; record++)
	{	
		if(record % 10000 == 0)printf("%d\n", record);
		
		SHPObject *
		pShape = shape_buffer[record];
		
		//skip if crown does not intersect the .las file (large speed up)
		//if(!(min_x < pShape->dfXMax && max_x > pShape->dfXMin && min_y < pShape->dfYMax && max_y > pShape->dfYMin))
		if(  min_x > pShape->dfXMax || max_x < pShape->dfXMin || min_y > pShape->dfYMax || max_y < pShape->dfYMin )
		{
			continue;
		}
		
		for(pt = 0; pt < n_points; pt++)
		{
			double x_coord = xcoords[pt];
			double y_coord = ycoords[pt];
			float  z_coord = zcoords[pt];
			
			if(pointInPolygon(pShape, x_coord, y_coord))
			{
				//write the point to file
				fprintf(fp_lid, "%.2lf,%.2lf,%.2f,%d\n", x_coord, y_coord, z_coord, record);
				num_all[record]++;
				
				if(z_coord >= z_threshold)
				{		
					float * data = (float *) malloc(sizeof(float));
					*data = z_coord;
					
					if(!addFront(buffered_points[record], data)) 
						return "Error. index_lidar_no_index: addFront failed";
				}
			}
		}
	}
	
	free(xcoords);
	free(ycoords);
	free(zcoords);
	
	return NULL;
}

/**
 * Build an index on each crown to the lidar points referenced by 'lasreader' when a spatial index file is present. Write
 * the lidar points with z values > 'z_thresh' to the index file.
 */
const char * index_lidar_with_index(SHPObject ** shape_buffer, int nRecords, LASreader * lasreader, List ** buffered_points, float z_threshold, FILE * fp_lid, int * num_all)
{
	puts("index_lidar_with_index");
	
	int record;
	
	for(record = 0; record < nRecords; record++)
	{	
		SHPObject *
		pShape = shape_buffer[record];
		
		if(!lasreader->inside_rectangle(pShape->dfXMin, pShape->dfYMin, pShape->dfXMax, pShape->dfYMax))
		{
			printf("Failed to subset region.\n");
		}
		
		while(lasreader->read_point())
		{
			double x_coord = (lasreader->point.x * lasreader->point.quantizer->x_scale_factor) + lasreader->point.quantizer->x_offset;
			double y_coord = (lasreader->point.y * lasreader->point.quantizer->y_scale_factor) + lasreader->point.quantizer->y_offset;
			float  z_coord = (float)( (lasreader->point.z * lasreader->point.quantizer->z_scale_factor) + lasreader->point.quantizer->z_offset );
			
			if(pointInPolygon(pShape, x_coord, y_coord))
			{
				//write the point to file
				fprintf(fp_lid, "%.2lf,%.2lf,%.2f,%d\n", x_coord, y_coord, z_coord, record);
				num_all[record]++;
				
				if(z_coord >= z_threshold)
				{		
					float * data = (float *) malloc(sizeof(float));
					*data = z_coord;
					
					if(!addFront(buffered_points[record], data)) 
						return "Error. index_lidar_with_index: addFront failed.";
				}
			}
		}
	}
	
	return NULL;
}


/**
 * Computes biometrics for lidar points within a given set of treecrown
 * vectors.
 */
static PyObject* Py_TreeCrownMetrics(PyObject * self, PyObject * args)
{
	puts("Py_TreeCrownMetrics");
	
	float z_threshold, bucket_size;
	char * las_dir, * treecrown_path, * output_dir, * tc_base_name;
	
	if(!PyArg_ParseTuple(args, "ssssff", &las_dir, &treecrown_path, &tc_base_name, &output_dir, &z_threshold, &bucket_size))
	{
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	//
	// Open dbf file
	//
	DBFHandle 
	HDBF_treecrowns = DBFOpen(treecrown_path, "rb");
	int field_index = DBFGetFieldIndex(HDBF_treecrowns, "TC_Id");
	
	puts("\topen shape file");
	//
	// Open shape file containing treecrowns.
	//
	SHPHandle
	HSHP_treecrowns = SHPOpen(treecrown_path, "rb");
	if(!HSHP_treecrowns){
		PyErr_SetString(PyExc_IOError, "SHPOpen failed. Could not open tree crown file.");
		return NULL;
	}
	
	int i, nRecords, nShapeType, n_points, record, n_metrics;
	
	SHPGetInfo(HSHP_treecrowns, &nRecords, &nShapeType, NULL, NULL);
	if(nShapeType != SHPT_POLYGON){
		PyErr_SetString(PyExc_IOError, "Shape type not SHPT_POLYGON.");
		return NULL;
	}
	
	printf("\tnRecords: %d\n", nRecords);
	int * num_all = (int *)malloc(sizeof(int) * nRecords);
	for(i = 0; i < nRecords; i++){
		num_all[i] = 0;
	}
	
	SHPObject ** shape_buffer = (SHPObject**) malloc(sizeof(SHPObject*) * nRecords);
	for(i = 0; i < nRecords; i++){
		shape_buffer[i] = SHPReadObject(HSHP_treecrowns, i);
		if(!shape_buffer[i]){
			puts("\terror:couldn't read shape buffer");
			return NULL;
		}
	}
	SHPClose(HSHP_treecrowns);
	
	puts("\tcreate a list for each treecrown");
	
	List ** buffered_points = (List **) malloc(sizeof(List *) * nRecords);
	if(!buffered_points){
		printf("Couldn't inti bp list\n");
		return NULL;
	}

	for(i = 0; i < nRecords; i++) {
		buffered_points[i] = initList();
		if(!buffered_points[i]){
			printf("Couldn't init list for tc %d\n", i);
			return NULL;
		}
	}
	
	//
	// Open output point file.
	//
	char * lid_path = buildCSVPath(output_dir, "tc_index_points");
	printf("lid_path: %s\n", lid_path);
	FILE * fp_lid   = fopen(lid_path, "w");
	if(!fp_lid){
		printf("Could not open ouput point index file: %s\n", lid_path);
		PyErr_SetString(PyExc_IOError, "Could not open ouput point index file.");
		return NULL;
	}
	free(lid_path);
	fprintf(fp_lid, "Easting,Northing,Height,Record\n");
	
	struct dirent *entry;
	DIR *dp;
	
	dp = opendir(las_dir);
	if (dp == NULL) {
		perror("opendir: Path does not exist or could not be read.");
		return NULL;
	}
	
	while ((entry = readdir(dp)))
	{	
		char * las_path = entry->d_name;
		
		printf("\tlas_path: %s\n", las_path);
		
		//
		// Open las lidar file.
		//
		LASreadOpener lasreadopener;
		LASreader * lasreader;
		
		char path [256];
		sprintf(path, "%s/%s", las_dir, las_path);
		printf("%s\n", path);
		
		printf("%s\n", (char *)(path + strlen(path) - 4));
		if(strcmp(".las", (char *)(path + strlen(path) - 4)) != 0){
			puts("not las continue");
			continue;
		}
		
		lasreadopener.set_file_name(path);
		lasreader = lasreadopener.open();
		if(!lasreader){
			PyErr_SetString(PyExc_IOError, "Could not open lidar file.");
			continue;
		}
		
		printf("\tnpoints: %lld\n", lasreader->npoints);
		printf("\tp_count: %lld\n", lasreader->p_count);
		printf("\t%lf,%lf,%lf,%lf\n", lasreader->get_min_x(), lasreader->get_min_y(), lasreader->get_max_x(), lasreader->get_max_y());
		
		if(lasreader->get_index() != NULL)
		{
			const char * err = index_lidar_with_index(shape_buffer, nRecords, lasreader, buffered_points, z_threshold, fp_lid, num_all);
			
			if(err){
				PyErr_SetString(PyExc_IOError, err);
				return NULL;
			}
		}
		else
		{
			const char * err = index_lidar_no_index(shape_buffer, nRecords, lasreader, buffered_points, z_threshold, fp_lid, num_all);
			
			if(err){
				PyErr_SetString(PyExc_IOError, err);
				return NULL;
			}
		}
		
		lasreader->close();
		delete lasreader;
	
	}
	fclose(fp_lid);
	closedir(dp);
	
	
	
	puts("Compute biometrics on lidar points.");
	
	//
	// Open output biometrics file
	//
	char * fpath = buildPathForTCBio(output_dir, tc_base_name);
	FILE * fp_bio = fopen(fpath, "w");
	
		if(!fp_bio){
			PyErr_SetString(PyExc_IOError, "Could not open output biometrics csv file.");
			return 0;
		}
		
	fprintf(fp_bio, "TC_Id, Height to Crown, Rugosity,Gap Fraction,Avg Above 85th Perc,L-Mean,L-Cov,L-Skew,L-Kurtosis,85th Perc, # Above Threshold, # Points, LHQ 0,LHQ 5,LHQ 10,LHQ 15,LHQ 20,LHQ 25,LHQ 30,LHQ 35,LHQ 40,LHQ 45,LHQ 50,LHQ 55,LHQ 60,LHQ 65,LHQ 70,LHQ 75,LHQ 80,LHQ 85,LHQ 90,LHQ 95,LHQ 100,CCF z-Threshold,CCF 5,CCF 10,CCF 15,CCF 20,CCF 25,CCF 30,CCF 35,CCF 40,CCF 45,CCF 50,CCF 55,CCF 60,CCF 65,CCF 70,CCF 75,CCF 80,CCF 85,CCF 90,CCF 95, CCF Max Height\n");
	
	//
	// Compute the biometrics for the points associated with each treecrown.
	//
	for(record = 0; record < nRecords; record++)
	{	
		if(buffered_points[record]->size == 0)
			continue;
		
		float * 
		z_points = listToFloatArray(buffered_points[record]);
		n_points = buffered_points[record]->size;
		
		if(!z_points)
			continue;
		
		printf("(record, n_points) : (%d,%d)\n",record, n_points);
		
		qsort(z_points, n_points, sizeof(float), floatCompare);
		
		float 
		height_to_crown = computeTreeCrownHeight(z_points, n_points, bucket_size);
		
		n_metrics = 52;
		
		//todo: added
		int tc_id = record;
		if(field_index >= 0)
			tc_id = DBFReadIntegerAttribute(HDBF_treecrowns, record, field_index);
		
		writeBiometricsAsCSV(
			tc_id, //record,
			computeBiometrics(z_points, n_points, num_all[record], z_threshold), 
			n_metrics,
			height_to_crown,
			fp_bio
		);
		
		free(z_points);
	}
	
	fclose(fp_bio);
	free(fpath);
	
	//
	// Free pointers.
	//
	DBFClose(HDBF_treecrowns);
	for(i = 0; i < nRecords; i++){
		freeList(buffered_points[i]);
		SHPDestroyObject(shape_buffer[i]);
	}
	free(buffered_points);
	free(shape_buffer);
	free(num_all);
	
	Py_INCREF(Py_None);
	return Py_None;
}

char * filterTreecrowns(SHPObject * shapes[], int n_records, SHPObject * fline)
{
	puts("filterTreeCrowns");
	
	int i,j;
	
	char * tc_flags = (char *)malloc(n_records);
	if(!tc_flags){
		return NULL;
	}
	
	for(i = 0; i < n_records; i++)
		tc_flags[i] = 0;
	
	for(i = 0; i < n_records; i++){
		SHPObject * shape = shapes[i];
		
		for(j = 0; j < shape->nVertices; j++){
			if(pointInPolygon(fline, shape->padfX[j], shape->padfY[j])){
				tc_flags[i] = 1;
				break;
			}
		}
	}
	return tc_flags;
}


/**
 * Determine pixels that are in a flight line and tree crown vector and create
 * a HSpecRecord for each.
 *
 * todo: add tc index if it exists
 */
int determineIntersectingPixels_fast(List * hspec_records [], SHPObject * shapes [], int * tc_ids,
	ENVIHeader * envi_hdr, SHPObject * fline, double ul_easting, double ul_northing, int n_records, float half_pix)
{
	puts("determineIntersectingPixels_fast");
	
	int record;
	int lines = envi_hdr->lines;
	int samples = envi_hdr->samples;
	
	for(record = 0; record < n_records; record++)
	{
		if(record % 10000 == 0)
			printf("\t%d/%d\n",record+1, n_records);
		
		SHPObject * crown = shapes[record];
		double x_min, y_min, x_max, y_max;
		x_min = crown->dfXMin;
		y_min = crown->dfYMin;
		x_max = crown->dfXMax;
		y_max = crown->dfYMax;
		
		int x_pix_min, y_pix_min, x_pix_max, y_pix_max;
		x_pix_min = (x_min - ul_easting) / envi_hdr->info.x_scale;
		y_pix_min = (ul_northing - y_max) / envi_hdr->info.y_scale; 	//y coord. decr. as column incr
		x_pix_max = (x_max - ul_easting) / envi_hdr->info.x_scale;
		y_pix_max = (ul_northing - y_min) / envi_hdr->info.y_scale;	//y coord. decr. as column incr
		
		//check if bounding region intersects hyperspectral raster
		if(x_pix_max < 0 || x_pix_min >= samples || y_pix_max < 0 || y_pix_min >= lines){
			//printf("out:%d,%d,%d,%d\n", x_pix_min, y_pix_min, x_pix_max, y_pix_max);
			continue;
		}
		
		//check if pixel coordinates are valid
		x_pix_min = (x_pix_min < 0) ? 0 : x_pix_min;
		y_pix_min = (y_pix_min < 0) ? 0 : y_pix_min;
		x_pix_max = (x_pix_max >= samples) ? samples-1 : x_pix_max;
		y_pix_max = (y_pix_max >= lines) ? lines-1 : y_pix_max;
		
		//if(record % 100 == 0)
		//	printf("%d,%d,%d,%d\n",x_pix_min,y_pix_min,x_pix_max,y_pix_max);
		
		int r, c;
		double x_coord;
		double y_coord = ul_northing - y_pix_min*envi_hdr->info.y_scale;
		
		for(r = y_pix_min; r <= y_pix_max; r++)
		{
			x_coord = ul_easting + x_pix_min*envi_hdr->info.x_scale;
			for(c = x_pix_min; c <= x_pix_max; c++)
			{
				
				//sanity check
				
				/*
				if( (pointInPolygon(fline, x_coord + half_pix, y_coord - half_pix)&&pointInPolygon(crown, x_coord + half_pix, y_coord - half_pix)) != 
					((fline_mask[r][offset] & (1 << shift))&&pointInPolygon(crown, x_coord + half_pix, y_coord - half_pix))){
						printf("discrepancy:r->%d,r->%d,c->%d,%d",record,r,c,(fline_mask[r][offset] & (1 << shift))&&pointInPolygon(crown, x_coord + half_pix, y_coord - half_pix));
					}
				*/
				
				if( pointInPolygon(fline, x_coord + half_pix, y_coord - half_pix) && //point has to be in flightline and treecrown
					pointInPolygon(crown, x_coord + half_pix, y_coord - half_pix))
				
				//int offset = c/d_size;
				//int shift  = c%d_size;
				//if((fline_mask[r][offset] & (1 << shift)) &&	//check against instead of point in polygon check
				//	pointInPolygon(crown, x_coord + half_pix, y_coord - half_pix))
				{
					HSpecRecord * data = (HSpecRecord *) malloc(sizeof(HSpecRecord));
					
					if(!data){
						puts("malloc failed for hspec record");
						return 0;
					}
					
					data->tc_id = tc_ids[record];
					data->x_idx = c;
					data->y_idx = r;
					data->easting = x_coord;
					data->northing = y_coord;
					data->lid_avg = calculateLidarAverage(gridded_zvalues[r][c]);
					data->lid_max = calculateLidarMax(gridded_zvalues[r][c]);
					data->rad_sum = 0;
					data->rank = 0.0;
					data->hspec_values = (unsigned short *)malloc(sizeof(unsigned short) * envi_hdr->bands);
					data->hspec_size = envi_hdr->bands;
					
					if(!addFront(hspec_records[record], data)) 
						return 0;
				}
				
				x_coord += envi_hdr->info.x_scale;
			}
			y_coord -= envi_hdr->info.y_scale;
		}
		
	}
	
	return 1;
}

/*
void prep_thread(List * hspec_records [], SHPObject * shapes [],
	ENVIHeader * envi_hdr, SHPObject * fline, double ul_easting, double ul_northing, int n_records, float half_pix)
{
	int n_threads = 8;
	int threads [n_threads];
	int threadi [n_threads];
	int params  [n_threads];
	
	int chunk = (int)ceil((double)n_records / n_threads);
	int core;
	for(core = 0; core < n_threads; core++){
		//build struct
		Param p;
		p.hspec_records = hspec_records;
		p.shapes = shapes;
		p.start_crown = i*chunk;
		p.end_crown = ((i+1)*chunk > n_records)? n_records : (i+1)*chunk;
		p.envi_hdr = envi_hdr;
		p.fline = fline;
		p.ul_easting = ul_easting;
		p.ul_northing = ul_northing;
		p.n_records = n_records;
		p.half_pix = half_pix;
		params[i] = p;
		
		pthread_create(
			&threads[core], 
			NULL, 
			&determineIntersectingPixels_fast, 
			reinterpret_cast<void*>(&params[i])
			);
	}
	
	for(core = 0; core < n_threads; core++){
		pthread_join(thread[core], NULL);
	}
}
*/

/**
 * Determine pixels that are in a flight line and tree crown vector and create
 * a HSpecRecord for each.
 */
int determineIntersectingPixels(List * hspec_records [], SHPObject * shapes [],
	ENVIHeader * envi_hdr, SHPObject * fline, double ul_easting, double ul_northing, int n_records, float half_pix)
{
	puts("determineIntersectingPixels");
	
	char * tc_flags;
	int record, i, j;
	double x_coord, y_coord;
	
	//flag tree crowns not contained in the flight line
	tc_flags = filterTreecrowns(shapes, n_records, fline);
	if(!tc_flags){
		return 0;
	}
	
	y_coord = ul_northing;
	
	for(i = 0; i < envi_hdr->lines; i++)
	{
		printf("\t%d\n", i);
		x_coord = ul_easting;
	for(j = 0; j < envi_hdr->samples; j++)
	{
		//x_coord += envi_hdr->info.x_scale;
		if(pointInPolygon(fline, x_coord + half_pix, y_coord - half_pix))
		{
			for(record = 0; record < n_records; record++)
			{
				if(tc_flags[record] == 0)
					continue;
				
				if(pointInPolygon(shapes[record], x_coord + half_pix, y_coord - half_pix))
				{
					HSpecRecord * data = (HSpecRecord *) malloc(sizeof(HSpecRecord));
					
					if(!data){
						puts("malloc failed for hspec record");
						break;//dont add null entry to list
					}
					
					data->x_idx = j;
					data->y_idx = i;
					data->easting = x_coord;
					data->northing = y_coord;
					data->lid_avg = calculateLidarAverage(gridded_zvalues[i][j]);
					data->lid_max = calculateLidarMax(gridded_zvalues[i][j]);
					data->rad_sum = 0;
					data->rank = 0.0;
					data->hspec_values = (unsigned short *)malloc(sizeof(unsigned short) * envi_hdr->bands);
					data->hspec_size = envi_hdr->bands;
					
					if(!addFront(hspec_records[record], data)) 
						return 0;
					
					//each point can only be in 1 treecrown
					break;
				}
			}
		}
		x_coord += envi_hdr->info.x_scale;
	}
	y_coord -= envi_hdr->info.y_scale;
	}
	
	free(tc_flags);
	
	return 1;
}

/**
 * Record hyper spectral values for each band for each pixel that intersects a 
 * tree crown.
 */
int extractHyperspectralValues(char * hyper_path)
{
	puts("extractHyperspectralValues");
	
	int band_idx, record;
	unsigned short ** band;
	
	for(band_idx = 0; band_idx < envi_hdr->bands; band_idx++)
	{
		printf("band_idx : %d \n", band_idx);
		
		band = (unsigned short **) readENVIStandardBand(hyper_path, band_idx, envi_hdr);
			if(!band) return 0;
		
		if(envi_hdr->byte_order == 1)
			swapBandBytes(band, envi_hdr->lines, envi_hdr->samples);
		
		for(record = 0; record < n_records; record++)
		{
			Node * curr = hspec_records[record]->head;
			
			//each pixel
			while(curr != NULL)
			{
				HSpecRecord * temp  = (HSpecRecord*) curr->data;
				
				temp->hspec_values[band_idx] = 
					band[temp->y_idx][temp->x_idx];	
				temp->rad_sum += band[temp->y_idx][temp->x_idx];
				
				curr = curr->next;
			}
		}
		freeus2d(band, envi_hdr->lines);
	}
	
	return 1;
}


/**
 * For each crown, determine the rank of each pixel relative to other pixels
 * in the same crown.
 */
void determineRank(List * hspec_records[], int n_records)
{
	puts("determineRank");
	int record;
	
	for(record = 0; record < n_records; record++)
	{
		if(hspec_records[record]->size == 0)
			continue;
		
		Node * curr = hspec_records[record]->head;
		HSpecRecord * hrec = (HSpecRecord*) curr->data;
		
		int sum = hrec->rad_sum;
		int min = sum, max = sum;
		float range;
		
		//each pixel
		while(curr != NULL)
		{
			hrec = (HSpecRecord*) curr->data;
			sum  = hrec->rad_sum;
			
			if(sum < min) min = sum;
			if(sum > max) max = sum;
			
			curr = curr->next;
		}
		
		range = (float)(max - min);
		curr  = hspec_records[record]->head;
		
		//each pixel
		while(curr != NULL)
		{
			hrec = (HSpecRecord*) curr->data;
			if(range == 0.0)
				hrec->rank = 1.0;
			else
				hrec->rank = (hrec->rad_sum - min) / range;
			
			curr = curr->next;
		}
	}
}

/**
 * If sum of the radiance for all bands of a pixel is 0 then remove it.
 *
 * todo: determine if this in conjunction with 'removeValuesBelowThreshold' is
 *  redundant.
 */
void removeZeroValues(List * hspec_records[], int n_records)
{
	puts("removeZeroValues");
	
	int record;
	
	for(record = 0; record < n_records; record++)
	{
		if(hspec_records[record]->size == 0)
			continue;
		
		Node * curr = hspec_records[record]->head;
		Node * prev = NULL, * temp;
		
		//each pixel
		while(curr != NULL)
		{
			HSpecRecord *
			hrec = (HSpecRecord*) curr->data;
			
			if(hrec->rad_sum <= 0.0)
			{	
				free(hrec->hspec_values);
				
				temp = curr;
				curr = curr->next;
				
				removeNode(hspec_records[record], prev, temp);
			}
			else
			{
				prev = curr;
				curr = curr->next;
			}
		}
	}
}

/**
 * If the rank of a pixel relative to other pixels is below the specified albedo
 * threshold remove it from consideration.
 */
void removeValuesBelowThreshold(List * hspec_records[], int n_records, float threshold)
{
	puts("removeValuesBelowThreshold");
	
	int record;
	
	for(record = 0; record < n_records; record++)
	{
		if(hspec_records[record]->size == 0)
			continue;
		
		Node * curr = hspec_records[record]->head;
		Node * prev = NULL, * temp;
		
		//each pixel
		while(curr != NULL)
		{
			HSpecRecord *
			hrec = (HSpecRecord*) curr->data;
			
			if(hrec->rank < threshold)
			{	
				free(hrec->hspec_values);
				
				temp = curr;
				curr = curr->next;
				
				removeNode(hspec_records[record], prev, temp);
			}
			else
			{
				prev = curr;
				curr = curr->next;
			}
		}
	}
}


void freeHSpecRecords(List * hspec_records [], int n_records)
{
	int record;
	
	for(record = 0; record < n_records; record++)
	{
		List * L = hspec_records[record];
		
		if(L->size == 0)
			continue;
		
		Node * curr = L->head;
		
		while(curr != NULL)
		{
			HSpecRecord * rec = (HSpecRecord *) curr->data;
			
			if(rec->hspec_values)
			free(rec->hspec_values);
			
			curr = curr->next;
		}
		
		freeList(L);
	}
}

int checkIfBoundsIntersect(double ule1, double uln1, double bre1, double brn1, 
	double ule2, double uln2, double bre2, double brn2)
{
	//upper left corner of first range is contained in second range
	if(	(ule1 > ule2 && ule1 < bre2 && uln1 < uln2 && uln1 > brn2) ||
		(ule2 > ule1 && ule2 < bre1 && uln2 < uln1 && uln2 > brn1))
	{
		return 1;
	}
	return 0;
	
}

/**
 * Builds a grid to store the z-values for each lidar point. Every cell
 * in the grid corresponds to a cell in the provided hyperspectral file.
 */
static PyObject* Py_Initialize(PyObject * self, PyObject * args)
{
	char * hyper_path;
	
	if(!PyArg_ParseTuple(args, "s", &hyper_path)){
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	//
	// Open ENVI header for hyperspectral data.
	//
	envi_hdr = getENVIHeaderInfo(hyper_path);
		if(!envi_hdr)return NULL;
	if(envi_hdr->datatype != 12){
		PyErr_SetString(PyExc_StandardError, "Datatype must be unsigned short (12).");
		return NULL;
	}
	if(envi_hdr->info.x_scale != envi_hdr->info.y_scale){
		PyErr_SetString(PyExc_StandardError, "Pixels must be square.");
		return NULL;
	}
	
	//
	// Initialize lidar buffer
	//
	printf("Init gridded_zvalues\n");
	printf("\tlines: %d, samples: %d\n", envi_hdr->lines, envi_hdr->samples);
	
	int line, sample, lines, samples;
	
	lines = envi_hdr->lines;
	samples = envi_hdr->samples;
	
	gridded_zvalues = (List ***) malloc(sizeof(List **) * lines);
		if(!gridded_zvalues){
			PyErr_SetString(PyExc_MemoryError, "malloc failed");
			return NULL;
		}
	for(line = 0; line < lines; line++){
		gridded_zvalues[line] = (List **) malloc(sizeof(List *) * samples);
			if(!gridded_zvalues[line]) return NULL;
			
		for(sample = 0; sample < samples; sample++){
			gridded_zvalues[line][sample] = initList();
				if(!gridded_zvalues[line][sample])return NULL;
		}
	}
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* Py_TeardownPerTCFile(PyObject * self, PyObject * args)
{
	puts("Py_TeardownPerTCFile");
	freeHSpecRecords(hspec_records, n_records);
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* Py_TeardownPerHypFile(PyObject * self, PyObject * args)
{
	puts("Py_TeardownPerHypFile");
	freeGriddedZValues(gridded_zvalues, envi_hdr->lines, envi_hdr->samples);
	free(envi_hdr);
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* Py_DetermineTreeCrownIntersection(PyObject * self, PyObject * args)
{
	puts("Py_DetermineTreeCrownIntersection");
	
	char * hyper_path, * fline_path, * vector_path;
	
	if(!PyArg_ParseTuple(args, "sss", &hyper_path, &fline_path, &vector_path)){
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	int n_shape_type, i, record;
	
	//
	// Open tree crown vector file.
	//
	SHPHandle
	HSHP_polygon = SHPOpen(vector_path, "rb");
	if(!HSHP_polygon){
		PyErr_SetString(PyExc_IOError, "SHPOpen failed. Could not open tree crown file.");
		return NULL;
	}
	SHPGetInfo(HSHP_polygon, &n_records, &n_shape_type, NULL, NULL);
	if(n_shape_type != SHPT_POLYGON){
		PyErr_SetString(PyExc_IOError, "Shape type not SHPT_POLYGON.");
		return NULL;
	}
	
	//
	// Buffer tree crown vectors.
	//
	puts("\tBuffer tree crowns");
	SHPObject ** shapes = (SHPObject**) malloc(sizeof(SHPObject*) * n_records);
	for(record = 0; record < n_records; record++){
		shapes[record] = SHPReadObject(HSHP_polygon, record);
		if(!shapes[record]){
			SHPClose(HSHP_polygon);
			return NULL;
		}
	}
	SHPClose(HSHP_polygon);
	
	//
	// Get TC_Id information from DBFHandle
	//
	
	int * tc_ids = (int *)malloc(sizeof(int)*n_records);
	if(!tc_ids){
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	DBFHandle
	HDBF_polygon = DBFOpen(vector_path, "rb");
	if(!HDBF_polygon){
		PyErr_SetString(PyExc_IOError, "DBFOpen failed. Could not open dbf crown file.");
		return NULL;
	}
	int field_index = DBFGetFieldIndex(HDBF_polygon, "TC_Id");
	
	if(field_index < 0){
		for(record = 0; record <  n_records; record++)
			tc_ids[record] = record;
	}
	else{
		for(record = 0; record <  n_records; record++){
			tc_ids[record] = 
				DBFReadIntegerAttribute(HDBF_polygon, record, field_index);
		}
	}
	DBFClose(HDBF_polygon);
	
	//
	// Open the outline of the flight line
	//
	SHPHandle
	HSHP_fline = SHPOpen(fline_path, "rb");
	if(!HSHP_fline){
		PyErr_SetString(PyExc_IOError, "SHPOpen failed.");
		return NULL;
	}
	
	float half_pix = envi_hdr->info.y_scale / 2;
	double ul_easting  = envi_hdr->info.easting - (envi_hdr->info.x_pixel-1) * envi_hdr->info.x_scale;
	double ul_northing = envi_hdr->info.northing + (envi_hdr->info.y_pixel-1) * envi_hdr->info.y_scale;	
	
	
	
	//
	// Initialize hyperspectral buffer.
	//
	hspec_records = (List **) malloc(sizeof(List *) * n_records);
		if(!hspec_records){
			PyErr_SetString(PyExc_MemoryError, "malloc failed.");
			return NULL;
		}
	for(i = 0; i < n_records; ++i)
		if( !(hspec_records[i] = initList()) )
			return NULL;
		
	//
	// Determining which pixels intersect the tree crowns.
	//
	if(
	!determineIntersectingPixels_fast(
		hspec_records, 
		shapes,
		tc_ids,
		envi_hdr, 
		SHPReadObject(HSHP_fline, 0), 
		ul_easting,
		ul_northing, 
		n_records, 
		half_pix) 
	)
	return NULL;
	
	
	//
	// free memory
	//
	for(record = 0; record < n_records; record++)
		SHPDestroyObject(shapes[record]);
	free(shapes);
	free(tc_ids);
	SHPClose(HSHP_fline);
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* Py_GridLidar(PyObject * self, PyObject * args)
{
	puts("Py_GridLidar");
	
	char * lidar_path;
	
	if(!PyArg_ParseTuple(args, "s", &lidar_path)){
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}

/*
	if(first){
		puts("\tAllocating gridded_zvalues");
		printf("\tlines: %d, samples: %d\n", envi_hdr->lines, envi_hdr->samples);
		//
		// Allocate buffer for gridded z values.
		//
		int lines = envi_hdr->lines;
		int samples = envi_hdr->samples;
		
		int line, sample;
		gridded_zvalues = (List ***) malloc(sizeof(List **) * lines);
			if(!gridded_zvalues){
				PyErr_SetString(PyExc_MemoryError, "malloc failed");
				return NULL;
			}
		for(line = 0; line < lines; line++){
			gridded_zvalues[line] = (List **) malloc(sizeof(List *) * samples);
				if(!gridded_zvalues[line]) return NULL;
				
			for(sample = 0; sample < samples; sample++){
				gridded_zvalues[line][sample] = initList();
					if(!gridded_zvalues[line][sample])return NULL;
			}
		}
	}
*/
	
	double ul_easting  = envi_hdr->info.easting - (envi_hdr->info.x_pixel-1) * envi_hdr->info.x_scale;
	double ul_northing = envi_hdr->info.northing + (envi_hdr->info.y_pixel-1) * envi_hdr->info.y_scale;	
	
	//
	// Grid the lidar points consistent with hyperspectral resolution.
	//
	if(!
	gridLasPoints(
		lidar_path, 
		ul_easting, 
		ul_northing, 
		envi_hdr->lines, 
		envi_hdr->samples, 
		envi_hdr->info.x_scale)
	)
		return NULL;
		
	Py_INCREF(Py_None);
	return Py_None;
}

FILE * openIndexFile(char * out_dir, char * fl_name)
{
	char path [256];
	sprintf(path, "%s/Index_%s.csv", out_dir, fl_name);
	
	return fopen(path, "w");
}

void writePixelsToIndexFile(FILE * fp_index, Node * curr, int record, float ** chm, double ulEasting, double ulNorthing, double * mpsData, int imageLength, int imageWidth)
{
	while(curr != NULL)
	{
		HSpecRecord *
		hrec = (HSpecRecord *) curr->data;
		
		int col = (hrec->easting - ulEasting)/mpsData[0];
		int row = (ulNorthing - hrec->northing)/mpsData[1];
		float z = -1.0;
		if(row >= 0 && row < imageLength && col >= 0 && col < imageWidth)
			z = chm[row][col];
		
		fprintf(fp_index, "%d,%.2lf,%.2lf,%.2f,%d,%.2f\n", hrec->tc_id /*todo: removed->record*/, hrec->easting, hrec->northing, z, hrec->rad_sum, hrec->rank);
		
		//todo: write spectra to file aswell (as below)
		
		curr = curr->next;
	}
}


bool compare_hspec_records(HSpecRecord * x, HSpecRecord * y)
{
	return (x->lid_max > y->lid_max);
}

void writeSubsetIndexFile(FILE * fp_index, Node * curr, int record, float ** chm, double ulEasting, double ulNorthing, double * mpsData, int imageLength, int imageWidth, int n_to_subset)
{
	std::vector<HSpecRecord*> h_records;
	
	while(curr != NULL)
	{
		h_records.push_back((HSpecRecord*)curr->data);
		curr = curr->next;
	}
	
	std::sort(h_records.begin(), h_records.end(), compare_hspec_records);
	
	int i;
	for(i = 0; i < h_records.size()-1; i++){
		if(h_records.at(i)->lid_max < h_records.at(i+1)->lid_max){
			puts("error");
			int j;
			for(j=0;j<h_records.size();j++)
				printf("%f\n",h_records.at(j)->lid_max);
		}
	}
	for(i = 0; i < h_records.size() && i < n_to_subset; ++i)
	{
		HSpecRecord * hrec = h_records.at(i);
		
		int col = (hrec->easting - ulEasting)/mpsData[0];
		int row = (ulNorthing - hrec->northing)/mpsData[1];
		float z = -1.0;
		if(row >= 0 && row < imageLength && col >= 0 && col < imageWidth)
			z = chm[row][col];
		
		//removed >>
		//	fprintf(fp_index, "%d,%.2lf,%.2lf,%.2f,%d,%.2f\n", record, hrec->easting, hrec->northing, z, hrec->rad_sum, hrec->rank);
		//
		//replaced with >>
		fprintf(fp_index, "%d,%.2lf,%.2lf,%.2f,%d,%.2f,", hrec->tc_id /*todo: removed->record*/, hrec->easting, hrec->northing, z, hrec->rad_sum, hrec->rank);
		unsigned short * spectra = hrec->hspec_values;
		int n_values = hrec->hspec_size;
		int i;
		for(i = 0; i < n_values-1; i++)
			fprintf(fp_index, "%d,", spectra[i]);
		fprintf(fp_index, "%d\n", spectra[n_values-1]);
		//end replace
	}
}


const char * writeHyperspectralMetrics(char * out_dir, int record, char * fl_fname, List * list, int n_bands, int num_all [])
{
	HSpecRecord *
	temp = (HSpecRecord *) list->head->data;
	char * hm_path = buildPathForSpectralMetrics(out_dir, "Spectral_Metrics", temp->tc_id/*todo:removed->record*/, fl_fname);
		
	if(!hm_path)
	{
		PyErr_SetString(PyExc_MemoryError, "Could not build file path..");
		return NULL;
	}
		
	FILE * fp_hm = fopen(hm_path, "w");
	
	if(!fp_hm)
	{
		free(hm_path);
		PyErr_SetString(PyExc_IOError, "Could not open bio file..");
		return NULL;
	}
	
	fprintf(fp_hm, "Band Index,Wavelength,# Points,L-Mean,L-Cov,L-Skew,L-Kurtosis\n");
	
	int band;
	for(band = 0; band < n_bands; band++)
	{
		Node * curr = list->head;
		
		int idx = 0;
		float * buffer = allocf1d(list->size);
		
		while(curr != NULL)
		{
			HSpecRecord *
			hrec = (HSpecRecord *) curr->data;
			buffer[idx++] = hrec->hspec_values[band];
			curr = curr->next;
		}
		
		float * 
		lmoments = computeLMoments(buffer, hspec_records[record]->size);
			if(!lmoments)return NULL;
		
		//
		// Write biometrics to file...
		fprintf(fp_hm,"%d,%f,%d,%f,%f,%f,%f\n", band+1, envi_hdr->wavelengths[band], num_all[record], lmoments[0], lmoments[1], lmoments[2], lmoments[3]);
		
		free(buffer);
		free(lmoments);
	}
	
	fclose(fp_hm);
}

static PyObject* Py_HyperspectralExtraction(PyObject * self, PyObject * args)
{
	puts("Py_HyperspectralExtraction");
	
	int subset_by_height, n_to_subset, output_met, output_ext;
	float h_threshold;
	char * hyper_path, * out_dir, * fl_fname, * chm_path;
	
	if(!PyArg_ParseTuple(args, "sfsssiiii", 
		&chm_path, &h_threshold, &hyper_path, &out_dir, &fl_fname, 
		&subset_by_height, &n_to_subset, &output_met, &output_ext)){
		PyErr_SetString(PyExc_IOError, "PyArg_ParseTuple failed.");
		return NULL;
	}
	
	int record, n_bands;
	
	n_bands = envi_hdr->bands;
	
	//
	// Record number of intersecting points for each record
	//
	int num_all[n_records];
	for(record = 0; record < n_records; record++){
		num_all[record] = hspec_records[record]->size;
	}
	
	//
	// Extract values from bands...
	//
	if(!extractHyperspectralValues(hyper_path))
		return NULL;
	
	
	//
	//Remove pixels with rad_sum = 0
	//
	removeZeroValues(hspec_records, n_records);
	
	//
	//Compute the rank
	//
	determineRank(hspec_records, n_records);
	
	//
	// Write hyperspectral values to file.
	//
	if(output_ext){
		writeSpectralSubsetToCSV(hspec_records, n_records, n_bands, out_dir, fl_fname);
	}
	
	//
	// Remove those intersections that don't meet the albedo threshold
	//
	printf("\tnum_all: %d\n", hspec_records[7]->size);
	removeValuesBelowThreshold(hspec_records, n_records, h_threshold);
	printf("\tnum_abv: %d\n", hspec_records[7]->size);
	
	
	//
	// Create index / summary file.
	//
	FILE * fp_index = openIndexFile(out_dir, fl_fname);
		if(fp_index == NULL) return NULL;
	fprintf(fp_index, "CrownId,Easting,Northing,Elevation,Albedo,Rank,");
	int i;
	for(i = 0; i < envi_hdr->bands-1; i++){
		fprintf(fp_index, "%f,", envi_hdr->wavelengths[i]);
	}
	fprintf(fp_index, "%f\n", envi_hdr->wavelengths[i]);
	
		
	//
	// Open chm to extract z-values
	//
	TIFF *tiff = XTIFFOpen((const char*)chm_path, "r");
	if(!tiff){
		PyErr_SetString(PyExc_IOError, "XTIFFOpen failed.");
		return NULL;
	}
	float ** chm = readTIFF(tiff);
	if(!chm){
		PyErr_SetString(PyExc_IOError, "Error buffering tiff.");
		return NULL;
	}
	
	short mttCount, mpsCount;
	int imageLength, imageWidth;
	double * mpsData, * mttData;
	TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH,  &imageWidth);
	getModelTiePointTag   (tiff, &mttData, &mttCount);
	getModelPixelScaleTage(tiff, &mpsData, &mpsCount);
	double ulEasting  = getUlEasting(mpsData, mpsCount, mttData, mttCount);
	double ulNorthing = getUlNorthing(mpsData, mpsCount, mttData, mttCount);
	
	if(ulEasting < 0 || ulNorthing < 0){
		printf("1st:%lf,%lf,%d,%d\n", ulEasting, ulNorthing, mpsCount, mttCount);
		PyErr_SetString(PyExc_ValueError, "ulEasting < 0 || ulNorthing < 0");
		return NULL;
	}
	if(mpsCount < 2){
		printf("2nd:%lf,%lf,%d,%d\n", ulEasting, ulNorthing, mpsCount, mttCount);
		PyErr_SetString(PyExc_ValueError, "mpsCount < 3");
		return NULL;
	}
	
	
	
	//
	// For each record and each band.. extract the hyperspectral values,
	//  compute biometrics on them, then write them to file.
	//
	puts("\tBuilding bio list / compute bio / write to file");

	for(record = 0; record < n_records; record++)
	{
		List * list = hspec_records[record];
		
		if(list->size == 0)
			continue;
		
		if(subset_by_height)
			writeSubsetIndexFile(fp_index, list->head, record, chm, ulEasting, ulNorthing, mpsData, imageLength, imageWidth, n_to_subset);
		else
			writePixelsToIndexFile(fp_index, list->head, record, chm, ulEasting, ulNorthing, mpsData, imageLength, imageWidth);
		
		if(output_met){
			writeHyperspectralMetrics(out_dir, record, fl_fname, list, n_bands, num_all);
		}
		/*
		char * hm_path = buildPathForSpectralMetrics(out_dir, "Spectral_Metrics", record, fl_fname);
			if(!hm_path){PyErr_SetString(PyExc_MemoryError, "Could not build file path..");return NULL;}
		FILE * fp_hm = fopen(hm_path, "w");
			if(!fp_hm){printf("%s\n", hm_path);printf("%s\n", hm_path);PyErr_SetString(PyExc_IOError, "Could not open bio file..");return NULL;}
			free(hm_path);
		
			
		fprintf(fp_hm, "Band Index,Wavelength,# Points,L-Mean,L-Cov,L-Skew,L-Kurtosis\n");
			
		for(band = 0; band < n_bands; band++)
		{
			Node * curr = list->head;
			
			idx = 0;
			buffer = allocf1d(list->size);
			
			while(curr != NULL)
			{
				HSpecRecord *
				hrec = (HSpecRecord *) curr->data;
				buffer[idx++] = hrec->hspec_values[band];
				curr = curr->next;
			}
			
			float * 
			lmoments = computeLMoments(buffer, hspec_records[record]->size);
				if(!lmoments)return NULL;
			
			//
			// Write biometrics to file...
			fprintf(fp_hm,"%d,%f,%d,%f,%f,%f,%f\n", band+1, envi_hdr->wavelengths[band], num_all[record], lmoments[0], lmoments[1], lmoments[2], lmoments[3]);
			
			free(buffer);
			free(lmoments);
		}
		
		fclose(fp_hm);
		*/
		//end todo
	}
	
	fclose(fp_index);
	
	Py_INCREF(Py_None);
	return Py_None;
}


static PyMethodDef TCM_module_methods[] = {
	{"DetermineTreeCrownIntersection", Py_DetermineTreeCrownIntersection, METH_VARARGS},
	{"GridLidar", Py_GridLidar, METH_VARARGS},
	{"HyperspectralExtraction", Py_HyperspectralExtraction, METH_VARARGS},
	{"Initialize", Py_Initialize, METH_VARARGS},
	{"TeardownPerTCFile", Py_TeardownPerTCFile, METH_VARARGS},
	{"TeardownPerHypFile", Py_TeardownPerHypFile, METH_VARARGS},
	{"treeCrownMetrics", Py_TreeCrownMetrics, METH_VARARGS},
	{NULL, NULL}
};

PyMODINIT_FUNC 
initTCM_module(void)
{
	(void) Py_InitModule("TCM_module", TCM_module_methods);
}
