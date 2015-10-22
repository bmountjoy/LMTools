

#include "SHPMerge.h"

int merge(char * from, char * to)
{	
	printf("merge\n\tfrom:%s\n\tto:%s\n", from, to);
	
	SHPHandle 
	from_SHP = SHPOpen(from, "rb+");
		if(!from_SHP) return 0;
		
	DBFHandle
	from_DBF = DBFOpen(from, "rb+");
		if(!from_DBF) return 0;
	
	SHPHandle
	to_SHP = SHPOpen(to, "rb+");
		if(!to_SHP) return 0;
		
	DBFHandle
	to_DBF = DBFOpen(to, "rb+");
		if(!to_DBF) return 0;
	
	int i, n_shapes, to_type, from_type;
	
	SHPGetInfo(to_SHP, NULL, &to_type, NULL, NULL);
	SHPGetInfo(from_SHP, &n_shapes, &from_type, NULL, NULL);
	
	if(to_type != from_type){
		PyErr_SetString(PyExc_IOError, "Cannot merge shape files with conflicting shape types.");
		return 0;
	}
	
	int to_ifield = DBFGetFieldIndex(to_DBF, "Flightline");
	int from_ifield = DBFGetFieldIndex(from_DBF, "Flightline");
	//
	// Copy shapes
	//
	for( i = 0; i < n_shapes; i++ )
	{
		SHPObject *
		p_shpObj = SHPReadObject(from_SHP, i);
			if(!p_shpObj)return 0;
			
		SHPWriteObject(to_SHP, -1, p_shpObj);
		SHPDestroyObject(p_shpObj);
		
		int n_records;
		SHPGetInfo(to_SHP, &n_records, NULL, NULL, NULL);
		
		//add dbf field
		const char * 
		dbf_val = DBFReadStringAttribute(from_DBF, i, from_ifield);
		DBFWriteStringAttribute(to_DBF, n_records-1, to_ifield, dbf_val);
	}
	
	SHPClose(from_SHP);
	DBFClose(from_DBF);
	SHPClose(to_SHP);
	DBFClose(to_DBF);
	
	return 1;
}

