#ifndef MEMORY_H
#define MEMORY_H

void freec2d(char ** buff, int d1);
void freeus2d(unsigned short ** buff, int d1);
void freeus3d(unsigned short *** buff, int d1, int d2);
void freei2d(int ** buff, int d1);
void freef2d(float ** buff, int d1);
void freef3d(float *** buff, int d1, int d2);
char * allocc1d(int d1);
int * alloci1d(int d1);
float *  allocf1d(int d1);
double * allocd1d(int d1);
unsigned short ** allocH2d(int d1, int d2);
int ** alloci2d(int d1, int d2);
float ** allocf2d(int d1, int d2);
double ** allocd2d(int d1, int d2);
void ** allocv2d(int d1, int d2, int size);
unsigned short *** allocus3d(int d1, int d2, int d3);
float *** allocf3d(int d1, int d2, int d3);


#endif
