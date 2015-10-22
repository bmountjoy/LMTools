#include "TM_module.h"
#include <math.h>
#include <stdlib.h>

#define RAD2DEG 57.29578

#define FLAT 	0
#define PIT 	1
#define CHANNEL 2
#define PASS 	3
#define RIDGE 	4
#define PEAK 	5
#define RED		0
#define GREEN	1
#define BLUE	2

extern int IMAGE_WIDTH;
extern int IMAGE_LENGTH;
extern int NUMBER_OF_ROWS;
extern int NUMBER_OF_COLS;

int classify(float ** rast)
{
	float a, b, c, d, e, f, g, * coeffs,
			z1, z2, z3, z4, z5, z6, z7, z8, z9,
			gSqrd,
			minic, maxic, slope, crosc,
			slope_tol, curve_tol;
	
	int row, col, type;
	                                                                           
	type = 0;
	
	//float out [3][NUMBER_OF_ROWS][NUMBER_OF_COLS];
	char out [3][NUMBER_OF_ROWS][NUMBER_OF_COLS]; // 3 samples per pixel - 8 bits per sample - rgba
	
	char colors [6][3] = { // rgb colors corresponding to landform classifications
		{160,160,160}, {0,0,0}, {102,178,255}, {76,153,0}, {255,255,102}, {255,102,102}
	};
	
	g     		= 1.0; //grid spacing -- assumed to be 1
	gSqrd 		= g*g;
	slope_tol 	= 1.0;
	curve_tol 	= 0.0001;
	
	for(row = 1; row < NUMBER_OF_ROWS-1; row++){
		for(col = 1; col < NUMBER_OF_COLS-1; col++)
		{
			z1 = rast[row-1][col-1];
			z2 = rast[row-1][col  ];
			z3 = rast[row-1][col+1];
			z4 = rast[row  ][col-1];
			z5 = rast[row  ][col  ];
			z6 = rast[row  ][col+1];
			z7 = rast[row+1][col-1];
			z8 = rast[row+1][col  ];
			z9 = rast[row+1][col+1];
			
			a = (z1 +z3 +z4 +z6 +z7 +z9) / (6.0*gSqrd) - (z2 +z5 +z8) / (3.0*gSqrd);
			b = (z1 +z2 +z3 +z7 +z8 +z9) / (6.0*gSqrd) - (z4 +z5 +z6) / (3.0*gSqrd);
			c = (z3 +z7 -z1 -z9) / (4.0*gSqrd); 
			d = (z3 +z6 +z9 -z1 -z4 -z7) / (6.0*g);
			e = (z1 +z2 +z3 -z7 -z8 -z9) / (6.0*g);
			f = (2.0*(z2 +z4 +z6 +z8)-(z1 +z3 +z7 +z9) + 5.0*z5) / 9.0;
			
			minic = (-a - b - sqrt((a - b) * (a - b) + c * c));
			maxic = (-a - b + sqrt((a - b) * (a - b) + c * c));
			slope = RAD2DEG * atan(sqrt((d * d) + (e * e)));
			crosc = -2.0 * (b * d * d + a * e * e - c * d * e) / (d * d + e * e);
			
			/* Case 1: Surface is sloping. Cannot be a peak,pass or pit. Therefore
			   calculate the cross-sectional curvature to characterise as
		 	   channel, ridge or planar.                                   */
		 	   
		 	type = FLAT;
	
			if (slope > slope_tol) 
			{
				if (crosc > curve_tol) {
					type = RIDGE;
				}
				else if (crosc < -curve_tol) {
					type = CHANNEL;
				}
				else {
					type = FLAT;
				}
			}
			else {
		
		
			/* Case 2: Surface has (approximately) vertical slope normal. Feature
			   can be of any type.                                        */
			
				if (maxic > curve_tol) 
				{
					if (minic > curve_tol) {
						type = PEAK;
					}
					else if (minic < -curve_tol) {
						type = PASS;
					}
					else {
						type = RIDGE;
					}
				}
				else if (maxic < -curve_tol) {
					if (minic < -curve_tol) {
						type = PIT;
					}
				}
				else {
					if (minic < -curve_tol) {
						type = CHANNEL;
					}
					else if (minic > curve_tol && minic < -curve_tol) {
						type = FLAT;
					}
				}
			}
			
			out[RED  ][row][col] = colors[type][RED  ];
			out[GREEN][row][col] = colors[type][GREEN];
			out[BLUE ][row][col] = colors[type][BLUE ];
			
		}//end for
		
	}//end for
	
	//write tiff out to file
	TIFF * fdout;
	
	if((fdout = TIFFOpen("/Users/Ben/Desktop/out/TM_classified.tif", "w")) == NULL)
	{
		PyErr_SetString(PyExc_IOError, "Could not open output file.");
		return 0;
	}
	
	TIFFSetField(fdout, TIFFTAG_IMAGEWIDTH, 		IMAGE_WIDTH);
	TIFFSetField(fdout, TIFFTAG_IMAGELENGTH, 		IMAGE_LENGTH);
	TIFFSetField(fdout, TIFFTAG_BITSPERSAMPLE, 		32);
	TIFFSetField(fdout, TIFFTAG_COMPRESSION, 		COMPRESSION_NONE);
	TIFFSetField(fdout, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_RGB);
	//TIFFSetField(fdout, TIFFTAG_PHOTOMETRIC, 		PHOTOMETRIC_MINISBLACK);
	TIFFSetField(fdout, TIFFTAG_RESOLUTIONUNIT, 	RESUNIT_NONE);
	TIFFSetField(fdout, TIFFTAG_DATATYPE, 			3);
	TIFFSetField(fdout, TIFFTAG_ORIENTATION, 		ORIENTATION_TOPLEFT);
	TIFFSetField(fdout, TIFFTAG_PLANARCONFIG, 		PLANARCONFIG_SEPARATE);
	TIFFSetField(fdout, TIFFTAG_SAMPLESPERPIXEL, 	3);
	
	tsample_t band;
	for(band = 0; band < 3; band++){
		for(row = 0; row < NUMBER_OF_ROWS; row++)
		{
			if(TIFFWriteScanline(fdout, out[band][row], row, band) != 1)
			{
				PyErr_SetString(PyExc_IOError, "Could not write scanline to file.");
				return 0;
			}
		}
	}
	TIFFClose(fdout);
	
	return 1;
}//end function
