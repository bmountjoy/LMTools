from distutils.core import setup, Extension

LAS_SRC_FILES = []

"""
LAS_SRC_FILES = ['external/laslib/LAStools/LASlib/src/fopen_compressed.cpp',\
'external/laslib/LAStools/LASlib/src/lasfilter.cpp',\
'external/laslib/LAStools/LASlib/src/lasreader.cpp',\
'external/laslib/LAStools/LASlib/src/lasreader_asc.cpp',\
'external/laslib/LAStools/LASlib/src/lasreader_bil.cpp',\
'external/laslib/LAStools/LASlib/src/lasreader_bin.cpp',\
'external/laslib/LAStools/LASlib/src/lasreader_dtm.cpp',\
'external/laslib/LAStools/LASlib/src/lasreader_las.cpp',\
'external/laslib/LAStools/LASlib/src/lasreader_qfit.cpp',\
'external/laslib/LAStools/LASlib/src/lasreader_shp.cpp',\
'external/laslib/LAStools/LASlib/src/lasreader_txt.cpp',\
'external/laslib/LAStools/LASlib/src/lasreaderbuffered.cpp',\
'external/laslib/LAStools/LASlib/src/lasreadermerged.cpp',\
'external/laslib/LAStools/LASlib/src/lasreaderpipeon.cpp',\
'external/laslib/LAStools/LASlib/src/lastransform.cpp',\
'external/laslib/LAStools/LASlib/src/lasutility.cpp',\
'external/laslib/LAStools/LASlib/src/laswaveform13reader.cpp',\
'external/laslib/LAStools/LASlib/src/laswaveform13writer.cpp',\
'external/laslib/LAStools/LASlib/src/laswriter.cpp',\
'external/laslib/LAStools/LASlib/src/laswriter_bin.cpp',\
'external/laslib/LAStools/LASlib/src/laswriter_las.cpp',\
'external/laslib/LAStools/LASlib/src/laswriter_qfit.cpp',\
'external/laslib/LAStools/LASlib/src/laswriter_txt.cpp',\
'external/laslib/LAStools/LASlib/src/laswriter_wrl.cpp',\
'external/laslib/LAStools/LASlib/src/laswritercompatible.cpp',\
'external/laslib/LAStools/LASzip/src/arithmeticdecoder.cpp',\
'external/laslib/LAStools/LASzip/src/arithmeticencoder.cpp',\
'external/laslib/LAStools/LASzip/src/arithmeticmodel.cpp',\
'external/laslib/LAStools/LASzip/src/integercompressor.cpp',\
'external/laslib/LAStools/LASzip/src/lasindex.cpp',\
'external/laslib/LAStools/LASzip/src/lasinterval.cpp',\
'external/laslib/LAStools/LASzip/src/lasquadtree.cpp',\
'external/laslib/LAStools/LASzip/src/lasreaditemcompressed_v1.cpp',\
'external/laslib/LAStools/LASzip/src/lasreaditemcompressed_v2.cpp',\
'external/laslib/LAStools/LASzip/src/lasreadpoint.cpp',\
'external/laslib/LAStools/LASzip/src/lasunzipper.cpp',\
'external/laslib/LAStools/LASzip/src/laswriteitemcompressed_v1.cpp',\
'external/laslib/LAStools/LASzip/src/laswriteitemcompressed_v2.cpp',\
'external/laslib/LAStools/LASzip/src/laswritepoint.cpp',\
'external/laslib/LAStools/LASzip/src/laszip.cpp',\
'external/laslib/LAStools/LASzip/src/laszip_dll.cpp',\
'external/laslib/LAStools/LASzip/src/laszipper.cpp']
"""

module1 = Extension('TM_module',
	library_dirs = [],
	libraries = ['tiff', 'geotiff'],
	sources = ['module1/TM_module.c', 'module1/glcm.c', 'module1/TM_D8.c', 'module1/TM_Dinf.c', 'module1/TM_Form.c', 'module1/TM_Wetness.c', 'module1/TM_TiffIO.c', 'module1/TM_Math.c', 'module1/TM_memory.c'])

module2 = Extension('Bio_module',
	extra_compile_args = ['-Wall'],
	libraries = ['tiff', 'geotiff'],
	# include_dirs = ['src', 'external/laslib/LAStools/LASlib/inc', 'external/laslib/LAStools/LASzip/src', 'external/laslib/LAStools/LASzip/stl'],
	include_dirs = ['src'],
	sources = LAS_SRC_FILES + ['module2/Bio_module.cpp', 'src/LasIO.cpp', 'module2/Bio_renameBinary.c', 'src/memory.c', 'src/TerraScanBin.c', 'module2/Bio_biometrics.c', 'module2/Bio_LMoments.c', 'module2/Bio_IO.c', 'module2/Memory.c', 'module2/Bio_rename.c', 'module2/Bio_centroid.c', 'module2/list.c', 'module2/Bio_square.c'])

module3 = Extension('TCR_module',
	libraries = ['shp', 'tiff', 'geotiff'],
	sources = ['TCR_module/TCR_module.c', 'TCR_module/io.c', 'TCR_module/memory.c', 'TCR_module/quantize.c', 'TCR_module/diff_bio.c'])

module4 = Extension('TCD_module',
	extra_compile_args = ['-Wall'],
	libraries =['shp', 'tiff', 'geotiff'],
	sources =['src/TCD_module.c', 'src/tiffio.c', 'src/TreeTops.c', 'src/boxMuller.c', 'src/memory.c', 'src/partitionTiff.c', 'src/TreeCrownDelineation.c', 'src/moore_neighbour_tracing.c', 'src/treetop_metrics.c'])

module5 = Extension('FLD_module',
	extra_compile_args = ['-Wall'],
	libraries =['shp'],
	sources =['src/FLD_module.c', 'src/ENVIStandardIO.c', 'src/SHPMerge.c'])

module6 = Extension('TCM_module',
	extra_compile_args = ['-Wall'],
	# include_dirs = ['external/laslib/LAStools/LASlib/inc', 'external/laslib/LAStools/LASzip/src', 'external/laslib/LAStools/LASzip/stl'],
	libraries =['tiff', 'geotiff'],
	sources = LAS_SRC_FILES + ['src/memory.c', 'src/List.c', 'src/TCM_module.cpp', 'src/PointInPolygon.c', 'src/Biometrics.c', 'src/Quantize.c', 'src/tiffio.c'])

module7 = Extension('CR_module',
	extra_compile_args = ['-Wall'],
	sources =['src/memory.c', 'src/continuum_removal.c', 'src/ENVIStandardIO.c', 'src/CR_module.c', 'src/cr_dynamic_range.c', 'src/derivative_metrics.c'])

module8 = Extension('CSP_module',
	extra_compile_args = ['-Wall'],
	sources =['src/CSP_module.cpp', 'src/memory.c', 'src/csv_io.cpp'])

module9 = Extension('SM_module',
	extra_compile_args = ['-Wall'],
	sources =['src/SM_module.cpp', 'src/memory.c', 'src/asd_io.c', 'src/ENVIStandardIO.c', 'src/csv_io.cpp', 'src/sam.cpp', 'src/pearson_correlation.cpp'])

setup (name = 'PackageName',
       version = '1.1',
       description = 'Terrain modelling software.',
       ext_modules = [module1, module2, module3, module4, module5, module6, module7, module8])#, module9])
