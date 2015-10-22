

#include "treetop_metrics.h"

#define ModelPixelScaleTag	33550	//geotiff tag id
#define ModelTiepointTag	33922	//geotiff tag id

/**
 * 0 - # of treetops
 * 1 - treetops / hectare
 * 2 - min height
 * 3 - max height
 * 4 - mean height
 * 5 - variance
 */

//
//reference image size
int blk_size, image_width, image_length;

//
//reference image position info
int mps_size, mtt_size;
double * mps_data, *mtt_data;
double  ul_east, ul_north;

//
//metric info
int 	n_metrics = 6, max_treetops;
float *** metrics;
float *** z_coords;
int    ** z_counts;
char * metric_names [] = {"points_per_pixel", "points_per_hectar", "min_height",
			"max_height", "mean_height", "variance"};

/**
 * Get the pixel size and upper left pixel location from the reference file.
 */
int treetop_metrics_init(char * ref_file)
{
	TIFF * ref_tiff;
	ref_tiff = TIFFOpen((const char *)ref_file, "r");

	if(ref_tiff == NULL)
	{
		return 0;
	}
	
	TIFFGetField(ref_tiff, ModelPixelScaleTag, &mps_size, &mps_data);
	TIFFGetField(ref_tiff, ModelTiepointTag,   &mtt_size, &mtt_data);
	TIFFGetField(ref_tiff, TIFFTAG_IMAGEWIDTH, &image_width);
	TIFFGetField(ref_tiff, TIFFTAG_IMAGEWIDTH, &image_length);
	
	if(mps_size < 3 || mtt_size < 6)
	{
		printf("Not enough model pixel scale or model tiepoint data\n");
		return 0;
	}
	
	if(mps_data[0] != mps_data[1])
	{
		printf("Pixels aren't square.\n");
		return 0;
	}
	
	blk_size = (int)mps_data[0];
	max_treetops = blk_size;
	ul_east  = mtt_data[3] - (mtt_data[0] * blk_size);
	ul_north = mtt_data[4] + (mtt_data[1] * blk_size);
	
	metrics = allocf3d(n_metrics, image_length, image_width);
	z_coords = allocf3d(image_length, image_width, max_treetops);
	z_counts = alloci2d(image_length, image_width);
	
	if(metrics == NULL || z_coords == NULL || z_counts == NULL)
		return 0;
	
	//init metrics to 0
	int i,j,k;
	for(i = 0; i < n_metrics; i++)
	{
		for(j = 0; j < image_length; j++)
		{
			for(k = 0; k < image_width; k++)
			{
				metrics[i][j][k] = 0.0f;
			}
		}
	}
	
	//init z_coords & z_counts to 0
	for(i = 0; i < image_length; i++)
	{
		for(j = 0; j < image_width; j++)
		{
			z_counts[i][j] = 0;
			for(k = 0; k < max_treetops; k++)
			{
				z_coords[i][j][k] = 0.0f;
			}
		}
	}
	
	printf("ul_east: %lf\n", ul_east);
	printf("ul_north: %lf\n", ul_north);
	printf("image_width: %d\n", image_width);
	printf("image_length: %d\n", image_length);
	printf("blk_size: %d\n", blk_size);
	
	return 1;
}

/**
 * Grid the shape file pointed to by 'tt_path' updating
 * global data structures.
 */
int treetop_metrics_grid(char * tt_path)
{
	SHPHandle hshp;
	hshp = SHPOpen(tt_path, "rb");
	if(!hshp)
		return 0;
	
	DBFHandle hdbf;
	hdbf = DBFOpen(tt_path, "rb");
	if(!hdbf)
		return 0;
	
	
	int i, n_shapes, shape_type;
	
	SHPGetInfo(hshp, &n_shapes, &shape_type, NULL, NULL);
	
	if(shape_type != SHPT_POINT)
	{
		printf("Shape type isn't point.\n");
		return 0;
	}
	
	float hect_ratio = (100*100) / (blk_size*blk_size);
	
	for(i = 0; i < n_shapes; i++)
	{
		SHPObject *
		p_shp_obj = SHPReadObject(hshp, i);
		
		int n_verts = p_shp_obj->nVertices;
		
		if(n_verts < 1)
			return 0;
		
		double x_coord = p_shp_obj->padfX[0];
		double y_coord = p_shp_obj->padfY[0];
		int field_idx = DBFGetFieldIndex(hdbf, "Elevation");
		if(field_idx == -1)
		{
			printf("Field 'Elevation' does not exist.\n");
			return 0;
		}
		float z_coord = (float)DBFReadDoubleAttribute(hdbf, i, field_idx);
		
		SHPDestroyObject(p_shp_obj);
		
		//grid these coordinates in some data structure
		int col = (int)( (x_coord - ul_east) / blk_size );
		int row = (int)( (ul_north - y_coord) / blk_size );
		
		if(row < 0 || row >= image_length || col < 0 || col >= image_width)
		{
			printf("Out of bounds: %d, %d\n", row, col);
			continue;
		}
		
		int n = z_counts[row][col]++;
		if(n == max_treetops){
			n = max_treetops;
			z_counts[row][col] = max_treetops;
		}
		z_coords[row][col][n] = z_coord;
		
		metrics[0][row][col]++; //# of trees
		metrics[1][row][col] += hect_ratio; //density per hectare
		
		if(z_coord < metrics[2][row][col] || metrics[3][row][col] == 0)
			metrics[2][row][col] = z_coord;  //min height
		
		if(z_coord > metrics[3][row][col])
			metrics[3][row][col] = z_coord; //max height
	}
	
	SHPClose(hshp);
	DBFClose(hdbf);

	return 1;
}

char * build_output_path(char * ttm_path, int metric_idx)
{
	char * out_path = (char*)malloc(256);
	
	strcpy(out_path, ttm_path);
	
	//find .tif
	int i;
	for(i = 0; i < 256-4; i++)
	{
		char * str = out_path + i;
		if(strcmp(str, ".tif") == 0 || strcmp(str, ".tiff") == 0)
		{
			break;
		}
	}
	
	if(i == 256-4){
		return NULL;
	}
	
	out_path[i] = '\0';
	strcat(out_path, "_");
	strcat(out_path, metric_names[metric_idx]);
	strcat(out_path, ".tif");
	
	return out_path;
	
}

int treetop_metrics_output(char * ttm_path)
{
	int band, row;
	for(band = 0; band < n_metrics; band++)
	{
		TIFF * out_tiff;
		GTIF * out_gtif;
		char * out_path = build_output_path(ttm_path, band);
		
		if(out_path == NULL){
			return 0;
		}
		
		if( (out_tiff = XTIFFOpen(out_path, "w")) == NULL )
			return 0;
		
		if( (out_gtif = GTIFNew(out_tiff)) == NULL )
			return 0;
	
		printf("Output path: %s\n", out_path);
		
		TIFFSetField(out_tiff, TIFFTAG_IMAGEWIDTH, 		image_width);
		TIFFSetField(out_tiff, TIFFTAG_IMAGELENGTH, 	image_length);
		TIFFSetField(out_tiff, TIFFTAG_BITSPERSAMPLE, 	32);
		TIFFSetField(out_tiff, TIFFTAG_SAMPLESPERPIXEL, 1);
		TIFFSetField(out_tiff, TIFFTAG_COMPRESSION, 	COMPRESSION_NONE);
		TIFFSetField(out_tiff, TIFFTAG_PHOTOMETRIC, 	PHOTOMETRIC_MINISBLACK);
		TIFFSetField(out_tiff, TIFFTAG_DATATYPE, 		3);	//IEEE floating point
		TIFFSetField(out_tiff, TIFFTAG_ORIENTATION, 	ORIENTATION_TOPLEFT);
		//geotiff tags
		TIFFSetField(out_tiff,	ModelPixelScaleTag, mps_size, mps_data);
		TIFFSetField(out_tiff,	ModelTiepointTag,	mtt_size, mtt_data);
		
		for(row = 0; row < image_length; row++)
		{
			if(TIFFWriteScanline(out_tiff, metrics[band][row], row, 0) == -1)
			{
				printf("treetop_metrics:output; failed to write scanline\n");
				printf("at\t%d, %d\n", band, row);
				return 0;
			}
		}
		
		GTIFFree(out_gtif);
		TIFFClose(out_tiff);
		free(out_path);
	}
	
	return 1;
}

float compute_mean(float * arr, int size)
{
	if(size <= 0)
	{		
		return 0.0f;
	}
	
	float sum = 0.0f;
	int i;
	
	for(i = 0; i < size; i++)
	{
		sum += arr[i];
	}
	
	if(sum == 0.0f)
	{
		return 0.0f;
	}
	return sum / size;
}

float compute_variance(float * arr, int size)
{
	float mean, diff_sum = 0.0;
	int i;
	
	mean = compute_mean(arr, size);
	
	for(i = 0; i < size; i++)
	{
		diff_sum += pow( arr[i] - mean, 2);
	}
	
	if(diff_sum == 0.0f)
		return 0.0f;
	
	return diff_sum / size;
}

void treetop_metrics_finish(char * ttm_path)
{
	int i, j, size;
	
	//compute mean
	for(i = 0; i < image_length; i++)
	{
		for(j = 0; j < image_width; j++)
		{
			size = z_counts[i][j];
			metrics[4][i][j] = compute_mean(z_coords[i][j], size);
			metrics[5][i][j] = compute_variance(z_coords[i][j], size);
		}
	}
	
	treetop_metrics_output(ttm_path);
	
	free(mps_data);
	free(mtt_data);
	freei2d(z_counts, image_length);
	freef3d(z_coords, image_length, image_width);
	freef3d(metrics, n_metrics, image_length);
}



