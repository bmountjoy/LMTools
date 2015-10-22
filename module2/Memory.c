#include <Python/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include "Memory.h"

float * initFloatBuffer1D(int size, float initVal)
{
	float * buffer;
	
	if((buffer = malloc(sizeof(float) * size)) == NULL)
	{
		return NULL;	
	}
	
	int i;
	for(i = 0; i < size; i++)
	{
		buffer[i] = initVal;
	}
	
	return buffer;
}

char ** init_char_Buffer2D(int numRows, int numCols)
{
	char ** buffer;
	
	if( (buffer = (char **) malloc( sizeof(char *) * numRows )) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	int row;
	
	for(row = 0; row < numRows; row++)
	{
		if( (buffer[row] = (char *) malloc(sizeof(char) * numCols)) == NULL)
		{
			PyErr_SetString(PyExc_MemoryError, "malloc failed.");
			return NULL;
		}
	}
	
	return buffer;
}

double ** initDoubleBuffer2D(int numRows, int numCols, double initVal)
{
	double ** buffer;
	
	if( (buffer = (double **) malloc( sizeof(double *) * numRows)) == NULL )
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	int row, col;
	for(row = 0; row < numRows; row++)
	{
		if( (buffer[row] = (double *) malloc( sizeof(double) * numCols)) == NULL)
		{
			//freeBuffer2D(buffer, row);
			PyErr_SetString(PyExc_MemoryError, "malloc failed.");
			return NULL;
		}
		
		for(col = 0; col < numCols; col++)
		{
			buffer[row][col] = initVal;
		}
	}
	
	return buffer;
}

float ** initFloatBuffer2D(int numRows, int numCols, float initVal)
{
	float ** buffer;
	
	if( (buffer = (float **) malloc( sizeof(float *) * numRows)) == NULL )
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	int row, col;
	for(row = 0; row < numRows; row++)
	{
		if( (buffer[row] = (float *) malloc( sizeof(float) * numCols)) == NULL)
		{
			//freeBuffer2D(buffer, row);
			PyErr_SetString(PyExc_MemoryError, "malloc failed.");
			return NULL;
		}
		
		for(col = 0; col < numCols; col++)
		{
			buffer[row][col] = initVal;
		}
	}
	
	return buffer;
}

int ** initIntBuffer2D(int numRows, int numCols, int initVal)
{
	int ** buffer;
	
	if( (buffer = (int **) malloc( sizeof(int *) * numRows)) == NULL )
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	int row, col;
	for(row = 0; row < numRows; row++)
	{
		if( (buffer[row] = (int *) malloc( sizeof(int) * numCols)) == NULL)
		{
			//freeBuffer2D(buffer, row);
			PyErr_SetString(PyExc_MemoryError, "malloc failed.");
			return NULL;
		}
		
		for(col = 0; col < numCols; col++)
		{
			buffer[row][col] = initVal;
		}
	}
	
	return buffer;
}

/**
 * Initializes a 3D float array.
 */
float *** initFloatBuffer3D(int numLevels, int numRows, int numCols, float initVal)
{
	float *** buffer;
	
	if( (buffer = (float ***) malloc( sizeof(float **) * numLevels)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	int level;
	for(level = 0; level < numLevels; level++)
	{
		if( (buffer[level] = initFloatBuffer2D(numRows, numCols, initVal)) == NULL)
		{
			return NULL;
		}
	}
	
	return buffer;
}

/**
 * Free 2D dynamically allocated float array 'buffer'.
 */

void free_char_Buffer2D(char ** buffer, int numRows)
{
	int row;
	for(row = 0; row < numRows; row++)
	{
		free(buffer[row]);
	}
	free(buffer);
}

void freeIntBuffer2D(int ** buffer, int numRows)
{
	int row;
	for(row = 0; row < numRows; row++)
	{
		free(buffer[row]);
	}
	free(buffer);
} 

void freeFloatBuffer2D(float ** buffer, int numRows)
{
	int row;
	for(row = 0; row < numRows; row++)
	{
		free(buffer[row]);
	}
	free(buffer);
} 

void freeDoubleBuffer2D(double ** buffer, int numRows)
{
	int row;
	for(row = 0; row < numRows; row++)
	{
		free(buffer[row]);
	}
	free(buffer);
}

/**
 * Free dynamically allocated 3D float array 'buffer'.
 */
 
void freeFloatBuffer3D(float *** buffer, int numLevels, int numRows)
{
	int level, row;
	
	for(level = 0; level < numLevels; level++)
	{
		for(row = 0; row < numRows; row++)
		{
			free(buffer[level][row]);
		}
		free(buffer[level]);
	}
	free(buffer);
}
 
void freeDoubleBuffer3D(double *** buffer, int numLevels, int numRows)
{
	int level, row;
	
	for(level = 0; level < numLevels; level++)
	{
		for(row = 0; row < numRows; row++)
		{
			free(buffer[level][row]);
		}
		free(buffer[level]);
	}
	free(buffer);
}
