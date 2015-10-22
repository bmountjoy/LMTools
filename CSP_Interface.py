

import Tkinter
import sys
import os
from os import path
import tkFileDialog
import envi_header_io
import CSP_module


class Init:
	
	def __init__(self, master, clipBoard):
	
	
		self.helpMessage = "CONVOLUTION\n\n" + \
			"Reference Spectra: CSV file.\n" + \
			" - row 0: contains the wavelength information of the spectra\n" +\
			" - row i: contains the i-th spectra\n" +\
			" - the first column of each row > 0 contains the class name of the corresponding spectra\n" +\
			" - the class name is organized as 'class_identifier' where class is the class of the spectra and the identifier is used to distinguish spectra belonging to the same class\n\n" +\
			"Observered Spectra: ENVI header file. The resolution of the library spectra are reduced to match the resolution described by this header.\n\n" + \
			"Output Directory: The directory to which the convolved library spectra will be written."
	
	
	
		frame = Tkinter.Frame(master)
		frame.grid(row = 0, column = 0, padx = 10, sticky = Tkinter.NW)
		
		###################### Pad Frame ##################################
		
		padframe1 = Tkinter.Frame(frame)
		padframe1.grid(row = 0, column = 0, pady = 0, sticky = Tkinter.NW)
		
		###################################################################
		###################### Input Frame ################################
		###################################################################
		
		input_frame = Tkinter.Frame(frame)
		input_frame.grid(row = 1, column = 0, sticky = Tkinter.NW)
		
		#grid row
		r = 0
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 16, sticky = Tkinter.NW)
		
		r = r + 1
		
		Tkinter.Button(input_frame, text="Reference Spectra", command = self.getInputReferenceFile).grid(row = r, column = 0, sticky = Tkinter.NW)
		
		self.inputReferenceEntry = Tkinter.Entry(input_frame, width = 70)
		self.inputReferenceEntry.grid(row = r, column = 1, sticky = Tkinter.NW)
		self.inputReferenceEntry.insert(0,"/Users/ben/Desktop/test_cv/lib_spec_csv/Gravel_asd.csv")
		
		r = r + 1
		
		Tkinter.Button(input_frame, text="Observed Spectra", command = self.getInputObservedFile).grid(row = r, column = 0, sticky = Tkinter.NW)
		
		self.inputObservedEntry = Tkinter.Entry(input_frame, width = 70)
		self.inputObservedEntry.grid(row = r, column = 1, sticky = Tkinter.NW)
		self.inputObservedEntry.insert(0,"/Users/ben/Desktop/test_cv/obs_spec/0906-2152.hdr")
		
		r = r + 1
		
		Tkinter.Button(input_frame, text="Output Directory", command = self.getOutputDirectory).grid(row = r, column = 0, sticky = Tkinter.NW)
		
		self.outDirectoryEntry = Tkinter.Entry(input_frame, width = 70)
		self.outDirectoryEntry.grid(row = r, column = 1, sticky = Tkinter.NW)
		self.outDirectoryEntry.insert(0,"/Users/ben/Desktop/test_cv/lib_conv")
		
		r = r + 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 5)
		
		r = r + 1
		
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
		
		self.messageWindow = Tkinter.Text(self.textFrame, height = 30, width = 80, background = 'white', borderwidth = 1, relief = Tkinter.RIDGE, yscrollcommand=self.scroll.set)
		self.messageWindow.pack(side=Tkinter.LEFT);
		
		self.scroll.pack(side=Tkinter.RIGHT, fill=Tkinter.Y)
		self.scroll.config(command=self.messageWindow.yview)
		self.textFrame.grid(row = 1, column = 0);
		
		###################################################################
		###################### Execute Frame ##############################
		###################################################################
		
		exe_frame = Tkinter.Frame(master)
		exe_frame.grid(row = 4, column = 0, padx = 10, pady = 10, sticky = Tkinter.W)
		
		copyImageButton = Tkinter.Button(exe_frame, text="Start", command = self.convolve)
		copyImageButton.grid(row = 0, column = 0, sticky = Tkinter.SW, pady = 10)
		
		Tkinter.Button(exe_frame, text= "HELP", command = self.showHelp).grid(row = 0, column = 1)
		
		
	def showHelp(self):
		top = Tkinter.Toplevel()
		txt = Tkinter.Text(top, width = 100)
		txt.insert(Tkinter.END, self.helpMessage)
		txt.update()
		txt.pack()
		
	def getInputReferenceFile(self):
		infile = tkFileDialog.askopenfilename()
		if(infile != ""):
			self.inputReferenceEntry.delete(0,Tkinter.END)
			self.inputReferenceEntry.insert(0,infile)
			
	def getInputObservedFile(self):
		infile = tkFileDialog.askopenfilename()
		if(infile != ""):
			self.inputObservedEntry.delete(0,Tkinter.END)
			self.inputObservedEntry.insert(0,infile)
			
	def getOutputDirectory(self):
		outdir = tkFileDialog.askdirectory()
		if(outdir != ""):
			self.outDirectoryEntry.delete(0,Tkinter.END)
			self.outDirectoryEntry.insert(0,outdir)
			
	def writeMessage(self, text):
		self.messageWindow.insert(Tkinter.END, text + "\n")
		self.messageWindow.update()
		self.messageWindow.yview(Tkinter.END)
		
	##
	## todo: not used - maybe be used in the future
	##
	def buffer_csv_data(self, path, cols_to_skip):
		
		fp_in = open(path, "rb")

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
		
	##
	## todo: not used - maybe be used in the future
	##
	def extract_wavelengths_csv(self, path, cols_to_skip):
		
		f = open(path, "rb")
		line = f.readline().strip().split(",")
		line = line[cols_to_skip:]
		wave = [float(s) for s in line]
		f.close()
		return wave

	def convolve(self):
		
		try:
			lib_path = self.inputReferenceEntry.get()
			obs_path = self.inputObservedEntry.get()
			out_dir  = self.outDirectoryEntry.get()
			out_path = out_dir + '/c_' + lib_path[lib_path.rfind('/')+1:]
			
			if obs_path.endswith(".hdr") or obs_path.endswith(".dat"):
				d = envi_header_io.read(obs_path[:obs_path.rfind(".")] + ".hdr")
				fwhm = envi_header_io.extract_float_list(d['fwhm'])
				wave = envi_header_io.extract_float_list(d['wavelength'])
			elif obs_path.endswith(".csv"):
				self.write_message("Error: no support for observed CSV files.")
				return
			else:
				self.write_message("Error: do not supported observed spectra file type.")
				return
			
			if len(fwhm) != len(wave):
				self.writeMessage("Error: # of wavelength != # of fwhm values")
				return

			self.writeMessage("Convolving...")
			self.writeMessage("    Lib: " + lib_path)
			self.writeMessage("    Obs: " + obs_path)
			self.writeMessage("    Out: " + out_path)
			
			
			CSP_module.convolve(wave, fwhm, len(wave), lib_path, obs_path, out_path)
			
			self.writeMessage("Done!")
			
			
			
		except:
			self.writeMessage("Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
