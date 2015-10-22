from distutils.core import setup, Extension

module1 = Extension('TM_module',
	libraries = ['tiff', 'geotiff'],
	sources = ['module1/TM_module.c', 'module1/glcm.c', 'module1/TM_D8.c', 'module1/TM_Dinf.c', 'module1/TM_Form.c', 'module1/TM_Wetness.c', 'module1/TM_TiffIO.c', 'module1/TM_Math.c', 'module1/TM_memory.c'])

module2 = Extension('Bio_module',
	libraries = ['tiff', 'geotiff', 'laslib'],
	include_dirs = ['src', 'include/laslib/inc'],
	sources = ['module2/Bio_module.cpp', 'src/LasIO.cpp', 'module2/Bio_renameBinary.c', 'src/memory.c', 'src/TerraScanBin.c', 'module2/Bio_biometrics.c', 'module2/Bio_LMoments.c', 'module2/Bio_IO.c', 'module2/Memory.c', 'module2/Bio_rename.c', 'module2/Bio_centroid.c', 'module2/list.c', 'module2/Bio_square.c'])

module3 = Extension('TCR_module',
	libraries = ['shapelib', 'tiff', 'geotiff'],
	sources = ['TCR_module/TCR_module.c', 'TCR_module/io.c', 'TCR_module/memory.c', 'TCR_module/quantize.c', 'TCR_module/diff_bio.c'])

module4 = Extension('TCD_module',
	extra_compile_args = ['-Wall'],
	libraries =['shapelib', 'tiff', 'geotiff'],
	sources =['src/TCD_module.c', 'src/tiffio.c', 'src/TreeTops.c', 'src/boxMuller.c', 'src/memory.c', 'src/partitionTiff.c', 'src/TreeCrownDelineation.c', 'src/moore_neighbour_tracing.c', 'src/treetop_metrics.c'])

module5 = Extension('FLD_module',
	extra_compile_args = ['-Wall'],
	libraries =['shapelib'],
	sources =['src/FLD_module.c', 'src/ENVIStandardIO.c', 'src/SHPMerge.c'])

module6 = Extension('TCM_module',
	extra_compile_args = ['-Wall'],
	include_dirs = ['include/laslib/inc'],
	libraries =['laslib', 'tiff', 'geotiff'],
	sources =['src/memory.c', 'src/List.c', 'src/TCM_module.cpp', 'src/PointInPolygon.c', 'src/Biometrics.c', 'src/Quantize.c', 'src/tiffio.c'])

module7 = Extension('CR_module',
	extra_compile_args = ['-Wall'],
	include_dirs = [],
	libraries =[],
	sources =['src/memory.c', 'src/continuum_removal.c', 'src/ENVIStandardIO.c', 'src/CR_module.c', 'src/cr_dynamic_range.c', 'src/derivative_metrics.c'])

module8 = Extension('CSP_module',
	extra_compile_args = ['-Wall'],
	include_dirs = [],
	libraries =[],
	sources =['src/CSP_module.cpp', 'src/memory.c', 'src/csv_io.cpp'])

module9 = Extension('SM_module',
	extra_compile_args = ['-Wall'],
	include_dirs = [],
	libraries =[],
	sources =['src/SM_module.cpp', 'src/memory.c', 'src/asd_io.c', 'src/ENVIStandardIO.c', 'src/csv_io.cpp', 'src/sam.cpp', 'src/pearson_correlation.cpp'])

setup (name = 'PackageName',
       version = '1.1',
       description = 'Terrain modelling software.',
       ext_modules = [module1, module2, module3, module4, module5, module6, module7, module8])#, module9])
