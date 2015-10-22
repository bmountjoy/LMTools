
#include "TM_module.h" 

extern int IMAGE_WIDTH;
extern int IMAGE_LENGTH;
extern int NUMBER_OF_ROWS;
extern int NUMBER_OF_COLS;
extern int GRID;

void slopeGradient(float ** elev, float ** out)
{
	float g, h, sgg, sgh;
	
	int row, col;
	
	for(row = 1; row < NUMBER_OF_ROWS-1; row++){
		for(col = 1; col < NUMBER_OF_COLS-1; col++)
		{	
			g = (elev[row+1][col  ] - elev[row-1][col  ]) / (2 * GRID);
			h = (elev[row  ][col-1] - elev[row  ][col+1]) / (2 * GRID);
			
			sgg = g*g;
			sgh = h*h;
			
			out[row+1][col] = pow((sgg+sgh), 0.5);
		}
	}
}

float modulus(float value, float divisor)
{
	while(value >= divisor)
		value -= divisor;
	
	return value;
}

void slopeAspect(float ** elev, float ** out, int linearize, int offset)
{	
	int row, col;
	
	for(row = 1; row < NUMBER_OF_ROWS-1; row++){
		for(col = 1; col < NUMBER_OF_COLS-1; col++)
		{
			float g, h, middle;	
			
			middle = elev[row][col];
			
			g = (elev[row+1][col  ] - elev[row-1][col  ]) / (2 * GRID);
			h = (elev[row  ][col-1] - elev[row  ][col+1]) / (2 * GRID);
			
			if( ((h == 0) && (g == 0)) )// || (g == 0) ) -- part of added code
			{
				out[row+1][col] = 361;
			}
			/* Start of add. Check with authority figures. */
			else if ( g == 0 ){
				if ( h > 0 ){
					out[row+1][col] = 180.0;
				}
				else {
					out[row+1][col] = 0.0;
				}
			}
			else if( h == 0 ){
				if( g > 0 ){
					out[row+1][col] = 270.0;
				}
				else { // ( g < 0 )
					out[row+1][col] = 90.0;
				}
			}
			/* End of add. */
			else
			{
				float temp1 = fabs(h/(double)g);
				float temp  = (atan(temp1))*57.958;
				if(temp < 0)
				{
					out[row+1][col] = 361;
				}
				else
				{
					if(h > 0 && g > 0)
					{
						out[row+1][col] = 270-(fabs(temp));
					}
					else if (h > 0 && g < 0)
					{
						out[row+1][col] = 90 +(fabs(temp));
					}
					else if(h < 0 && g < 0)
					{
						out[row+1][col] = 90 -(fabs(temp));
					}
					else if(h < 0 && g > 0)
					{
						out[row+1][col] = 270+(fabs(temp));
					}
				}
			}
			
			if(linearize)
			{
				float theta = out[row+1][col];
				
				theta = 360.0 - modulus(theta + 90.0 + offset, 360.0);
				
				if(theta > 180.0)
					theta = 360 - theta;
				
				out[row+1][col] = theta;
			}
		}	
	}
}

void downSlopeCurvature(float ** elev, float ** out)
{
	int row, col;
	
	for(row = 1; row < NUMBER_OF_ROWS-1; row++){
		for(col = 1; col < NUMBER_OF_COLS-1; col++)
		{
			float d, e, f, g, h, sgg, shh, middle, gSqrd;
			
			middle = elev[row][col];
			gSqrd  = GRID*GRID;
			
			d = (((elev[row-1][col  ] + elev[row+1][col  ])/2.0) - middle) / gSqrd;
			e = (((elev[row  ][col-1] + elev[row  ][col+1])/2.0) - middle) / gSqrd;
			f = (( elev[row-1][col+1] + elev[row+1][col-1]) - (elev[row-1][col-1] + elev[row+1][col+1])) / (4 * gSqrd);
			
			g = (elev[row+1][col  ] - elev[row-1][col  ]) / (2 * GRID);
			h = (elev[row  ][col-1] - elev[row  ][col+1]) / (2 * GRID);
			
			sgg = g*g;
			shh = h*h;
		
			if( (g == 0) && (h == 0))
			{
				out[row+1][col] = 0.00001;
			}
			else
			{
				out[row+1][col] = (-2 * ((d*(sgg)) + (e*(shh)) + (f*g*h))) / (sgg + shh);
			}
		}
	}
}

void acrossSlopeCurvature(float ** elev, float ** out)
{
	int row, col;
	
	for(row = 1; row < NUMBER_OF_ROWS-1; row++){
		for(col = 1; col < NUMBER_OF_COLS-1; col++)
		{
			float d, e, f, g, h, sgg, sgh, middle, gSqrd;
			
			middle = elev[row][col];
			gSqrd  = GRID*GRID;
			
			d = (((elev[row-1][col  ] + elev[row+1][col  ])/2.0) - middle) / gSqrd;
			e = (((elev[row  ][col-1] + elev[row  ][col+1])/2.0) - middle) / gSqrd;
			f = ((elev[row-1][col+1] + elev[row+1][col-1]) - (elev[row-1][col-1] + elev[row+1][col+1])) / (4 * gSqrd);
			
			g = (elev[row+1][col  ] - elev[row-1][col  ]) / (2 * GRID);
			h = (elev[row  ][col-1] - elev[row  ][col+1]) / (2 * GRID);
			
			sgg = g*g;
			sgh = h*h;
			
			if((g == 0) && (h == 0))
			{
				out[row+1][col] = 0.00001;
			}
			else
			{
				out[row+1][col] = (-2 * ((d*(sgh)) + (e*(sgg)) - (f*g*h))) / (sgg + sgh);
			}
		}	
	}
}
