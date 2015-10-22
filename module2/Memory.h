#ifndef BIO_MEMORY_H
#define BIO_MEMORY_H

float * 	initFloatBuffer1D(int size, float initVal);
char ** 	init_char_Buffer2D(int numRows, int numCols);
double ** 	initDoubleBuffer2D(int rows, int cols, double initVal);
float ** 	initFloatBuffer2D(int rows, int cols, float initVal);
float *** 	initFloatBuffer3D(int bands, int rows, int cols, float initVal);
int ** 		initIntBuffer2D(int numRows, int numCols, int initVal);
void 		freeIntBuffer2D(int ** buffer, int numRows);
void	 	freeFloatBuffer2D(float ** buffer, int numRows);
void 		freeDoubleBuffer2D(double **, int);
void 		freeDoubleBuffer3D(double ***, int, int);
void  		freeFloatBuffer3D(float ***, int, int);

#endif
