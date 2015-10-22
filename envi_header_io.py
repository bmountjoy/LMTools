

import sys

def read_lines(fp_hdr):
	
	L = []
	
	for line in fp_hdr:
		if line.strip() == "ENVI":
			continue
		L.append(line)
		
	return L;
	
	
	
###############################################################################
## Extracts the list of float values represnted by the string in the ENVI header.
##
## ex. 'fwhm = {1.0, 2.0, 3.0, 4.0, 5.0}' -> [1.0, 2.0, 3.0, 4.0, 5.0]
###############################################################################

def extract_float_list(string):
		
	split = string.strip('{} \n').split(',')
	fvals = []
	for s in split:
		fvals.append(float(s)) # whitespace automatically removed
	return fvals



###############################################################################
## Read in all the (key,value) pairs of the ENVI header file specified by
## 'path'.
###############################################################################
def read(path):
	
	key_val = {}

	try:
		fp_hdr = open(path, "r")
	except:
		print str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2])
		return
		
	fp_hdr.seek(0)
	lines = read_lines(fp_hdr);
	fp_hdr.close()
	
	i = 0
	n_lines = len(lines)
	
	print str(n_lines)

	while i < n_lines:
		
		line = lines[i]
		eqi  = line.find("=")
		key  = line[0:eqi].strip().lower()
		val  = line[eqi+1:].strip()
		
		i = i + 1
		
		if i >= n_lines:
			key_val[key] = val
			break
			
		while i < n_lines and lines[i].find("=") == -1:
			val = val + lines[i].strip()
			i = i + 1
		
		key_val[key] = val
		
	return key_val


###############################################################################
## Read in all the (key,value) pairs of the ENVI header file specified by
## 'path' keeping all additional whitespace.
###############################################################################
def read_as_is(path):
	
	keys = []
	key_val = {}

	fp_hdr = open(path, "r")
	lines = read_lines(fp_hdr);
	fp_hdr.close()
	
	i = 0
	n_lines = len(lines)

	while i < n_lines:
		
		line = lines[i]
		eqi  = line.find("=")
		key  = line[0:eqi].strip().lower()
		val  = line[eqi+1:]
		
		i = i + 1
		
		if i >= n_lines:
			keys.append(key)
			key_val[key] = val
			break
			
		while i < n_lines and lines[i].find("=") == -1:
			val = val + lines[i]
			i = i + 1
			
		keys.append(key)
		key_val[key] = val
		
	return (keys, key_val)
	
def write_as_is(path, keys, key_val):
	
	fp_hdr = open(path, "w")
	fp_hdr.write("ENVI\n")
	for key in keys:
		fp_hdr.write(key + " = " + key_val[key])
		
	fp_hdr.close()
		
		
		
		
		
		
