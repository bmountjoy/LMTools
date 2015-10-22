





#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "csv_io.hpp"
	

using namespace std;


const char * buffer_csv(char * path, vector< vector<string> >& data)
{
	printf("buffer_csv:%s\n", path);
	
	ifstream infile (path);
	if(!infile.is_open()){
		return "Error opening csv file.";
	}
	
	while(infile)
	{
		string s;
		vector<string> row;
		
		if(!getline(infile, s))
			break;
		
		istringstream ss(s);
		
		while(ss)
		{
			string s;
			if(!getline(ss, s, ',')) 
				break;
			
			row.push_back(s);
		}
		
		data.push_back(row);
	}
	
	if(!infile.eof()){
		return "Error reading csv file.";
	}
	
	return NULL;
}
