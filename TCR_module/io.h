#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>

double ** buffFile (FILE * fp, int * nPtsPtr);
int writeFile (FILE * fp, double ** A, int nCols);
int writeToFile(FILE * fp, char * pc, double ** P, int * pNPoints, float hThreshold);

#endif
