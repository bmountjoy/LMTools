

#ifndef CSV_IO_HPP
#define CSV_IO_HPP

#include <vector>
#include <string>

#ifdef __cplusplus
extern "C"{
#endif

#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
}
#endif

using namespace std;

const char * buffer_csv(char * path, vector< vector<string> >& data);


#endif
