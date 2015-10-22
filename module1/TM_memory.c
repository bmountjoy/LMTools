#include "TM_module.h"

/**
 * Initializes a 2D float array.
 */
float ** initBuffer2D(int numRows, int numCols, float initVal)
{
	float ** buffer;
	
	if( (buffer = (float **) _TIFFmalloc( sizeof(float *) * numRows)) == NULL )
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return NULL;
	}
	
	int row, col;
	for(row = 0; row < numRows; row++)
	{
		if( (buffer[row] = (float *) _TIFFmalloc( sizeof(float) * numCols)) == NULL)
		{
			freeBuffer2D(buffer, row);
			PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
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
float *** initBuffer3D(int numLevels, int numRows, int numCols, float initVal)
{
	float *** buffer;
	
	if( (buffer = (float ***) _TIFFmalloc( sizeof(float **) * numLevels)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "_TIFFmalloc failed.");
		return NULL;
	}
	
	int level;
	for(level = 0; level < numLevels; level++)
	{
		if( (buffer[level] = initBuffer2D(numRows, numCols, initVal)) == NULL)
		{
			return NULL;
		}
	}
	return buffer;
}

/**
 * Free 2D dynamically allocated float array 'buffer'.
 */
void freeBuffer2D(float ** buffer, int numRows)
{
	int row;
	for(row = 0; row < numRows; row++)
	{
		_TIFFfree(buffer[row]);
	}
	_TIFFfree(buffer);
}

/**
 * Free dynamically allocated 3D float array 'buffer'.
 */
void freeBuffer3D(float *** buffer, int numLevels, int numRows)
{
	int level, row;
	
	for(level = 0; level < numLevels; level++)
	{
		for(row = 0; row < numRows; row++)
		{
			_TIFFfree(buffer[level][row]);
		}
		_TIFFfree(buffer[level]);
	}
	_TIFFfree(buffer);
}
