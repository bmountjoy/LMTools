

#include <stdlib.h>
#include <stdio.h>

#include "moore_neighbour_tracing.h"
#include "memory.h"

int	MAX_SIZE;

Point2D findStart(char ** mask, int length, int width)
{
	Point2D start;
	int i,j;
	
	for(i = 0; i < length; i++)
	for(j = 0; j < width; j++)
	{
		if(mask[i][j] > 0){
			start.y = i;
			start.x = j;
			return start;
		}
	}
	start.x = -1;
	start.y = -1;
	return start;
}

Point2D next (Point2D ref, Point2D prev)
{
	Point2D vec = {prev.x - ref.x, prev.y - ref.y};
	Point2D res;
	
	if(vec.x == 0 && vec.y == -1){
		res.x = ref.x + 1; 
		res.y = ref.y - 1;
		return res;
	}
	else if(vec.x == 1 && vec.y == -1){
		res.x = ref.x + 1; 
		res.y = ref.y;
		return res;
	}
	else if(vec.x == 1 && vec.y == 0){
		res.x = ref.x + 1; 
		res.y = ref.y + 1;
		return res;
	}
	else if(vec.x == 1 && vec.y == 1){
		res.x = ref.x; 
		res.y = ref.y + 1;
		return res;
	}
	else if(vec.x == 0 && vec.y == 1){
		res.x = ref.x - 1; 
		res.y = ref.y + 1;
		return res;
	}
	else if(vec.x == -1 && vec.y == 1){
		res.x = ref.x - 1; 
		res.y = ref.y;
		return res;
	}
	else if(vec.x == -1 && vec.y == 0){
		res.x = ref.x - 1; 
		res.y = ref.y - 1;
		return res;
	}
	else{ // vec.x == 1 && vec.y == -1
		res.x = ref.x; 
		res.y = ref.y - 1;
		return res;
	}
}


/**
 * Entry to the start pixel is either from below or from the left
 * depending on the position of surrounding crown pixels.
 */
Point2D determineStartEntryPixel(char ** mask, Point2D start)
{
	Point2D start_entry;
	
	if(mask[start.y+1][start.x] == 0 && mask[start.y+1][start.x-1] == 0)
	{
		start_entry.x = start.x;
		start_entry.y = start.y+1;
	}
	else{
		start_entry.x = start.x-1;
		start_entry.y = start.y;
	}
	return start_entry;
}

int traceOutline(char ** mask, Point2D prev_b_entry, Point2D prev_b, Point2D prev, 
	double ** p_x_coords, double ** p_y_coords, int n_verts)
{
	double * x_coords = *p_x_coords;
	double * y_coords = *p_y_coords;
	
	int x_offsets [8] = {-1, 0, 1, 1, 1, 0, -1, -1};
	int y_offsets [8] = {-1, -1, -1, 0, 1, 1, 1, 0};
	
	int i, prev_i, curr_i;
	Point2D vec;
	
	prev_i = -1;
	vec.x = prev_b_entry.x - prev_b.x;
	vec.y = prev_b_entry.y - prev_b.y;
	
	for(i = 0; i < 8; i++)
		if(vec.x == x_offsets[i] && vec.y == y_offsets[i]){
			prev_i = i;
			break;
		}
	if(prev_i < 0) return -1;
	
	curr_i = -1;
	vec.x = prev.x - prev_b.x;
	vec.y = prev.y - prev_b.y;
	
	for(i = 0; i < 8; i++)
		if(vec.x == x_offsets[i] && vec.y == y_offsets[i]){
			curr_i = i;
			break;
		}
	if(curr_i < 0) return -1;
	
	i = prev_i;
	//loop through the indices between
	while(i != (curr_i + 1) % 8)
	{
		int x_off = x_offsets[i];
		int y_off = y_offsets[i];
		
		if(x_off == -1 && y_off == 0){
			x_coords[n_verts] = (double)prev_b.x;
			y_coords[n_verts] = (double)prev_b.y;
			n_verts++;
		}
		else if(x_off ==  0 && y_off == -1){
			x_coords[n_verts] = (double)prev_b.x+1;
			y_coords[n_verts] = (double)prev_b.y;
			n_verts++;
		}
		else if(x_off == 1 && y_off == 0){
			x_coords[n_verts] = (double)prev_b.x+1;
			y_coords[n_verts] = (double)prev_b.y+1;
			n_verts++;
		}
		else if(x_off == 0 && y_off == 1){
			x_coords[n_verts] = (double)prev_b.x;
			y_coords[n_verts] = (double)prev_b.y+1;
			n_verts++;
		}
		
		//printf("n_verts = %d\n", n_verts);
		if(n_verts == MAX_SIZE)
		{
			
			double * nx = allocd1d(MAX_SIZE + MAX_SIZE);if(!nx) return -1;
			double * ny = allocd1d(MAX_SIZE + MAX_SIZE);if(!ny) return -1;
			int i;
			for(i = 0; i < MAX_SIZE; i++){
				nx[i] = x_coords[i];
				ny[i] = y_coords[i];
			}
			free(x_coords);free(y_coords);
			*p_x_coords = x_coords = nx;
			*p_y_coords = y_coords = ny;
			
			MAX_SIZE = MAX_SIZE + MAX_SIZE;
		}
			
		
		i = (i + 1) % 8;
	}
	
	return n_verts;
}

SHPObject * startIsolated(char ** mask, Point2D start, double * x_coords, double * y_coords,
	double ul_easting, double ul_northing, float x_scale, float y_scale)
{
	int i,j,count;
	
	count = 0;
	
	for(i = start.y -1; i <= start.y+1; i++)
	for(j = start.x -1; j <= start.x+1; j++)
	{
		if(i == start.y && j == start.x)
			continue;
		
		if(mask[i][j] == 0)
			count++;
	}
	
	if(count == 8)
	{
		x_coords[0] = ul_easting + start.x*x_scale;
		x_coords[1] = ul_easting + (start.x + 1)*x_scale;
		x_coords[2] = ul_easting + (start.x + 1)*x_scale;
		x_coords[3] = ul_easting + start.x*x_scale;
		x_coords[4] = x_coords[0];
		y_coords[0] = ul_northing - start.y*y_scale;
		y_coords[1] = ul_northing - start.y*y_scale;
		y_coords[2] = ul_northing - (start.y + 1)*y_scale;
		y_coords[3] = ul_northing - (start.y + 1)*y_scale;
		y_coords[4] = y_coords[0];
		
		SHPObject * 
		shape = SHPCreateSimpleObject(SHPT_POLYGON, 5, x_coords, y_coords, NULL);
		
		free(x_coords);
		free(y_coords);
		
		return shape;
	}
	else
		return NULL;
}

/**
 * It is assumed that 'mask' has been appropriately processed inorder to 
 * represent an acceptable tree crown.
 *
 * Edge of mask assumed to be all zeros!!!
 */
SHPObject * trace (char ** mask, int length, int width, double ul_easting, double ul_northing, float x_scale, float y_scale)
{	
	int i, n_verts = 0;
	double * x_coords, * y_coords;
	Point2D start, prev_b, prev_b_entry, curr_b, curr, prev, start_entry;
	SHPObject * shape;
	
	MAX_SIZE = 64;
	x_coords = allocd1d(MAX_SIZE); if(!x_coords) return NULL;
	y_coords = allocd1d(MAX_SIZE); if(!y_coords) return NULL;
	
	//find the start pixel
	start = findStart(mask, length, width);
	
	if(start.x < 1 || start.x >= width-1 || start.y < 1 || start.y >= length-1){
		printf("Out of bounds: %d %d \n", start.x, start.y);
		return NULL;
	}
	
	if(start.x == -1 || start.y == -1){
		PyErr_SetString(PyExc_Exception, "Failed to find start vertex.");
		return NULL;
	}
	
	//check if the start isolated
	shape = startIsolated(mask, start, x_coords, y_coords, ul_easting, ul_northing, x_scale, y_scale);
	if(shape){
		//return NULL;
		return shape;
	}
		
	
	//determine entry neighbour to start pixel
	prev = 
	start_entry =
	prev_b_entry = 
	determineStartEntryPixel(mask, start);
	
	prev_b = start;
	curr_b = start;
	curr = next(curr_b, prev);
	
	int counter = 0;
	
	while(!( (curr.x == start.x && curr.y == start.y) && (prev.x == start_entry.x && prev.y == start_entry.y) ))
	{
		if(counter++ > 200000){
			PyErr_SetString(PyExc_Exception, "Shape outline too large.");
			return NULL;
		}
		
		if(mask[curr.y][curr.x] > 0)
		{
			curr_b = curr;
			curr = prev;
			
			//determine the outline here
			n_verts = 
			traceOutline(mask, prev_b_entry, prev_b, prev, &x_coords, 
				&y_coords, n_verts);
			
			if(n_verts < 0){
				PyErr_SetString(PyExc_Exception, "Empty outline (n_verts = 0).");puts("n_verts=0");
				return NULL;
			}
			
			prev_b = curr_b;
			prev_b_entry = prev;
		}
		else
		{
			prev = curr;
			curr = next(curr_b,prev);
		}
	}
	
	n_verts = 
	traceOutline(mask, prev_b_entry, prev_b, prev, &x_coords, 
		&y_coords, n_verts);
		
	if(n_verts < 0){
		PyErr_SetString(PyExc_Exception, "Empty outline (n_verts = 0).");puts("n_verts=0");
		return NULL;
	}
	
	//complete loop
	x_coords[n_verts] = x_coords[0];
	y_coords[n_verts] = y_coords[0];
	n_verts++;
	
	//convert outline to spatial coordinates
	for(i = 0; i < n_verts; i++){
		x_coords[i] = ul_easting  + (x_coords[i] * x_scale);
		y_coords[i] = ul_northing - (y_coords[i] * y_scale);
	}
	
	//create SHPObject with outline and return it
	shape = SHPCreateSimpleObject(SHPT_POLYGON, n_verts, x_coords, y_coords, NULL);
	if(!shape){
		PyErr_SetString(PyExc_Exception, "SHPCreateSimpleObject failed.");
		return NULL;
	}
	
	if(x_coords != NULL)free(x_coords);
	if(y_coords != NULL)free(y_coords);
	
	
	return shape;
}
