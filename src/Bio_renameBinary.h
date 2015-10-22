#ifndef BIO_RENAME_BINARY_H
#define BIO_RENAME_BINARY_H

char * renameBinFile(char * file_path, char * out_dir, double min_easting, 
	double min_northing, int blk_size,float z_threshold, int outText, int outBin);

#endif
