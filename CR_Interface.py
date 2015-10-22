

import Tkinter
import sys
import os
from os import path
import tkFileDialog
import CR_module
import envi_header_io

## this needs to commented out when deploying :(..
#import rpy2.robjects as R


##
## data is assumed to be in a csv file where the first row contains the wavelengths
## of each spectra and each subsequent line contains spectra
##
"""
class SpectraViewer:
	
	def quit(self):
		#R.r.quit()
		self.root.destroy()
		self.root.quit()
		
	def plot_next(self):
		if self.cur_spec < self.max_spec:
			self.cur_spec = self.cur_spec + 1
		self.plot_spectra()
	
	def plot_prev(self):
		if self.cur_spec > self.min_spec:
			self.cur_spec = self.cur_spec - 1
		self.plot_spectra()
		
	def plot_spectra(self):
		r_wave = R.FloatVector(self.wave)
		r_spec = R.FloatVector(self.spec[self.cur_spec])
		wdepth = self.metr[self.cur_spec][5]
		x_bdc  = R.FloatVector([wdepth, wdepth])

		#find index of depth
		i = 0;
		while(self.wave[i] < wdepth):
			i = i + 1
		depthi = i-1 if wdepth-self.wave[i-1] < self.wave[i]-wdepth else i
		depth  = self.spec[self.cur_spec][depthi]

		y_bdc  = R.FloatVector([depth,1.0])
		title  = self.file + ": Spectra " + str(self.cur_spec+1) + " of " + str(self.max_spec+1)
		R.r.plot(r_wave, r_spec, "l", xlab="Wavelengths", ylab="Continuum", main=title, axes = R.r.T)
		R.r.lines(x_bdc, y_bdc, col="red")

	def __init__(self, path, metr):
		
		print "In SpectraViewer"
		#
		# read in  output
		#
		self.file = path[path.rfind("/")+1:]
		self.spec = []
		self.metr = []
		
		try:
			#read in the metrics file
			fp_in = open(metr, "rb")
			first = 1
			for line in fp_in:
				if first == 1:
					self.name = [s for s in line.split(",")]
					first = 0
				else:
					self.metr.append([float(s) for s in line.split(",")])
			fp_in.close()
			
			#read in the continuum removal file
			fp_in = open(path, "rb")
			first = 1
			for line in fp_in:
				if first == 1:
					self.wave = [float(s) for s in line.split(",")[1:]]
					first = 0
				else:
					tmp = [float(s) for s in line.split(",")[1:]]
					self.spec.append(tmp)
			fp_in.close()
			
			self.cur_spec = 0
			self.min_spec = 0
			self.max_spec = len(self.spec)-1
			
			self.root = Tkinter.Tk()
			prev_button = Tkinter.Button(self.root, text="PREV", command = self.plot_prev)
			prev_button.grid(row = 0, column = 0)
			next_button = Tkinter.Button(self.root, text="NEXT", command = self.plot_next)
			next_button.grid(row = 0, column = 1)
			exit_button = Tkinter.Button(self.root, text="Exit", command = self.quit)
			exit_button.grid(row = 0, column = 2)
			self.plot_spectra()
			self.root.mainloop()
		except:
			print "Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2])
			return
"""

##
## data is assumed to be in a csv file where the first row contains the wavelengths
## of each spectra and each subsequent line contains spectra
##

class Init:
	
	def __init__(self, master, clipBoard):
		
		self.helpMessage = "CONTINUUM REMOVAL\n\n" + \
		"Input Directory: Directory of input spectra. The input spectra are assumed to the be the ouput of the Spectral Extraction from the TC Metrics tab.\n" +\
		"- spectral records run across each row\n" + \
		"- the first few columns of each record contain attributes of the spectra\n" + \
		"- the first row contains the attribute names followed by the wavelengths of each band in nanometers\n\n" + \
		"Columns to Skip\n" + \
		"- should be set to the number of attributes associated with each spectra\n" +\
		"- these attributes are ignored by the continuum removal process\n\n" + \
		"STATIC RANGES\n\n" + \
		"- wavelength ranges for which to perform the continuum removal\n" + \
		"- a seperate directory is created for each wavelength range within the select output directory\n\n" + \
		"Static Range Continuum Removal\n\n" + \
		"The user has the option to depth or area normalize the continuum removal curve\n\n" + \
		"Derivative Metrics\n\n" + \
		"Window\n" + \
		"- the size of the window used to calculate the derivative\n" + \
		"- ex: if window = 5 then deriv(x) = spectrum(x+2)-spectrum(x-2) / wavelength(x+2) - wavelength(x-2)\n\n" + \
		"Threshold\n" + \
		"- used to control which inflection points are reported\n" + \
		"- only inflection points with corresponding derivative values above this threshold are reported"
		
		
		self.wave_thresh = 5.0
		self.n_features = 9
		self.feature_names = [\
			"slope", \
			"y intercept", \
			"bd center", \
			"symmetry",\
			"depth",\
			"wavelength at depth", \
			"area",\
			"area left",\
			"area right"]
	
		frame = Tkinter.Frame(master)
		frame.grid(row = 0, column = 0, padx = 10)
		
		###################### Pad Frame ##################################
		
		padframe1 = Tkinter.Frame(frame)
		padframe1.grid(row = 0, column = 0, pady = 0)
		
		###################################################################
		###################### Input Frame ################################
		###################################################################
		
		input_frame = Tkinter.Frame(frame)
		input_frame.grid(row = 1, column = 0, sticky = Tkinter.NW)
		
		#grid row
		r = 0
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 16)
		
		r = r + 1
		
		button = Tkinter.Button(input_frame, text="Input Directory", command = self.getInputDirectory)
		button.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inputDirEntry = Tkinter.Entry(input_frame, width = 70)
		self.inputDirEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.inputDirEntry.insert(0,"/Users/ben/Desktop/test_cr/spectral_extraction")
		
		r = r + 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 5)
		
		r = r + 1
		
		##################################################################
		###################### static range
		##################################################################
		
		"""
		self.staticRangeIntVar = Tkinter.IntVar()
		self.staticRangeIntVar.set(1)
		cb = Tkinter.Checkbutton(input_frame, text="STATIC RANGE:", variable = self.staticRangeIntVar, command = self.staticRangeChecked)
		cb.grid(row = r, column = 0, sticky = Tkinter.W, padx = 0)
		"""
		
		
		Tkinter.Label(input_frame, text = "Columns to Skip").grid(row = r, column = 0, sticky = Tkinter.NW)
		self.col_skip_entry = Tkinter.Entry(input_frame, width = 5)
		self.col_skip_entry.grid(row = r, column = 1, sticky = Tkinter.NW)
		self.col_skip_entry.insert(0, "5")
		
		r += 1
		
		Tkinter.Label(input_frame, text = "Spectra Id Column").grid(row = r, column = 0, sticky = Tkinter.NW)
		self.col_id_entry = Tkinter.Entry(input_frame, width = 5)
		self.col_id_entry.grid(row = r, column = 1, sticky = Tkinter.NW)
		self.col_id_entry.insert(0, "1")
		
		r += 1
		
		"""
		self.dynamic_range_intvar = Tkinter.IntVar()
		Tkinter.Checkbutton(input_frame, text="Dynamic Ranges", variable = self.dynamic_range_intvar).grid(row=r, column = 0, sticky = Tkinter.NW)
		
		r += 1
		"""
		
		Tkinter.Label(input_frame, text = "RANGES").grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		self.max_ranges = 5
		self.static_ranges = []
		self.static_range_frame = Tkinter.Frame(input_frame)
		self.static_range_frame.grid(row = r, column = 1, sticky = Tkinter.W)
		
		for i in range(self.max_ranges):
			self.add_static_range(i)
		
		r = r + 1
		
		Tkinter.Label(input_frame, text = "Static Range Continuum Removal").grid(row = r, column = 0, sticky = Tkinter.W, columnspan = 2, pady = 10)
		
		r = r + 1
		
		f = Tkinter.Frame(input_frame)
		f.grid(row = r, column = 0, columnspan = 2, sticky = Tkinter.W)
		
		self.depthNormalizedIntVar = Tkinter.IntVar()
		Tkinter.Checkbutton(f, text = "Depth Normalized", variable = self.depthNormalizedIntVar, command = self.depthNormChecked).grid(row = 0, column = 0, sticky = Tkinter.W)
		self.depthNormalizedIntVar.set(1)

		self.areaNormalizedIntVar = Tkinter.IntVar()
		Tkinter.Checkbutton(f, text = "Area Normalized", variable = self.areaNormalizedIntVar, command = self.areaNormChecked).grid(row = 1, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		button = Tkinter.Button(input_frame, text="Output Directory", command = self.getSCROutputDirectory)
		button.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.outputSCRDirEntry = Tkinter.Entry(input_frame, width = 70)
		self.outputSCRDirEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.outputSCRDirEntry.insert(0, "/Users/ben/Desktop/test_cr/cr_out")
		
		r = r + 1
		
		run_scr_button = Tkinter.Button(input_frame, text = "Run!", command = self.run_cr);
		run_scr_button.grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		Tkinter.Label(input_frame, text = "Derivative Metrics").grid(row = r, column = 0, sticky = Tkinter.W, columnspan = 2, pady = 10)
		
		r = r + 1 
		
		Tkinter.Label(input_frame, text = "Window").grid(row = r, column = 0, sticky = Tkinter.E)
		self.derivWindowEntry = Tkinter.Entry(input_frame, width = 5)
		self.derivWindowEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.derivWindowEntry.insert(0, "5")
		
		r = r + 1
		
		Tkinter.Label(input_frame, text = "Threshold").grid(row = r, column = 0, sticky = Tkinter.E)
		self.derivThresholdEntry = Tkinter.Entry(input_frame, width = 5)
		self.derivThresholdEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.derivThresholdEntry.insert(0, "1")
		
		r = r + 1
		
		button = Tkinter.Button(input_frame, text="Output Directory", command = self.getDerivOutputDirectory)
		button.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.outputDerivDirEntry = Tkinter.Entry(input_frame, width = 70)
		self.outputDerivDirEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.outputDerivDirEntry.insert(0, "/Users/ben/Desktop/test_cr/cr_out")
		
		r = r + 1
		
		run_deriv_button = Tkinter.Button(input_frame, text = "Run!", command = self.run_deriv)
		run_deriv_button.grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 5)
		
		r = r + 1
		
		##################################################################
		###################### dynamic range #############################
		##################################################################
		
		"""
		self.dynamicRangeIntVar = Tkinter.IntVar()
		cb = Tkinter.Checkbutton(input_frame, text="DYNAMIC RANGE:", variable = self.dynamicRangeIntVar, command = self.dynamicRangeChecked)
		cb.grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		Tkinter.Label(input_frame, text="Kernel").grid(row = r, column = 0, sticky = Tkinter.W, padx = 5)
		
		self.kernelEntry = Tkinter.Entry(input_frame, width = 3)
		self.kernelEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.kernelEntry.insert(0, "3")
		
		r = r + 1
		
		label = Tkinter.Label(input_frame, text="Short Range")
		label.grid(row = r, column = 0, sticky = Tkinter.NW, padx = 5)
		
		
		#short dynamic range frame
		f1 = Tkinter.Frame(input_frame)
		f1.grid(row = r, column = 1, sticky = Tkinter.NW)
		
		self.shortRangeStartEntry = Tkinter.Entry(f1, width = 5)
		self.shortRangeStartEntry.grid(row = 0, column = 0, sticky = Tkinter.W)
		self.shortRangeStartEntry.insert(0, "500")
		
		label = Tkinter.Label(f1, text="-")
		label.grid(row = 0, column = 1, sticky = Tkinter.W, padx = 0)
		
		self.shortRangeStopEntry = Tkinter.Entry(f1, width = 5)
		self.shortRangeStopEntry.grid(row = 0, column = 2, sticky = Tkinter.W)
		self.shortRangeStopEntry.insert(0, "600")
		
		r = r + 1
		
		Tkinter.Label(input_frame, text="Strategy").grid(row = r, column = 0, sticky = Tkinter.NW, padx = 5)
		
		f = Tkinter.Frame(input_frame)
		f.grid(row = r, column = 1, columnspan = 3, sticky = Tkinter.NW)
		
		self.shortRangeZeroIntVar = Tkinter.IntVar()
		shortRangeZeroCheckbutton = Tkinter.Checkbutton(f, text="0 position of 1st derivative", variable = self.shortRangeZeroIntVar, command = self.shortRangeZeroChecked)
		shortRangeZeroCheckbutton.grid(row = 0, column = 0, sticky = Tkinter.W)
		
		self.shortRangeMinIntVar = Tkinter.IntVar()
		shortRangeMinCheckbutton = Tkinter.Checkbutton(f, text="min position of 1st derivitive", variable = self.shortRangeMinIntVar, command = self.shortRangeMinChecked)
		shortRangeMinCheckbutton.grid(row = 1, column = 0, sticky = Tkinter.W)
		
		self.shortRangeMaxIntVar = Tkinter.IntVar()
		shortRangeMaxCheckbutton = Tkinter.Checkbutton(f, text="max position of 1st derivitive", variable = self.shortRangeMaxIntVar, command = self.shortRangeMaxChecked)
		shortRangeMaxCheckbutton.grid(row = 2, column = 0, sticky = Tkinter.W)
		
		Tkinter.Frame(f).grid(row = 3, column = 0, pady = 5)
		
		r = r + 1
		
		label = Tkinter.Label(input_frame, text="Long Range")
		label.grid(row = r, column = 0, sticky = Tkinter.W, padx = 5)
		
		#long dynamic range frame
		f2 = Tkinter.Frame(input_frame)
		f2.grid(row = r, column = 1, sticky = Tkinter.NW)
		
		self.longRangeStartEntry = Tkinter.Entry(f2, width = 5)
		self.longRangeStartEntry.grid(row = 0, column = 0, sticky = Tkinter.W)
		self.longRangeStartEntry.insert(0, "700")
		
		label = Tkinter.Label(f2, text="-")
		label.grid(row = 0, column = 1, sticky = Tkinter.W, padx = 0)
		
		self.longRangeStopEntry = Tkinter.Entry(f2, width = 5)
		self.longRangeStopEntry.grid(row = 0, column = 2, sticky = Tkinter.W)
		self.longRangeStopEntry.insert(0, "800")
		
		r = r + 1
		
		Tkinter.Label(input_frame, text="Strategy").grid(row = r, column = 0, sticky = Tkinter.NW, padx = 5)
		
		f = Tkinter.Frame(input_frame)
		f.grid(row = r, column = 1, columnspan = 3, sticky = Tkinter.NW)
		
		self.longRangeZeroIntVar = Tkinter.IntVar()
		longRangeZeroCheckbutton = Tkinter.Checkbutton(f, text="0 position of 1st derivative", variable = self.longRangeZeroIntVar, command = self.longRangeZeroChecked)
		longRangeZeroCheckbutton.grid(row = 0, column = 0, sticky = Tkinter.W)
		
		self.longRangeMinIntVar = Tkinter.IntVar()
		longRangeMinCheckbutton = Tkinter.Checkbutton(f, text="min position of 1st derivitive", variable = self.longRangeMinIntVar, command = self.longRangeMinChecked)
		longRangeMinCheckbutton.grid(row = 1, column = 0, sticky = Tkinter.W)
		
		self.longRangeMaxIntVar = Tkinter.IntVar()
		longRangeMaxCheckbutton = Tkinter.Checkbutton(f, text="max position of 1st derivitive", variable = self.longRangeMaxIntVar, command = self.longRangeMaxChecked)
		longRangeMaxCheckbutton.grid(row = 2, column = 0, sticky = Tkinter.W)
		
		Tkinter.Frame(f).grid(row = 3, column = 0, pady = 5)
		
		"""
		
		###################################################################
		###################### Message Frame ##############################
		###################################################################
		
		## status frame
		messageFrame = Tkinter.Frame(master)
		messageFrame.grid(row = 0, column = 1, sticky = Tkinter.N, rowspan = 3, padx = 10, pady =10)
		
		resultLabel = Tkinter.Label(messageFrame, text = "Status Message...")
		resultLabel.grid(row = 0, column = 0, sticky = Tkinter.NW)
		
		self.textFrame = Tkinter.Frame(messageFrame)
		self.scroll = Tkinter.Scrollbar(self.textFrame)
		
		self.messageWindow = Tkinter.Text(self.textFrame, height = 32, width = 80, background = 'white', borderwidth = 1, relief = Tkinter.RIDGE, yscrollcommand=self.scroll.set)
		self.messageWindow.pack(side=Tkinter.LEFT);
		
		self.scroll.pack(side=Tkinter.RIGHT, fill=Tkinter.Y)
		self.scroll.config(command=self.messageWindow.yview)
		self.textFrame.grid(row = 1, column = 0);
		
		###################################################################
		######################### HELP Frame ##############################
		###################################################################
		
		exe_frame = Tkinter.Frame(master)
		exe_frame.grid(row = 4, column = 0, sticky = Tkinter.W, padx = 10)
		
		Tkinter.Button(exe_frame, text="HELP", command = self.showHelp).grid(row = 0, column = 0, sticky = Tkinter.SW, pady = 10)
		
		
		
	def showHelp(self):
		top = Tkinter.Toplevel()
		txt = Tkinter.Text(top, width = 120, height = 40)
		txt.insert(Tkinter.END, self.helpMessage)
		txt.update()
		txt.pack()
		
	def add_static_range(self, row_id):
		
		frame = Tkinter.Frame(self.static_range_frame)
		frame.grid(row = row_id, column = 0, sticky = Tkinter.NW)
		
		lr_entry  = Tkinter.Entry(frame, width = 12)
		lr_entry.grid(row = 0, column = 0, sticky = Tkinter.W, padx = 5)
		hr_entry  = Tkinter.Entry(frame, width = 12)
		hr_entry.grid(row = 0, column = 1, sticky = Tkinter.W, padx = 5)
		
		self.static_ranges.append({"lr_entry": lr_entry, "hr_entry": hr_entry})
		
		
	def depthNormChecked(self):
		val = self.depthNormalizedIntVar.get()
		if val == 1:
			self.areaNormalizedIntVar.set(0)
		else:
			self.areaNormalziedIntVar.set(1)
			
	def areaNormChecked(self):
		val = self.areaNormalizedIntVar.get()
		if val == 1:
			self.depthNormalizedIntVar.set(0)
		else:
			self.depthNormalizedIntVar.set(1)
			
	def staticRangeChecked(self):
		val = self.staticRangeIntVar.get()
		if val == 1:
			self.dynamicRangeIntVar.set(0)
			
	def dynamicRangeChecked(self):
		val = self.dynamicRangeIntVar.get()
		if val == 1:
			self.staticRangeIntVar.set(0) 
			
	def shortRangeZeroChecked(self):
		val = self.shortRangeZeroIntVar.get()
		if val == 1:
			self.shortRangeMinIntVar.set(0)
			self.shortRangeMaxIntVar.set(0)
			
	def shortRangeMinChecked(self):
		val = self.shortRangeMinIntVar.get()
		if val == 1:
			self.shortRangeZeroIntVar.set(0)
			self.shortRangeMaxIntVar.set(0)
			
	def shortRangeMaxChecked(self):
		val = self.shortRangeMaxIntVar.get()
		if val == 1:
			self.shortRangeZeroIntVar.set(0)
			self.shortRangeMinIntVar.set(0)
			
	def longRangeZeroChecked(self):
		val = self.longRangeZeroIntVar.get()
		if val == 1:
			self.longRangeMinIntVar.set(0)
			self.longRangeMaxIntVar.set(0)
			
	def longRangeMinChecked(self):
		val = self.longRangeMinIntVar.get()
		if val == 1:
			self.longRangeZeroIntVar.set(0)
			self.longRangeMaxIntVar.set(0)
			
	def longRangeMaxChecked(self):
		val = self.longRangeMaxIntVar.get()
		if val == 1:
			self.longRangeZeroIntVar.set(0)
			self.longRangeMinIntVar.set(0)
			
	def getInputDirectory(self):
		indir = tkFileDialog.askdirectory()
		if(indir != ""):
			self.inputDirEntry.delete(0,Tkinter.END)
			self.inputDirEntry.insert(0,indir)
			
	def getSCROutputDirectory(self):
		outdir = tkFileDialog.askdirectory()
		if(outdir != ""):
			self.outputSCRDirEntry.delete(0,Tkinter.END)
			self.outputSCRDirEntry.insert(0,outdir)
			
	def getDerivOutputDirectory(self):
		outdir = tkFileDialog.askdirectory()
		if(outdir != ""):
			self.outputDerivDirEntry.delete(0,Tkinter.END)
			self.outputDerivDirEntry.insert(0,outdir)
			
	def writeMessage(self, text):
		self.messageWindow.insert(Tkinter.END, text + "\n")
		self.messageWindow.update()
		self.messageWindow.yview(Tkinter.END)
		
	def extract_wavelengths(self, wl_string):
		
		split = wl_string.strip('{} \n').split(',')
		waves = []
		for s in split:
			waves.append(float(s))
		return waves
		
	def compute_cr_band_extents(self, waves, wl_low, wl_high):
		
		dist = sys.float_info.max
		lowi = 0
		for i in range(len(waves)):
			if abs(waves[i] - wl_low) < dist:
				dist = abs(waves[i] - wl_low)
				lowi = i
		
		dist  = sys.float_info.max	
		highi = 0
		for i in range(len(waves)):
			if abs(waves[i] - wl_high) < dist:
				dist  = abs(waves[i] - wl_high)
				highi = i
				
		return (highi + 1 - lowi, lowi, highi)
		
	##
	## generate output envi header
	## - subset the bands
	## - add band names
	##
	def generate_cr_header(self, in_path, out_path, waves, cbands, lowi, highi):
		
		(keys, key_val) = envi_header_io.read_as_is(in_path)
		
		#subset wavelength
		W = []
		for i in range(lowi, highi+1):
			W.append(waves[i])
		
		#generate wavelength and band names
		wls = "{"		#wavelengths
		bns = "{"		#band names
		
		for w in W:
			wls += str(w) + ",\n"
			bns += str(w) + ",\n"
			
		for i in range(self.n_features):
			if i == self.n_features-1:
				wls += str(waves[len(waves)-1] + 1 + i) + "}\n"
				bns += self.feature_names[i] + "}\n"
			else:
				wls += str(waves[len(waves)-1] + 1 + i) + ",\n"
				bns += self.feature_names[i] + ",\n"
		
		self.writeMessage("subset fwhm")
		
		#subset fwhm
		if 'fwhm' in keys:
			split = key_val['fwhm'].strip('{} \n\t').split(',')
			for i in range(len(split)):
				split[i] = split[i].strip()
			fwhm = "{"
			for i in range(lowi, highi+1):
				fwhm += split[i] + ",\n"
			for i in range(self.n_features):
				if i == self.n_features-1:
					fwhm += "0.0}\n"
				else:
					fwhm += "0.0,\n"
			key_val['fwhm'] = fwhm
			
		#add band names
		if not 'band names' in keys:
			keys.append('band names')
		
		key_val['wavelength'] = wls
		key_val['band names'] = bns
		key_val['bands'] = str(cbands + self.n_features) + "\n" #features appended to bands
		key_val['interleave'] = "bil\n"
		key_val['data type'] = "4\n"
		
		envi_header_io.write_as_is(out_path, keys, key_val)
		
		
	##
	## buffer the asd reflectance file and split it into waves and data
	##
	def buffer_asd_data(self, path):
		
		try:
			waves = []
			image = []
			fp_in = open(path, "rb")
			
			for line in fp_in:
				l = [float(s) for s in line.split()]
				waves.append(l[0])
				image.append(l[1:])
				
			fp_in.close()
			
			# convert image to lists of spectrum
			n_cols   = len(image[0])
			spectrum = []
			
			for col in range(n_cols):
				spectrum.append([list[col] for list in image])
			
			return (waves, spectrum)
		except:
			self.messageWindow.insert(Tkinter.END, "Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			return
			
	##
	## buffer csv file
	##
	def buffer_csv_data(self, path, cols_to_skip):
		
		try:
			fp_in = open(path, "r")

			lines = fp_in.readlines()
			
			waves = lines[0].split(",")
			waves = waves[cols_to_skip:]
			waves = [float(s) for s in waves]
			
			spectrum = []
			for i in range(1,len(lines)):
				l = lines[i].split(",")
				l = l[cols_to_skip:]
				l = [int(s) for s in l]
				spectrum.append(l)
			
			fp_in.close()
			
			return (waves, spectrum)
		except:
			self.messageWindow.insert(Tkinter.END, "Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			return
			
	def buffer_spectra_csv(self, path, cols_to_skip, col_id):
		
		f = open(path, "r")
		lines = f.readlines()
		f.close()
		
		#self.writeMessage("get waves:" + str(cols_to_skip))
		
		wave = lines[0].split(",")
		wave = [float(s.strip()) for s in wave[cols_to_skip:]]
			
		lines = lines[1:]
		
		#self.writeMessage("get spectra")
		
		spec = []
		name = []		
		for line in lines:
			ln = line.split(",")
			name.append(ln[col_id-1].strip())
			spec.append([float(s.strip()) for s in ln[cols_to_skip:]])
			
		#print name
		#print wave
		#print spec
		
		return (name, wave, spec)
			
	##
	## text dynamic range
	##
	def cr_text_dynamic(self, mt_out_path, cr_out_path, waves, n_waves, image, n_cols):
	
		self.writeMessage("cr_text_dynamic")
			
		wt = self.wave_thresh
		
		srange_min = float(self.shortRangeStartEntry.get())
		srange_max = float(self.shortRangeStopEntry.get())
		lrange_min = float(self.longRangeStartEntry.get())
		lrange_max = float(self.longRangeStopEntry.get())
		kernel = int(self.kernelEntry.get())
		
		if kernel % 2 == 0:
			self.writeMessage("Error. Kernel size must be odd.")
			return
		
		#
		# validate
		#
		if (srange_max - srange_min < wt) or (lrange_min - srange_max < wt) or (lrange_max - lrange_min < wt):
			self.writeMessage("Error. Bad wavelength range.")
			return
		
		ss0 = self.shortRangeZeroIntVar.get()
		ss1 = self.shortRangeMinIntVar.get()
		ss2 = self.shortRangeMaxIntVar.get()
		ls0 = self.longRangeZeroIntVar.get()
		ls1 = self.longRangeMinIntVar.get()
		ls2 = self.longRangeMaxIntVar.get()
		
		if ss0 == 0 and ss1 == 0 and ss2 == 0:
			self.writeMessage("Error. Must select short wavelength strategy.")
			return
			
		if ls0 == 0 and ls1 == 0 and ls2 == 0:
			self.writeMessage("Error. Must select short wavelength strategy.")
			return
		
		s_strat = 0
		if ss1 == 1: s_strat = 1
		if ss2 == 1: s_strat = 2
		
		l_strat = 0
		if ls1 == 1: l_strat = 1
		if ls2 == 1: l_strat = 2
		
		(cr_width, low, high) = \
		self.compute_cr_band_extents(waves, srange_min, lrange_max)
		
		cr_waves = waves[low:high+1]
		
		#
		# run
		#
		self.writeMessage("\tcr_text_setup")
		CR_module.cr_text_setup(cr_waves, cr_width, mt_out_path, cr_out_path)
		
		
		self.writeMessage("Processing:")
		
		for col in range(n_cols):
			
			#self.writeMessage("\tcolumn " + str(col+1) + "/" + str(n_cols))
			spectrum = [list[col] for list in image]
			cr_spect = spectrum[low:high+1]
			
			(low_i, high_i) =\
			CR_module.cr_dynamic_range(\
				spectrum,\
				waves,\
				n_waves,\
				s_strat,\
				l_strat,\
				srange_min,\
				srange_max,\
				lrange_min,\
				lrange_max,\
				kernel)
			
			if low_i < low or low_i > high:
				self.writeMessage("Error: low_i to high or low:" + str(low_i))
				return;
			if high_i < low or high_i > high:
				self.writeMessage("Error: high_i to high or low:" + str(high_i))
				return;
		
			offset  = low_i - low
			f_width = high_i - low_i + 1
			
			self.writeMessage("\t" + str(col+1) + "/" + str(n_cols) + "[" + str(low) + "," + str(low_i) + "," + str(high_i) + "," + str(high) + "]")
			
			#pass in the exact spectra
			CR_module.cr_text_dynamic(\
				cr_spect,\
				cr_waves,\
				cr_width,
				f_width,
				offset)
			
		CR_module.cr_text_teardown()
		self.writeMessage("Done!")
	
	
	##
	## text file driver
	##
	def cr_text(self, in_path, out_dir):
		
		self.writeMessage("cr_text")
		
		(waves, image) = self.buffer_asd_data(in_path)
		n_cols = len(image[0])
		
		if self.staticRangeIntVar.get() == 1:
			
			mt_out_path = out_dir + "/mt_static.txt"
			cr_out_path = out_dir + "/cr_static.txt"
		
			self.cr_text_static(mt_out_path, cr_out_path, waves, len(waves), image, n_cols)
		else:
			
			mt_out_path = out_dir + "/mt_dynamic.txt"
			cr_out_path = out_dir + "/cr_dynamic.txt"
		
			self.cr_text_dynamic(mt_out_path, cr_out_path, waves, len(waves), image, n_cols)
			
			
		self.writeMessage("Done")
		#SpectraViewer(cr_out_path, mt_out_path)
		
	
	##
	## envi dynamic range
	##
	def cr_envi_dynamic(self, in_hdr_path, in_dat_path, out_hdr_path, out_dat_path, waves):
		
		self.writeMessage("cr_envi_dynamic")
		
		wt = self.wave_thresh
		
		srange_min = float(self.shortRangeStartEntry.get())
		srange_max = float(self.shortRangeStopEntry.get())
		lrange_min = float(self.longRangeStartEntry.get())
		lrange_max = float(self.longRangeStopEntry.get())
		kernel = int(self.kernelEntry.get())
		
		if kernel % 2 == 0:
			self.writeMessage("Error. Kernel size must be odd.")
			return
		
		#validate wavelength range
		if (srange_max - srange_min < wt) or (lrange_min - srange_max < wt) or (lrange_max - lrange_min < wt):
			self.writeMessage("Error. Bad wavelength range.")
			return
		
		ss0 = self.shortRangeZeroIntVar.get()
		ss1 = self.shortRangeMinIntVar.get()
		ss2 = self.shortRangeMaxIntVar.get()
		ls0 = self.longRangeZeroIntVar.get()
		ls1 = self.longRangeMinIntVar.get()
		ls2 = self.longRangeMaxIntVar.get()
		
		if ss0 == 0 and ss1 == 0 and ss2 == 0:
			self.writeMessage("Error. Must select short wavelength strategy.")
			return
			
		if ls0 == 0 and ls1 == 0 and ls2 == 0:
			self.writeMessage("Error. Must select short wavelength strategy.")
			return
		
		s_strat = 0
		if ss1 == 1: s_strat = 1
		if ss2 == 1: s_strat = 2
		
		l_strat = 0
		if ls1 == 1: l_strat = 1
		if ls2 == 1: l_strat = 2
		
		
		(f_width, low_i, high_i) =\
		self.compute_cr_band_extents(waves, srange_min, lrange_max)
		
		self.writeMessage("f_width:" + str(f_width))
		self.writeMessage("low_i:" + str(low_i))
		self.writeMessage("high_i:" + str(high_i))
		
		#
		# generate output header
		#
		self.generate_cr_header(in_hdr_path, out_hdr_path, waves, f_width, low_i, high_i)
		
		#
		# compute the continuum removal
		#
		CR_module.cr_envi_dynamic(\
			in_hdr_path,\
			in_dat_path,\
			out_dat_path,\
			srange_min,\
			srange_max,\
			lrange_min,\
			lrange_max,\
			s_strat,\
			l_strat,\
			kernel)
		
		self.writeMessage("Done!")
	
	
	
		
		
	##
	## envi file driver
	##
	def cr_envi(self, in_path, out_dir):
		
		self.writeMessage("cr_envi")
		
		base = in_path[in_path.rfind("/")+1:in_path.rfind(".")]
		in_hdr_path = in_path[:in_path.rfind(".")] + ".hdr";
		in_dat_path = in_path[:in_path.rfind(".")] + ".dat";
		out_hdr_path = out_dir + "/" + base + ".hdr"
		out_dat_path = out_dir + "/" + base + ".dat"
		
		self.writeMessage("in_hdr_path: " + in_hdr_path)
		self.writeMessage("in_dat_path: " + in_dat_path)
		self.writeMessage("out_hdr_path: " + out_hdr_path)
		self.writeMessage("out_dat_path: " + out_dat_path)
		
		d = envi_header_io.read(in_hdr_path)			
		waves = self.extract_wavelengths(d['wavelength'])
		
		if self.staticRangeIntVar.get() == 1:
			self.cr_envi_static(in_hdr_path, in_dat_path, out_hdr_path, out_dat_path, waves)
		else:
			self.cr_envi_dynamic(in_hdr_path, in_dat_path, out_hdr_path, out_dat_path, waves)
			
			
	##
	## main driver
	##
	def start(self):
		
		try:
			in_path = self.inputFileEntry.get()
			out_dir = self.outputDirEntry.get()
			
			if in_path == "" or out_dir == "" :
				self.writeMessage("Error. Missing input file or output directory.")
				return
			
			if self.staticRangeIntVar.get() == 0 and self.dynamicRangeIntVar == 0:
				self.writeMessage("Error. Select STATIC or DYNAMIC range.\n")
				return
				
			if in_path.endswith(".txt"):
				self.cr_text(in_path, out_dir)
			else:
				self.cr_envi(in_path, out_dir)
				
		except:
			self.messageWindow.insert(Tkinter.END, "Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			return
		
	
		
		
	###########################################################################
	#
	## ###################### Cont. Removal Static ######################### ##
	#
	###########################################################################
	
	##
	## get the ranges
	##
	def get_static_ranges(self):	
		
		static_ranges = []
		i = 0
		for d in self.static_ranges:
			try:				
				static_ranges.append([float(d["lr_entry"].get()), float(d["hr_entry"].get())])
			except ValueError:
				continue
			#self.writeMessage(str(static_ranges[i][0]) + "," + str(static_ranges[i][1]))
			i = i+1
				
		return static_ranges
		
	def get_dynamic_ranges(self):
		
		dynamic_ranges = []
		
		i = 0
		for d in self.static_ranges:
			try:				
				s = d["lr_entry"].strip().split("-")
				ll = float(s[0])
				lh = float(s[1])
				s = d["hr_entry"].strip().split("-")
				hl = float(s[0])
				hh = float(s[1])
			except ValueError:
				continue
			self.writeMessage(str(static_ranges[i][0]) + "," + str(static_ranges[i][1]))
			i = i+1
				
		return dynamic_ranges
	
	
		
	###########################################################################
	#
	## ########################## Static ENVI ############################## ##
	#
	###########################################################################


	##
	## generate output envi header
	## - subset wavelengths and fwhm based on 'lowi' and 'highi'
	## - set band names to 'self.feature_names'
	## - append features as bands to the end of the image
	## 
	##
	def generate_cr_header(self, in_path, out_path, waves, cbands, lowi, highi):
		
		(keys, key_val) = envi_header_io.read_as_is(in_path)
		
		#subset wavelength
		W = []
		for i in range(lowi, highi+1):
			W.append(waves[i])
		
		#generate wavelength and band names
		wls = "{"		#wavelengths
		bns = "{"		#band names
		
		for w in W:
			wls += str(w) + ",\n"
			bns += str(w) + ",\n"
			
		for i in range(self.n_features):
			if i == self.n_features-1:
				wls += str(waves[len(waves)-1] + 1 + i) + "}\n"
				bns += self.feature_names[i] + "}\n"
			else:
				wls += str(waves[len(waves)-1] + 1 + i) + ",\n"
				bns += self.feature_names[i] + ",\n"
		
		self.writeMessage("subset fwhm")
		
		#subset fwhm
		if 'fwhm' in keys:
			split = key_val['fwhm'].strip('{} \n\t').split(',')
			fwhm = "{"
			for i in range(lowi, highi+1):
				fwhm += split[i].strip() + ",\n"
			for i in range(self.n_features):
				if i == self.n_features-1:
					fwhm += "0.0}\n"
				else:
					fwhm += "0.0,\n"
					
		#add band names
		if not 'band names' in keys:
			keys.append('band names')
		
		key_val['fwhm'] = fwhm
		key_val['wavelength'] = wls
		key_val['band names'] = bns
		key_val['bands'] = str(cbands + self.n_features) + "\n"
		key_val['interleave'] = "bil\n"
		key_val['data type'] = "4\n"
		
		envi_header_io.write_as_is(out_path, keys, key_val)
		

	def cr_envi_static(self, in_dir, f, out_dir, waves):
		
		self.writeMessage("cr_envi_dynamic")
		
		wt = self.wave_thresh
		static_ranges = self.get_static_ranges()
		
		base = f[:f.rfind(".")]
		
		for static_range in static_ranges:
			
			wl_low = static_range[0]
			wl_high = static_range[1]
			
			if (wl_high - wl_low < wt) or (wl_high >= waves[len(waves)-1]) or (wl_low  <= waves[0]):
				self.writeMessage("Error. Band wavelength range.")
				return
				
			r_out_dir = "%s/%f-%f" % (out_dir, wl_low, wl_high)
			
			if not os.path.exists(r_out_dir):
				os.makedirs(r_out_dir)
			
			in_hdr_path = in_dir + "/" + f + ".hdr" 
			in_dat_path = in_dir + "/" + f + ".dat"
			out_hdr_path = r_out_dir + "/" + f + ".hdr"
			out_dat_path = r_out_dir + "/" + f + ".dat"
			
			(f_width, low_i, high_i) =\
			self.compute_cr_band_extents(waves, wl_low, wl_high)
			
			self.generate_cr_header(in_hdr_path, out_hdr_path, waves, f_width, low_i, high_i)
			
			CR_module.cr_envi(in_hdr_path, in_dat_path, out_dat_path, wl_low, wl_high)
		
		self.writeMessage("Done!")
		
		
	def cr_envi(self, in_dir, f, out_dir):
		
		self.writeMessage("cr_envi")
		
		
		
		self.writeMessage("in_hdr_path: " + in_hdr_path)
		self.writeMessage("in_dat_path: " + in_dat_path)
		self.writeMessage("out_hdr_path: " + out_hdr_path)
		self.writeMessage("out_dat_path: " + out_dat_path)
		
		if self.staticRangeIntVar.get() == 1:
			self.cr_envi_static(in_dir, f, out_dir)
		else:
			self.cr_envi_dynamic(in_dir, f, out_dir)

	###########################################################################
	#
	## ########################## Static Text ############################## ##
	#
	###########################################################################


	#static range continuum removal on text files
	def cr_text_static(self, fname, names, waves, spec, f_width, wl_low, wl_high):
		
		#self.writeMessage("- continuum removal: " + fname)
		
		out_dir = self.outputSCRDirEntry.get()
		out_dir = out_dir + "/" + str(wl_low) + "-" + str(wl_high)
		d_norm  = self.depthNormalizedIntVar.get()
		
		if not os.path.exists(out_dir):
			os.makedirs(out_dir)
	
		mt_out_path = out_dir + "/mt_" + fname[:fname.rfind('.')] + ".csv"
		cr_out_path = out_dir + "/cr_" + fname[:fname.rfind('.')] + ".csv"
		
		#self.writeMessage("- setup")
		
		CR_module.cr_text_setup(waves, f_width, mt_out_path, cr_out_path)
		
		for i in range(len(spec)):
			
			#self.writeMessage("\t - spectra " + str(i+1) + "/" + str(len(spec)))
			
			CR_module.cr_text(spec[i], waves, f_width, names[i], d_norm)
			
		CR_module.cr_text_teardown()
		#SpectraViewer(cr_out_path, mt_out_path)
		
		
	#static range derivatives on text files
	def deriv_text_static(self, fname, names, waves, spec, f_width, wl_low, wl_high):
		
		self.writeMessage("- derivative metrics: " + fname)
	
		window = int(self.derivWindowEntry.get())
		threshold = float(self.derivThresholdEntry.get())
		out_dir = self.outputDerivDirEntry.get()
		out_dir = out_dir + "/" + str(wl_low) + "-" + str(wl_high)
		
		run_second_deriv = True
		
		if not os.path.exists(out_dir):
			os.makedirs(out_dir)
	
		#deriv_out_path = out_dir + "/deriv_" + fname[:fname.rfind('.')] + ".txt"
		
		sp_out_path = out_dir + "/spectra_" + fname
		d1_out_path = out_dir + "/1st_deriv_" + fname
		d2_out_path = out_dir + "/2nd_deviv_" + fname
		mm_out_path = out_dir + "/min_max_"   + fname
		if_out_path = out_dir + "/inflection_" + fname
		
		CR_module.deriv_text_setup(waves, f_width, sp_out_path, d1_out_path, d2_out_path, mm_out_path, if_out_path)
		
		for i in range(len(spec)):
			#self.writeMessage("\tspectra " + str(i+1) + "/" + str(len(spec)))
			CR_module.deriv_text_static(spec[i], waves, f_width, names[i], window, threshold)
			
		CR_module.deriv_text_teardown()
		
	
	def run_text_static(self, in_path, fname, run_cr, run_deriv):
		
		#self.writeMessage("run_text_static");
		
		cols_to_skip = int(self.col_skip_entry.get())
		col_id = int(self.col_id_entry.get())
		
		wt = self.wave_thresh
		
		#(waves, spectrum) = self.buffer_csv_data(in_path, cols_to_skip)
		names, waves, spectrum = self.buffer_spectra_csv(in_path, cols_to_skip, col_id)
		
		static_ranges = self.get_static_ranges()
		
		for static_range in static_ranges:
			
			wl_low  = static_range[0]
			wl_high = static_range[1]
			
			#self.writeMessage("- processing range : [" + str(wl_low) + ", " + str(wl_high) + "]")
			
			#
			#validate wavelength range
			#
			if (wl_high - wl_low < wt) or (wl_high >= waves[len(waves)-1]) or (wl_low  <= waves[0]):
				self.writeMessage("Warning: wavelength range extends beyond extent.")
			
			#
			#compute extent of the feature
			#
			(f_width, low_i, high_i) = self.compute_cr_band_extents(waves, wl_low, wl_high)
			
			#
			#subset to include only the feature of interest
			#
			cwaves = waves[low_i:high_i+1] #wavelengths for current feature
			cspec  = []
			for i in range(len(spectrum)):
				cspec.append(spectrum[i][low_i:high_i+1])
			
			if run_cr:
				self.cr_text_static(fname, names, cwaves, cspec, f_width, wl_low, wl_high)
			elif run_deriv:
				self.deriv_text_static(fname, names, cwaves, cspec, f_width, wl_low, wl_high)
		
	
	def run_static(self, run_cr, run_deriv):
		
		try:
			self.writeMessage("STATIC RANGE")
			
			in_dir  = self.inputDirEntry.get()
			out_dir = self.outputSCRDirEntry.get()
			
			if in_dir == "" or out_dir == "" :
				self.writeMessage("Error. Missing input file or output directory.")
				return
				
			for f in os.listdir(in_dir):
				in_path = in_dir + "/" + f
				if f.endswith(".csv"):
					self.run_text_static(in_path, f, run_cr, run_deriv)
				elif f.endswith(".hdr"):
					self.cr_envi_static(in_dir, f, out_dir)
				
		except:
			self.writeMessage("Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			return
			
	def run_cr(self):
		
		try:
			self.run_static(True, False)
			self.writeMessage("Done!")
		except:
			self.writeMessage("Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			return
			
	def run_deriv(self):
		
		try:
			self.run_static(False, True)
		except:
			self.writeMessage("Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			self.writeMessage("Done!")
			return
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
