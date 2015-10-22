
#include <Python/Python.h>

#include <stdlib.h>
#include "memory.h"


void freec2d(char ** buff, int d1)
{
	int i;
	for(i = 0; i < d1; i++)
	{
		free(buff[i]);
	}
	free(buff);
}

void freeus2d(unsigned short ** buff, int d1)
{
	int i;
	for(i = 0; i < d1; i++)
	{
		free(buff[i]);
	}
	free(buff);
}

void freeus3d(unsigned short *** buff, int d1, int d2)
{
	int i, j;
	for(i = 0; i < d1; i++)
	{
		for(j = 0; j < d2; j++)
		{
			free(buff[i][j]);
		}
		free(buff[i]);
	}
	
	free(buff);
}

void freei2d(int **buff, int d1)
{
	int i;
	for(i = 0; i < d1; i++)
	{
		free(buff[i]);
	}
	free(buff);
}

void freef2d(float **buff, int d1)
{
	int i;
	for(i = 0; i < d1; i++)
	{
		free(buff[i]);
	}
	free(buff);
}

void freef3d(float ***buff, int d1, int d2)
{	
	int i, j;
	for(i = 0; i < d1; i++)
	{
		for(j = 0; j < d2; j++)
		{
			free(buff[i][j]);
		}
		free(buff[i]);
	}
	
	free(buff);
}

char * allocc1d(int d1)
{
	char * buff;
	
	buff = (char*)malloc(sizeof(char) * d1);
	if(!buff){
		//PyErr_SetString(PyExc_MemoryError, "malloc failed");	
		return NULL;
	}
	
	return buff;
}

int * alloci1d(int d1)
{
	int * buff;
	
	buff = (int*)malloc(sizeof(int) * d1);
	if(!buff){
		//PyErr_SetString(PyExc_MemoryError, "malloc failed");	
		return NULL;
	}
	
	return buff;
}

float * allocf1d(int d1)
{
	float * buff;
	
	buff = (float*)malloc(sizeof(float) * d1);
	if(!buff){
		//PyErr_SetString(PyExc_MemoryError, "malloc failed");	
		return NULL;
	}
	
	return buff;
}


unsigned short ** allocH2d(int d1, int d2)
{
	unsigned short **buff;
	
	buff = (unsigned short**)malloc(sizeof(unsigned short*) * d1);
	if(!buff){
	//	PyErr_SetString(PyExc_MemoryError, "malloc failed");	
		return NULL;
	}
	
	int i;
	for(i = 0; i < d1; i++)
	{
		buff[i] = (unsigned short*)malloc(sizeof(unsigned short) * d2);
		if(!buff[i]){
			//PyErr_SetString(PyExc_MemoryError, "malloc failed");
			return NULL;
		}
	}
	
	return buff;
}

int ** alloci2d(int d1, int d2)
{
	int **buff;
	
	buff = (int**)malloc(sizeof(int*) * d1);
	if(!buff){
		//PyErr_SetString(PyExc_MemoryError, "malloc failed");	
		return NULL;
	}
	
	int i;
	for(i = 0; i < d1; i++)
	{
		buff[i] = (int*)malloc(sizeof(int) * d2);
		if(!buff[i]){
			//PyErr_SetString(PyExc_MemoryError, "malloc failed");
			return NULL;
		}
	}
	
	return buff;
}

float ** allocf2d(int d1, int d2)
{
	float **buff;
	
	buff = (float**)malloc(sizeof(float*) * d1);
	if(!buff){
		//PyErr_SetString(PyExc_MemoryError, "malloc failed");	
		return NULL;
	}
	
	int i;
	for(i = 0; i < d1; i++)
	{
		buff[i] = (float*)malloc(sizeof(float) * d2);
		if(!buff[i]){
			//PyErr_SetString(PyExc_MemoryError, "malloc failed");
			return NULL;
		}
	}
	
	return buff;
}

double ** allocd2d(int d1, int d2)
{
	double **buff;
	
	buff = (double**)malloc(sizeof(double*) * d1);
	if(!buff){
		//PyErr_SetString(PyExc_MemoryError, "malloc failed");	
		return NULL;
	}
	
	int i;
	for(i = 0; i < d1; i++)
	{
		buff[i] = (double*)malloc(sizeof(double) * d2);
		if(!buff[i]){
			//PyErr_SetString(PyExc_MemoryError, "malloc failed");
			return NULL;
		}
	}
	
	return buff;
}

void ** allocv2d(int d1, int d2, int size)
{
	void **buff;
	
	buff = (void**)malloc(sizeof(void*) * d1);
	if(!buff){
		//PyErr_SetString(PyExc_MemoryError, "malloc failed");	
		return NULL;
	}
	
	int i;
	for(i = 0; i < d1; i++)
	{
		buff[i] = (void*)malloc(size * d2);
		if(!buff[i]){
			//PyErr_SetString(PyExc_MemoryError, "malloc failed");
			return NULL;
		}
	}
	
	return buff;
}


double * allocd1d(int d1)
{
	double * buff;
	
	buff = (double*)malloc(sizeof(double) * d1);
	if(!buff){
		//PyErr_SetString(PyExc_MemoryError, "malloc failed");	
		return NULL;
	}
	
	return buff;
}


unsigned short *** allocus3d(int d1, int d2, int d3)
{
	unsigned short ***buff;
	
	buff = (unsigned short ***)malloc(sizeof(unsigned short**) * d1);
	
	int i;
	for(i = 0; i < d1; i++)
	{
		buff[i] = (unsigned short **)malloc(sizeof(unsigned short*) * d2);
		if(!buff[i]) 
			return NULL;
		
		int j;
		for(j = 0; j < d2; j++)
		{
			buff[i][j] = (unsigned short *)malloc(sizeof(unsigned short) * d3);
			if(!buff[i][j])
				return NULL;
		}
	}
	
	return buff;
}

float *** allocf3d(int d1, int d2, int d3)
{
	float ***buff;
	
	buff = (float ***)malloc(sizeof(float**) * d1);
	
	int i;
	for(i = 0; i < d1; i++)
	{
		buff[i] = (float **)malloc(sizeof(float*) * d2);
		if(!buff[i]) 
			return NULL;
		
		int j;
		for(j = 0; j < d2; j++)
		{
			buff[i][j] = (float *)malloc(sizeof(float) * d3);
			if(!buff[i][j])
				return NULL;
		}
	}
	
	return buff;
}
