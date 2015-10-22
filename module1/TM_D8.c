
#include "TM_module.h"

extern int IMAGE_WIDTH;
extern int IMAGE_LENGTH;
extern int NUMBER_OF_ROWS;
extern int NUMBER_OF_COLS;

/**
 * Returns the index of the maximum element in 'array'.
 */
int getMaxIndex(float array [], int size)
{
	int curIndex, maxIndex;
	
	float max;
	
	curIndex = 0;
	maxIndex = 0;
	max      = array[curIndex];
	
	for(curIndex = 1; curIndex < size; curIndex++)
	{
		if(array[curIndex] >= max)
		{
			maxIndex = curIndex;
			max      = array[curIndex];
		}
	}
	return maxIndex;
}

void D8(float ** elevMap, float ** outBuf)
{
	float middle;
	
	float slopes [9];
	
	int row, col, currRow, currCol, nextRow, nextCol, maxIndex;	
	
	for(row = 1; row < NUMBER_OF_ROWS-1; row++){printf("\trow: %d\n",row);
		for(col = 1; col < NUMBER_OF_COLS-1; col++)
		{
			//work around to improve speed
			if(elevMap[row][col] == 0.0)
				continue;
			//end of work around
			middle     = elevMap[row][col];
			slopes[0]  = (middle - elevMap[row-1][col-1])/1.414;
			slopes[1]  = (middle - elevMap[row  ][col-1]);
			slopes[2]  = (middle - elevMap[row+1][col-1])/1.414;
			slopes[3]  = (middle - elevMap[row+1][col  ]);
			slopes[4]  = (middle - elevMap[row+1][col+1])/1.414;
			slopes[5]  = (middle - elevMap[row  ][col+1]);
			slopes[6]  = (middle - elevMap[row-1][col+1])/1.414;
			slopes[7]  = (middle - elevMap[row-1][col  ]);          
			slopes[8]  = 0;
			
			maxIndex = getMaxIndex(slopes, 9); //if(maxIndex < 8)printf("%d\n", maxIndex);
			
			//flow must initially be diagonal
			if( (maxIndex == 1) || (maxIndex == 3) || (maxIndex == 5) || (maxIndex == 7) || (maxIndex == 8)){
				continue;
			}
			
			currRow = row;
			currCol = col;
			
			//Continue if current position does not represent a sink and follow the flow.
			while( 1 )
			{	
				if( (currRow >= NUMBER_OF_ROWS-1) || (currRow <= 0) || (currCol >= NUMBER_OF_COLS-1) || (currCol <= 0) ){
					break;
				}
				
				middle    = elevMap[currRow][currCol];
				slopes[0] = (middle - elevMap[currRow-1][currCol-1])/1.414;
				slopes[1] = (middle - elevMap[currRow  ][currCol-1]);
				slopes[2] = (middle - elevMap[currRow+1][currCol-1])/1.414;
				slopes[3] = (middle - elevMap[currRow+1][currCol  ]);
				slopes[4] = (middle - elevMap[currRow+1][currCol+1])/1.414;
				slopes[5] = (middle - elevMap[currRow  ][currCol+1]);
				slopes[6] = (middle - elevMap[currRow-1][currCol+1])/1.414;
				slopes[7] = (middle - elevMap[currRow-1][currCol  ]);
				slopes[8] = 0;
				
				maxIndex = getMaxIndex(slopes, 9); //if(maxIndex < 8)printf("%d\n", maxIndex);

				switch (maxIndex)
				{
					case 0:
						nextRow = currRow-1;
						nextCol = currCol-1;
						break;
					case 1:
						nextRow = currRow;
						nextCol = currCol-1;
						break;
					case 2:
						nextRow = currRow+1;
						nextCol = currCol-1;
						break;
					case 3:
						nextRow = currRow+1;
						nextCol = currCol;
						break;
					case 4:
						nextRow = currRow+1;
						nextCol = currCol+1;
						break;
					case 5:
						nextRow = currRow;
						nextCol = currCol+1;
						break;
					case 6:
						nextRow = currRow-1;
						nextCol = currCol+1;
						break;
					case 7:
						nextRow = currRow-1;
						nextCol = currCol;
						break;
					default:
						nextRow = currRow;
						nextCol = currCol;
				}
				
				if( ((nextRow != currRow) || (nextCol != currCol)))
				{	
					outBuf[nextRow][nextCol]++;
							
					currRow = nextRow;
					currCol = nextCol;
				}else{
					break;
				}
			}//end while
			
		}//end for
		
	}//end for
}//end calculateFlow
