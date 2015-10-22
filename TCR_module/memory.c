

#include "TCR_module.h"

char * allocc1d(int n)
{
	char * buff;
	
	if((buff = (char *) malloc(sizeof(char) * n)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	return buff;
}

int * alloci1d(int n)
{
	int * buff;
	
	if((buff = (int *) malloc(sizeof(int) * n)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	return buff;
}

float * allocf1d(int n)
{
	float * buff;
	
	if((buff = (float *) malloc(sizeof(float) * n)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	return buff;
}

int ** alloci2d(int nRows, int nCols)
{
	int ** buff;
	
	if((buff = (int **)malloc(sizeof(int *) * nRows)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	int i;
	for(i = 0; i < nRows; i++)
	{
		if((buff[i] = (int *)malloc(sizeof(int) * nCols)) == NULL)
		{
			PyErr_SetString(PyExc_MemoryError, "malloc failed.");
			return NULL;
		}
	}

	return buff;
}

double ** allocd2d(int nRows, int nCols)
{
	double ** buff;
	
	if((buff = (double **)malloc(sizeof(double *) * nRows)) == NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	int i;
	for(i = 0; i < nRows; i++)
	{
		if((buff[i] = (double *)malloc(sizeof(double) * nCols)) == NULL)
		{
			PyErr_SetString(PyExc_MemoryError, "malloc failed.");
			return NULL;
		}
	}

	return buff;
}


float *** allocf3d(int x, int y, int z)
{
	float *** buff;
	
	if((buff = (float ***) malloc(sizeof(float **) * x))==NULL)
	{
		PyErr_SetString(PyExc_MemoryError, "malloc failed.");
		return NULL;
	}
	
	int i,j;
	for(i = 0; i < x; i++)
	{
		if((buff[i] = (float **) malloc(sizeof(float*) * y)) == NULL)
		{
			PyErr_SetString(PyExc_MemoryError, "malloc failed.");
			return NULL;
		}
		
		for(j = 0; j < y; j++)
		{
			if((buff[i][j] = (float *) malloc(sizeof(float) * z)) == NULL)
			{
				PyErr_SetString(PyExc_MemoryError, "malloc failed.");
				return NULL;	
			}
		}
	}
	return buff;
}

void freei2d(int ** buffer, int numRows)
{
	int row;
	for(row = 0; row < numRows; row++)
	{
		free(buffer[row]);
	}
	free(buffer);
}

void freed2d(double ** buffer, int numRows)
{
	int row;
	for(row = 0; row < numRows; row++)
	{
		free(buffer[row]);
	}
	free(buffer);
}


void freef3d(float *** buffer, int x, int y)
{
	int i, j;
	for(i = 0; i < x; i++)
	{
		for(j = 0; j < y; j++)
		{
			free(buffer[i][j]);
		}
		free(buffer[i]);
	}
	free(buffer);
}












