#include "TM_module.h" 
#include <math.h>

extern int NUMBER_OF_ROWS;
extern int NUMBER_OF_COLS;

void wetnessIndex(float ** upslopeArea, float ** slopeGradient, float ** out)
{
	int row, col;
	
	for(row = 0; row < NUMBER_OF_ROWS; row++){
		for(col =0; col < NUMBER_OF_COLS; col++)
		{
			//will be inifinity if the slope gradient is 0 -- just set to max?
			out[row][col] = log ( upslopeArea[row][col] / tan(slopeGradient[row][col]) );
		}
	}
}
