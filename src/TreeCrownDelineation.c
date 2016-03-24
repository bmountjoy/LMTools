
#include <Python/Python.h>
#include <math.h>
#include "TCD_module.h"


#define EMPTY  0 //pixel not in crown
#define CROWN  1 //pixel part of tree crown
#define TRTOP  2 //pixel at tree top

// Used to prevent the treecrown file from exceeding 2 GB
#define MAX_TREE_CROWNS_PER_FILE 2000000

/**
 * image is a mask centered the treetop position
 */
void build_mask(float ** image, char ** tc_mask, char ** visited, float tree_height, float p_value, int wnd_size, int x_ref, int y_ref)
{	
	visited[y_ref][x_ref] = 1;
	
	//printf("%d %d \n", x_ref, y_ref);
	
	int x_offsets[] = {-1,  0,  1, -1, 1, -1, 0, 1};
	int y_offsets[] = {-1, -1, -1,  0, 0,  1, 1, 1};
	
	int i, x, y;
	for(i = 0; i < 8; i++)
	{
		x = x_ref + x_offsets[i];
		y = y_ref + y_offsets[i];
		
		//check if in range
		if(x < 0 || y < 0 || x >= wnd_size|| y >= wnd_size)
			continue;
		
		if(	image[y][x] <= image[y_ref][x_ref] &&
			image[y][x] >= p_value * tree_height)
		{
			tc_mask[y][x] = 1;
		}
	}

	for(i = 0; i < 8; i++)
	{
		x = x_ref + x_offsets[i];
		y = y_ref + y_offsets[i];
		
		//check if in range
		if(x < 0 || y < 0 || x >= wnd_size|| y >= wnd_size)
			continue;
		
		if(tc_mask[y][x] == 1 && visited[y][x] == 0)
			build_mask(image, tc_mask, visited, tree_height, p_value, wnd_size, x, y);
	}
}

/**
 * Add an option here to select smoothing type.
 */
float ** tcd_smooth(float ** buff, int length, int width, int smooth_type)
{

	float ** smoothed = allocf2d(length, width);
		if(!smoothed)
			return NULL;
		
	int i, j, ii, jj; float sum;
	
/*
	//fill in edges
	for(i = 0; i < length; i++){
		smoothed[i][0] = buff[i][0];
		smoothed[i][1] = buff[i][1];
		smoothed[i][width-1] = buff[i][width-1];
		smoothed[i][width-2] = buff[i][width-2];
	}
	
	for(j = 0; j < width; j++){
		smoothed[0][j] = buff[0][j];
		smoothed[1][j] = buff[1][j];
		smoothed[length-1][j] = buff[length-1][j];
		smoothed[length-2][j] = buff[length-2][j];
	}
*/
	for(i = 0; i < length; i++)
	{
		for(j = 0; j < width; j++)
		{
			smoothed[i][j] = buff[i][j];
		}
	}
	
	switch(smooth_type)
	{
		
	case 1:
		break;
		
	//3x3 boxcar
	case 2:
		
		for(i = 1; i < length-1; i++){
		for(j = 1; j < width-1;  j++)
		{
			sum = 0.0;
			for(ii = i-1; ii <= i+1; ii++)
			for(jj = j-1; jj <= j+1; jj++)
				sum += buff[ii][jj];
			
			smoothed[i][j] = sum / 9.0;			
		}}
		
		break;
		
	//gaussian
	case 3:
		{
	
		float g3 [3][3] = {
			{0.0751136, 0.123841, 0.0751136},
			{0.1238410, 0.204180, 0.1238410},
			{0.0751136, 0.123841, 0.0751136}};
			
		
		for(i = 1; i < length-1; i++){
			for(j = 1; j < width-1; j++)
			{
				sum = 0.0;
				for(ii = 0; ii < 3; ii++)
				for(jj = 0; jj < 3; jj++)
					sum += buff[i-1+ii][j-1+jj] * g3[ii][jj];
				
				smoothed[i][j] = sum;
			}
		}	
		
		break;
		}
		
	//gaussian
	case 4:
		{
		/*
		float g5 [5][5] = {
			{0.00296902, 0.0133062, 0.0219382, 0.0133062, 0.00296902},
			{0.01330620, 0.0596343, 0.0983203, 0.0596343, 0.0133062},
			{0.02193820, 0.0983203, 0.1621030, 0.0983203, 0.0219382},
			{0.01330620, 0.0596343, 0.0983203, 0.0596343, 0.0133062},
			{0.00296902, 0.0133062, 0.0219382, 0.0133062, 0.00296902}};
		*/
		float g5 [5][5] = { 
			{0.01247764154323288, 0.02641516735431067, 0.03391774626899505, 0.02641516735431067, 0.01247764154323288}, 
			{0.02641516735431067, 0.05592090972790156, 0.07180386941492609, 0.05592090972790156, 0.02641516735431067}, 
			{0.03391774626899505, 0.07180386941492609, 0.09219799334529226, 0.07180386941492609, 0.03391774626899505}, 
			{0.02641516735431067, 0.05592090972790156, 0.07180386941492609, 0.05592090972790156, 0.02641516735431067}, 
			{0.01247764154323288, 0.02641516735431067, 0.03391774626899505, 0.02641516735431067, 0.01247764154323288}};
			
		
		for(i = 2; i < length-2; i++){
			for(j = 2; j < width-2; j++)
			{
				sum = 0.0;
				for(ii = 0; ii < 5; ii++)
				for(jj = 0; jj < 5; jj++)
					sum += buff[i-2+ii][j-2+jj] * g5[ii][jj];
				
				smoothed[i][j] = sum;
			}
		}
		
		break;
		}
		
	//do nothing
	default:
		return NULL;
	}
	
	return smoothed;
}

/**
 * Find the max value in a 3x3 window centered around buff(y,x) and returns
 * the maximum value and its location in the variables 'p_maxh', 'p_maxx', 
 * and 'p_maxy'.
 */
void find_treetop(float ** buff, int x, int y, float * p_maxh, int * p_maxx, int * p_maxy)
{
	int i,j;
	
	*p_maxh = buff[y-1][x-1];
	*p_maxx = x-1;
	*p_maxy = y-1;
	
	for(i = y-1; i <= y+1; i++)
	for(j = x-1; j <= x+1; j++)
	if(buff[i][j] > *p_maxh){
		*p_maxh = buff[i][j];
		*p_maxx = j;
		*p_maxy = i;
	}
}

void set_radius_perc(
	float tt_height, 
	int run_h1,		float h1_min, 	float h1_max, 
	int run_h2, 	float h2_min, 	float h2_max, 
	int run_h3, 	float h3_min, 	float h3_max,
	int rad_1,  	int rad_2,    	int rad_3, 
	float perc_1, 	float perc_2, 	float perc_3,
	int * p_radius, 
	float * p_percentile)
{	
	if(run_h1 == 1 && tt_height >= h1_min && tt_height < h1_max)
	{
		*p_radius = rad_1;
		*p_percentile = perc_1;
	}
	else if(run_h2 == 1 && tt_height >= h2_min && tt_height < h2_max)
	{
		*p_radius = rad_2;
		*p_percentile = perc_2;
	}
	else if(run_h3 == 1 && tt_height >= h3_min && tt_height < h3_max)
	{
		*p_radius = rad_3;
		*p_percentile = perc_3;
	}
	else
	{
		*p_radius = -1;
	}
	
	return;
}


/**
 * Remove isolated points from the tree crown.
 */
void removeIsolatedPoints(char ** mask, int size)
{
	int i, j, ii, jj, count;
	for(i = 1; i < size-1; i++){
		for(j = 1; j < size-1; j++)
		{
			if(mask[i][j] == 0 || mask[i][j] == 2)
				continue;
			
			count = 0;
			
			for(ii = i - 1; ii <= i + 1; ii++)
			for(jj = j - 1; jj <= j + 1; jj++)
			{
				if(ii == i && jj == j)
					continue;
				
				if(mask[ii][jj] == 0)
					count++;
			}
			
			//point is too isolated to be in crown
			if(count >= 7)
				mask[i][j] = 0;
		}
	}
}

/**
 * Fill in holes in the crown.
 */
void fillHoles(char ** mask, int size)
{
	int i, j, ii, jj, count;
	for(i = 1; i < size-1; i++){
		for(j = 1; j < size-1; j++)
		{
			if(mask[i][j] != 0)
				continue;
			
			count = 0;
			
			for(ii = i - 1; ii <= i + 1; ii++)
			for(jj = j - 1; jj <= j + 1; jj++)
			{
				if(ii == i && jj == j)
					continue;
				
				if(mask[ii][jj] != 0)
					count++;
			}
			
			//point is too isolated to be in crown
			if(count >= 6)
				mask[i][j] = 1;
		}
	}
}

void clearEdges(char ** mask, int size)
{
	int i;
	
	for(i = 0; i < size; i++)
	{
		mask[i][0] = 0;
		mask[i][size-1] = 0;
		mask[0][i] = 0;
		mask[size-1][i] = 0;
	}
}

void shapeMask(char ** mask, int radius)
{
	int x_coords [100];
	int y_coords [100];
	int n_points = 0;
	int wsize = radius + radius + 1;
	
	int r, max_radius = (int)(1.4142 * radius + 0.05);
	
	for(r = /*0*/2; r < max_radius; r++)
	{
		n_points = 0;
		int i;
		for(i = 0; i < 100; i++)
		{
			int x = (int)(radius + r * cos(2 * 3.14159265 * i / 100.0) + 0.5);
			int y = (int)(radius + r * sin(2 * 3.14159265 * i / 100.0) + 0.5);
			
			if(x < 0) x = 0;
			if(y < 0) y = 0; 
			if(x >= wsize) x = wsize-1;
			if(y >= wsize) y = wsize-1;
			
			if(n_points == 0){
				x_coords[n_points] = x;
				y_coords[n_points] = y;
				n_points++;
				continue;
			}
			
			if( (x == x_coords[n_points-1] && y == y_coords[n_points-1]) ||
				(x == x_coords[0]          && y == y_coords[0]))
			{
				continue;
			}
			
			x_coords[n_points] = x;
			y_coords[n_points] = y;
			n_points++;
		}
		
		int n_ones = 0;
		for(i = 0; i < n_points; i++)
		{
			if(mask[y_coords[i]][x_coords[i]] == 1)
				n_ones++;
		}
		
		/**
		 * Not enough ones, set everything to zero.
		 */
/*
		if((float)n_ones/n_points < 0.35)
		{
			for(i = 0; i < n_points; i++)
				mask[y_coords[i]][x_coords[i]] = 0;
		}
*/
		if((float)n_ones/n_points < 0.35)
		{
			break;
		}
	}
	
	for(; r < max_radius; r++)
	{
		n_points = 0;
		int i;
		for(i = 0; i < 100; i++)
		{
			int x = (int)(radius + r * cos(2 * 3.14159265 * i / 100.0) + 0.5);
			int y = (int)(radius + r * sin(2 * 3.14159265 * i / 100.0) + 0.5);
			
			if(x < 0) x = 0;
			if(y < 0) y = 0; 
			if(x >= wsize) x = wsize-1;
			if(y >= wsize) y = wsize-1;
			
			if(n_points == 0){
				x_coords[n_points] = x;
				y_coords[n_points] = y;
				n_points++;
				continue;
			}
			
			if( (x == x_coords[n_points-1] && y == y_coords[n_points-1]) ||
				(x == x_coords[0]          && y == y_coords[0]))
			{
				continue;
			}
			
			x_coords[n_points] = x;
			y_coords[n_points] = y;
			n_points++;
		}
		
		/**
		 * Not enough ones, set everything to zero.
		 */
		for(i = 0; i < n_points; i++)
			mask[y_coords[i]][x_coords[i]] = 0;
	}
}

void post_process(char ** mask, int radius, int shape_crown)
{
	int size = radius + radius + 1;
	
	clearEdges(mask, size);
	
	if(shape_crown==1)
	{
		removeIsolatedPoints(mask,size);
		shapeMask(mask, radius);
		removeIsolatedPoints(mask,size);
		fillHoles(mask,size);
	}
}

int compute_radius(float tt_height)
{
	float a = 1.54;
	float b = 0.123;
	
	float radius = a + b * tt_height;
	
	int rad = (int)(radius + 0.5);
	
	if(rad % 2 == 0)
		rad++;
	
	return rad;
}


void writeContour(DBFHandle HDBF_treetops, SHPHandle HSHP_treeCrowns, DBFHandle HDBF_treeCrowns, SHPObject * contour, int tt_index, int tc_index)
{
	int n_records;
	
	SHPWriteObject(HSHP_treeCrowns, -1, contour);
	SHPDestroyObject(contour);
	
	SHPGetInfo(HSHP_treeCrowns, &n_records, NULL, NULL, NULL);
		
	DBFWriteIntegerAttribute(
		HDBF_treeCrowns, 
		n_records-1,
		DBFGetFieldIndex(HDBF_treeCrowns, "TC Id"),
		tc_index
	);
	
	DBFWriteIntegerAttribute(
		HDBF_treeCrowns, 
		n_records-1, 
		DBFGetFieldIndex(HDBF_treeCrowns, "TT Index"),
		tt_index
	);
	
	DBFWriteIntegerAttribute(
		HDBF_treetops, 
		tt_index, 
		DBFGetFieldIndex(HDBF_treetops, "TC Id"),
		tc_index
	);
}

int * bubbleSortTreetops(DBFHandle HDBF_treetops, int n_records)
{
	int i, j = 0;
	int * a = alloci1d(n_records);
	if(!a) return NULL;
	
	for(i = 0; i < n_records; i++)
		a[i] = i;
	
	int field_index = DBFGetFieldIndex(HDBF_treetops, "Height");
	float * h = allocf1d(n_records);
	
	for(i = 0; i < n_records; i++){
		h[i] = (float)DBFReadDoubleAttribute(HDBF_treetops, i, field_index);
	}
	
	for(i = 0; i < n_records-1; i++)
	{
		for(j = 0; j < n_records-1-i; j++)
		{	
			if(h[a[j]] > h[a[j+1]])
			{//swap
				int temp = a[j];
				a[j] = a[j+1];
				a[j+1] = temp;
			}
		}
	}
	free(h);
	return a;
}

int initializeNewTreeCrownShapeFile(const char * base, int n_tree_crowns, int nRecords, SHPHandle * shpHandle, DBFHandle * dbfHandle)
{
	char path [512];

	// Don't partition the file if the number of tree tops is less than the max
	// number of tree crowns per file. Becuase not every TT results in a TC
	// we may have a single partition "part1.shp"
	if (nRecords <= MAX_TREE_CROWNS_PER_FILE) {
		sprintf(path, "%s", base);
	} else {
		int part = (int)(n_tree_crowns / MAX_TREE_CROWNS_PER_FILE) + 1;
		sprintf(path, "%s_part%d", base, part);
	}

	/**
	 * Open ouput shapefile to contain tree crowns. 
	 */
	SHPHandle
	HSHP_treeCrowns = SHPCreate(path, SHPT_POLYGON);
	if (!HSHP_treeCrowns) {
		PyErr_SetString(PyExc_IOError, "SHPCreate failed. File could already exist.");
		return 0;
	}

	*shpHandle = HSHP_treeCrowns;
	
	DBFHandle
	HDBF_treeCrowns = DBFCreate(path);
	if (!HDBF_treeCrowns) {
		PyErr_SetString(PyExc_IOError, "DBFCreate failed. File could already exist.");
		return 0;
	}

	*dbfHandle = HDBF_treeCrowns;

	DBFAddField(HDBF_treeCrowns, "TC Id",    FTInteger, 10, 0);
	DBFAddField(HDBF_treeCrowns, "TT Index", FTInteger, 10, 0);

	return 1;
}

int compare (const void* p1, const void* p2) {
	TTPos3D tt1 = *((TTPos3D*)p1);
	TTPos3D tt2 = *((TTPos3D*)p2);
	if (tt1.z < tt2.z)
		return -1;
	if (tt1.z == tt2.z)
		return  0;
	return 1;
}

/**
 * Builds a tree crown around each treetop in HSHP_treetops from the buffer 'inTif'
 * and write each crown to 'HSHP_treeCrowns'.
 * 
 * Assumptions:
 * - mpsData[0-2] must exist
 */
int tcd (SHPHandle HSHP_treetops, DBFHandle HDBF_treetops, const char * tc_out_path,
	float ** inTif, int length, int width, 
	double ulEasting, double ulNorthing, 
	int run_h1, float h1_min, float h1_max,
	int run_h2, float h2_min, float h2_max,
	int run_h3, float h3_min, float h3_max, 
	float perc_1, float perc_2, float perc_3, 
	int rad_1, int rad_2, int rad_3,			// radius in meters
	double * mpsData, int smooth_type, int shape_crown)
{	
	int i,j, nRecords, nShapeType, wsize, tree_x, tree_y, record, radius;
	float percentile, tt_height;
	char  ** visited,  ** tc_mask;
	float ** tc_wnd, ** buff;
	
	printf("Ul Easting %lf \n", ulEasting);
	printf("Ul Northing %lf \n", ulNorthing);
	printf("scale_x = %lf, scale_y = %lf\n", mpsData[0], mpsData[1]);

	SHPGetInfo(HSHP_treetops, &nRecords, &nShapeType, NULL, NULL);
	if(nShapeType != SHPT_POINT){
		PyErr_SetString(PyExc_IOError, "Shape type not SHPT_POINT.");
		return 0;
	}
	
	int max_pix_rad = (int)(rad_3 / mpsData[0] + 0.5);
	int max_pix_wnd = max_pix_rad + max_pix_rad + 1;
	
	//todo:remove
	printf("max pix wnd = %d\n", max_pix_wnd);
	
	tc_wnd  = allocf2d(max_pix_wnd,max_pix_wnd); if(!tc_wnd) return 0;
	tc_mask = (char **) allocv2d(max_pix_wnd,max_pix_wnd,1); if(!tc_mask)return 0;
	visited = (char **) allocv2d(max_pix_wnd,max_pix_wnd,1); if(!visited)return 0;
	
	buff = tcd_smooth(inTif, length, width, smooth_type);
		if(!buff) return 0;
	freef2d(inTif, length);
	
	/**
	 * Open the shape file.
	 */
	unsigned int n_tree_crowns = 0;
	SHPHandle HSHP_treeCrowns;
	DBFHandle HDBF_treeCrowns;
	if (!initializeNewTreeCrownShapeFile(tc_out_path, n_tree_crowns, nRecords, &HSHP_treeCrowns, &HDBF_treeCrowns))
		return 0;

	// Read all treetops and sort them by height
	TTPos3D *treeTops = (TTPos3D*)malloc(sizeof(TTPos3D) * nRecords);
	if (!treeTops) {
		return 0;
	}

	int heightIndex = DBFGetFieldIndex(HDBF_treetops, "Height");
	for (record = 0; record < nRecords; record++) {
		SHPObject *pShape = SHPReadObject(HSHP_treetops, record);
		treeTops[record].index = record;
		treeTops[record].x = (int)((pShape->padfX[0] - ulEasting) /mpsData[0]  + 0.5);
		treeTops[record].y = (int)((ulNorthing - pShape->padfY[0])/mpsData[1]  + 0.5);
		treeTops[record].z = DBFReadDoubleAttribute(
								HDBF_treetops,
								record,
								heightIndex);
	}

	qsort(treeTops, nRecords, sizeof(TTPos3D), compare);

	/**
	 * Loop through each treetop.
	 */
	for(record = 0; record < nRecords; record++)
	{
		printf("\trecord: %d/%d\n", record+1, nRecords);
		//SHPObject *
		//pShape = SHPReadObject(HSHP_treetops, record);	//sorted[record]);
		//tree_x = (int)( (pShape->padfX[0] - ulEasting) /mpsData[0]  + 0.5);
		//tree_y = (int)( (ulNorthing - pShape->padfY[0])/mpsData[1]  + 0.5);
		tree_x = treeTops[record].x;
		tree_y = treeTops[record].y;
		
		printf("tree_x: %d, tree_y: %d, width: %d\n", tree_x, tree_y, width);
		
		if(tree_x < 0 || tree_x >= width || tree_y < 0 || tree_y >= length){
			PyErr_SetString(PyExc_IOError, "Treetop index out of bounds.");
			return 0;
		}	
		if(tree_x < 1 || tree_x >= width-1 || tree_y < 1 || tree_y >= length-1)
			continue;
		
		if(buff[tree_y][tree_x] == 0)
			continue;
		
		// Use highest point in smoothed buffer as treetop
		find_treetop(buff, tree_x, tree_y, &tt_height, &tree_x, &tree_y);
		 
		set_radius_perc(tt_height, run_h1, h1_min, h1_max, run_h2, h2_min, h2_max, run_h3, h3_min, h3_max, rad_1, rad_2, rad_3, 
			perc_1, perc_2, perc_3, &radius, &percentile);
		if(radius < 0)
			continue;
		
		radius = (int)(radius / mpsData[0] + 0.5);
		
		printf("radius: %d\n", radius);
		
		// Too close to the edge so skip
		if(tree_x < radius || tree_x >= width-radius || tree_y < radius || tree_y >= length-radius)
			continue;
		
		wsize = radius + radius + 1;
		
		// Create mask centered at tree top
		for(i = 0; i < wsize; i++)
		for(j = 0; j < wsize; j++){
			tc_wnd[i][j] = buff[tree_y-radius+i][tree_x-radius+j];
			tc_mask[i][j] = 0;
			visited[i][j] = 0;
		}
		
		// Treetop at tc_mask[radius][radius]
		tc_mask[radius][radius] = 2;
		build_mask(tc_wnd, tc_mask, visited, tt_height, percentile, wsize, radius, radius);
		
		// Post-process the mask so that it can be traced properly and resembles a tree crown
		post_process(tc_mask, radius, shape_crown);
		
		// Build the tree crown vector
		SHPObject * contour = trace(tc_mask, wsize, wsize, ulEasting + (tree_x - radius) * mpsData[0], ulNorthing - (tree_y - radius) * mpsData[1], mpsData[0], mpsData[1]);
		if (!contour) continue;
		
		++n_tree_crowns;

		// Write the current tree crown file to disk and open a new partition
		if (n_tree_crowns % MAX_TREE_CROWNS_PER_FILE == 0) {
			SHPClose(HSHP_treeCrowns);
			DBFClose(HDBF_treeCrowns);
			if (!initializeNewTreeCrownShapeFile(tc_out_path, n_tree_crowns, nRecords, &HSHP_treeCrowns, &HDBF_treeCrowns))
				return 0;
		}

		//writeContour(HDBF_treetops, HSHP_treeCrowns, HDBF_treeCrowns, contour, record, n_tree_crowns-1);
		writeContour(HDBF_treetops, HSHP_treeCrowns, HDBF_treeCrowns, contour, treeTops[record].index, n_tree_crowns-1);
		
		// Clear values to 0 so that we don't have overlapping tree crowns
		for(i = 0; i < wsize; i++)
		for(j = 0; j < wsize; j++)
			if(tc_mask[i][j] > 0)
				buff[tree_y-radius+i][tree_x-radius+j] = 0;
	}
	
	free(treeTops);
	freef2d(tc_wnd, max_pix_wnd);
	freec2d(tc_mask, max_pix_wnd);
	freec2d(visited, max_pix_wnd);
	freef2d(buff, length);
	
	return 1;
}

