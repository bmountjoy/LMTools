

import Tkinter
import sys
import os
from os import path
import tkFileDialog
import TCM_module


class Init:
	
	def __init__(self, master, clipBoard):
		
		self.clipBoard = clipBoard
		self.helpMessage = "Treecrown Biometrics\n" + \
		"  Vector File - Input shape file containing the tree crowns.\n" + \
		"  Lidar Directory - Directory of las lidar files.\n\n" + \
		"Spectral Extraction + Metrics\n" + \
		"  Hyperspectral Directory - Directory of ENVI standard files containing hyperspectral data. Data type must be unsigned short.\n" + \
		"  Flight Line Directory - Directory of shape files outlining the hyperspectral files specfied by the Hyperspectral Directory entry. Flight line file must have the same name as the corresponding hyperspectral file.\n"+ \
		"  Lidar Directory - Directory of las lidar files.\n"+ \
		"  Treecrown Directory - Directory of shape files containing tree crowns.\n" + \
		"  Albedo Threshold - A number in the range 0-100 that represents the lower bound albedo level of pixels in which spectral metrics are calculated.\n"
	
		frame = Tkinter.Frame(master)
		frame.grid(row = 0, column = 0)
		
		###################### Pad Frame ##################################
		
		padframe1 = Tkinter.Frame(frame)
		padframe1.grid(row = 0, column = 0, pady = 16)
		
		###################################################################
		###################### Input Frame ################################
		###################################################################
		
		
		#Tkinter.Frame(frame).grid(row = 1, column = 0, padx = 5)
		input_frame = Tkinter.Frame(frame)
		input_frame.grid(row = 1, column = 0)
		
		#grid row
		r = 0
		
		outDirButton = Tkinter.Button(input_frame, text="Output Directory:", command = self.getOutputDirectory)
		outDirButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.outDirEntry = Tkinter.Entry(input_frame, width = 70)
		self.outDirEntry.grid(row = r, column = 1)
		self.outDirEntry.insert(0,"/Volumes/data1/000/Mereworth_crashing/test_bio_out_on_ss")
		
		r = r + 1
		
		#spacing frame
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 5)
		
		r = r + 1
		
		#
		# Biometrics section
		#
		tcbioLabel = Tkinter.Label(input_frame, text = "Treecrown Biometrics", font = ("Times", 16))
		tcbioLabel.grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		inVectorDirButton = Tkinter.Button(input_frame, text="Treecrown Directory:", command = self.getInputVectorDirectory)
		inVectorDirButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inVectorDirEntry = Tkinter.Entry(input_frame, width = 70)
		self.inVectorDirEntry.grid(row = r, column = 1)
		self.inVectorDirEntry.insert(0,"")
		
		r = r + 1
		
		inLidarDirButton = Tkinter.Button(input_frame, text="Lidar Directory:", command = self.getInputLidarDirectory)
		inLidarDirButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inLidarDirEntry = Tkinter.Entry(input_frame, width = 70)
		self.inLidarDirEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.inLidarDirEntry.insert(0,"")
		
		r = r + 1
		
		hThreshLabel = Tkinter.Label(input_frame, text = "Height Threshold (meters)")
		hThreshLabel.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.hThreshEntry = Tkinter.Entry(input_frame, width = 8)
		self.hThreshEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		#self.hThreshEntry.insert(0,'2.1')
		
		r = r + 1
		
		bucketLabel = Tkinter.Label(input_frame, text = "Bucket Size (meters)")
		bucketLabel.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.bucketEntry = Tkinter.Entry(input_frame, width = 8)
		self.bucketEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		#self.bucketEntry.insert(0, '4.1')
		
		r = r + 1
		
		run = Tkinter.Button(input_frame, text="Run Treecrown Biometrics", command = self.runBio)
		run.grid(row = r, column = 0, sticky = Tkinter.SW, pady = 10, padx = 10)
		
		r = r + 1
		
		#
		# Hyperspectral section
		#
		hyperSubsetLabel = Tkinter.Label(input_frame, text = "Spectral Extraction + Metrics", font = ("Times", 16))
		hyperSubsetLabel.grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		inChmFileButton = Tkinter.Button(input_frame, text="CHM File:", command = self.getInputChmFile)
		inChmFileButton.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.inChmFileEntry = Tkinter.Entry(input_frame, width = 70)
		self.inChmFileEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.inChmFileEntry.insert(0, "/Volumes/data1/000/Mereworth_crashing/Mereworth_test_CHM_05m_c1_first_return_snap_ss.tif")
		
		r = r + 1
		
		inHyperDirectoryButton = Tkinter.Button(input_frame, text="Hyperspectral Directory:", command = self.getInputHyperDirectory)
		inHyperDirectoryButton.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		
		self.inHyperDirEntry = Tkinter.Entry(input_frame, width = 70)
		self.inHyperDirEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.inHyperDirEntry.insert(0,"/Volumes/data1/000/Mereworth_crashing/hyper")
		
		r = r + 1
		
		inFLDirectoryButton = Tkinter.Button(input_frame, text="Flight Line Directory:", command = self.getInputFLDirectory)
		inFLDirectoryButton.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		
		self.inFLDirEntry = Tkinter.Entry(input_frame, width = 70)
		self.inFLDirEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.inFLDirEntry.insert(0,"/Volumes/data1/000/Mereworth_crashing/fl_shp")
		
		r = r + 1
		
		inSELidarDirButton = Tkinter.Button(input_frame, text="Lidar Directory", command = self.getInputSELidarDir)
		inSELidarDirButton.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.inSELidarDirEntry = Tkinter.Entry(input_frame, width = 70)
		self.inSELidarDirEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.inSELidarDirEntry.insert(0, "/Volumes/data1/000/Mereworth_crashing/lid_subset_index")
		
		r = r + 1
		
		inTCDirectoryButton = Tkinter.Button(input_frame, text="Treecrown Directory", command = self.getInputTCDirectory)
		inTCDirectoryButton.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.inTCDirectoryEntry = Tkinter.Entry(input_frame, width = 70)
		self.inTCDirectoryEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.inTCDirectoryEntry.insert(0, "/Volumes/data1/000/Mereworth_crashing/crowns")
		
		r = r + 1
		
		albedoThresholdLabel = Tkinter.Label(input_frame, text = "Albedo Threshold Percentile")
		albedoThresholdLabel.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.albedoThresholdEntry = Tkinter.Entry(input_frame, width = 8)
		self.albedoThresholdEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		
		
		r = r + 1
		
		self.ssByHeightIntVar = Tkinter.IntVar()
		Tkinter.Checkbutton(input_frame, text = "Subset by Height", var = self.ssByHeightIntVar).grid(row = r, column = 0, padx = 10, sticky = Tkinter.W)
		
		self.ssByHeightEntry = Tkinter.Entry(input_frame, width = 8)
		self.ssByHeightEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		
		r = r + 1
		
		self.output_hyper_extraction = Tkinter.IntVar()
		Tkinter.Checkbutton(input_frame, text = "Output Extraction", variable = self.output_hyper_extraction).grid(row = r, column = 0, padx = 10, sticky = Tkinter.NW)
		self.output_hyper_extraction.set(1)
		
		r = r + 1
		
		self.output_hyper_metrics = Tkinter.IntVar()
		Tkinter.Checkbutton(input_frame, text = "Output Metrics", variable = self.output_hyper_metrics).grid(row = r, column = 0, padx = 10, sticky = Tkinter.NW)
		self.output_hyper_metrics.set(1)
		
		r = r + 1
		
		run_hyper = Tkinter.Button(input_frame, text="Run Extraction + Metrics", command = self.runHyper)
		run_hyper.grid(row = r, column = 0, pady = 10, padx = 10)
		
		r = r + 1
		
		helpButton = Tkinter.Button(input_frame, text="Help", command = self.showHelp)
		helpButton.grid(row = r, column = 0)
		
		
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
		###################### Execute Frame ##############################
		###################################################################
		
		exe_frame = Tkinter.Frame(master)
		exe_frame.grid(row = 4, column = 0, sticky = Tkinter.W)
	
	def showHelp(self):
		top = Tkinter.Toplevel()
		txt = Tkinter.Text(top, width = 121)
		txt.insert(Tkinter.END, self.helpMessage)
		txt.update()
		txt.pack()
		
	def getInputLidarDirectory(self):
		indir = tkFileDialog.askdirectory()
		if(indir != ""):
			self.inLidarDirEntry.delete(0,Tkinter.END)
			self.inLidarDirEntry.insert(0,indir)
			
	def getInputVectorDirectory(self):
		indir = tkFileDialog.askdirectory()
		if(indir != ""):
			self.inVectorDirEntry.delete(0, Tkinter.END)
			self.inVectorDirEntry.insert(0, indir)
			
	def getInputChmFile(self):
		file = tkFileDialog.askopenfilename()
		if(file != ""):
			self.inChmFileEntry.delete(0, Tkinter.END)
			self.inChmFileEntry.insert(0, file)
		
	def getInputHyperDirectory(self):
		direct = tkFileDialog.askdirectory()
		if(direct != ""):
			self.inHyperDirEntry.delete(0, Tkinter.END)
			self.inHyperDirEntry.insert(0, direct)
	
	def getInputFLDirectory(self):
		directory = tkFileDialog.askdirectory()
		if(directory != ""):
			self.inFLDirEntry.delete(0, Tkinter.END)
			self.inFLDirEntry.insert(0, directory)
			
	def getInputSELidarDir(self):
		directory = tkFileDialog.askdirectory()
		if(directory != ""):
			self.inSELidarDirEntry.delete(0, Tkinter.END)
			self.inSELidarDirEntry.insert(0, directory)
	
	def getInputTCDirectory(self):
		direct = tkFileDialog.askdirectory()
		if(direct != ""):
			self.inTCDirectoryEntry.delete(0, Tkinter.END)
			self.inTCDirectoryEntry.insert(0, direct)
			
	def getOutputDirectory(self):
		outdir = tkFileDialog.askdirectory()
		if(outdir != ""):
			self.outDirEntry.delete(0,Tkinter.END)
			self.outDirEntry.insert(0,outdir)
			
	def writeMessage(self, text):
		self.messageWindow.insert(Tkinter.END, text)
		self.messageWindow.update()
		self.messageWindow.yview(Tkinter.END)
		
	##
	##
	def runBio(self):
		
		try:
			
			lid_dir = self.inLidarDirEntry.get()
			vec_dir = self.inVectorDirEntry.get()
			out_dir  = self.outDirEntry.get()
			z_thresh = float(self.hThreshEntry.get())
			b_size   = float(self.bucketEntry.get())
			
			if(lid_dir == ""):
				self.writeMessage("Must specify input lidar file.\n\n")
				return
			
			if(vec_dir == ""):
				self.writeMessage("Must specify input vector file.\n\n")
				return
				
			if(out_dir == ""):
				self.writeMessage("Must specify output directory.\n\n")
				return
				
			if(z_thresh < 0):
				self.writeMessage("Height threshold must be a postive integer.\n\n")
				return
			
			if(b_size < 0):
				self.writeMessagew("Bucket size must be positive. \n\n")
				return
				
			self.writeMessage("Running treecrown biometrics\n")
			
			for f in os.listdir(vec_dir):
				if f.endswith(".shp"):
					path = vec_dir + "/" + f
					self.writeMessage("\tTC file:" + f + "\n")
					TCM_module.treeCrownMetrics(lid_dir, path, f[:f.rfind('.')], out_dir, z_thresh, b_size)
			
			self.writeMessage("Done.\n\n")
				
		except:
			self.messageWindow.insert(Tkinter.END, str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			return
			
	def runHyper(self):
		
		try:
			chm_file = self.inChmFileEntry.get()
			hspec_dir = self.inHyperDirEntry.get()
			fline_dir = self.inFLDirEntry.get()
			lid_dir = self.inSELidarDirEntry.get()
			tc_dir  = self.inTCDirectoryEntry.get()
			out_dir = self.outDirEntry.get()
			albedo  = float(self.albedoThresholdEntry.get()) / 100.0
			ssbyh   = self.ssByHeightIntVar.get()
			output_met = self.output_hyper_metrics.get()
			output_ext = self.output_hyper_extraction.get()
			
			n_to_ss = 0
			if ssbyh == 1:
				n_to_ss = int(self.ssByHeightEntry.get())
			
			if(hspec_dir == ""):
				self.writeMessage("Must specify an input hyperspectral directory.\n\n")
				return
				
			if(fline_dir == ""):
				self.writeMessage("")
				return
				
			if(lid_dir == ""):
				self.writeMessage("Must specify an input lidar directory.\n\n")
				return
				
			if(tc_dir == ""):
				self.writeMessage("Must specify an input treecrown directory.\n\n")
				return
				
			if(out_dir == ""):
				self.writeMessage("Must specify an output directory.\n\n")
				return
			
			
			hspec_files = []
			for hspec_file in os.listdir(hspec_dir):
				if hspec_file.endswith(".dat"):
					hspec_files.append(hspec_file)
					
			lid_files = []
			for lid_file in os.listdir(lid_dir):
				if lid_file.endswith(".las"):
					lid_files.append(lid_file)
			
			tc_files = []
			for tc_file in os.listdir(tc_dir):
				if tc_file.endswith(".shp"):
					tc_files.append(tc_file)
					
			self.writeMessage("the number of hspec files " + str(len(hspec_files)) + "\n\n");
			for hspec_file in hspec_files:
				self.writeMessage(hspec_file + "\n");
				
			first = 1
			for hspec_file in hspec_files:
				
				hyper_path  = hspec_dir + '/' + hspec_file[:hspec_file.rfind('.')]
				fline_path  = fline_dir + '/' + hspec_file[:hspec_file.rfind('.')]
				
				self.writeMessage('Hyper path: ' + hyper_path + '\n')
				self.writeMessage('Fline path: ' + fline_path + '\n')
				self.writeMessage('Py_Initialize\n')
				
				#
				# Builds a grid to store the z-values for each lidar point. Every cell
				# in the grid corresponds to a cell in the provided hyperspectral file.
				TCM_module.Initialize(hyper_path)
				
				#
				# grid the lidar points
				#
				# Populates the grid generated by TCM_module.Initialize with the
				# z-values of the lidar points found in the lidar files of the 
				# specified lidar directory.
				for lid_file in lid_files:
					self.writeMessage('    ' + lid_dir + "/" + lid_file + '\n')
					TCM_module.GridLidar(lid_dir + "/" + lid_file)
				
				for tc_file in tc_files:
					
					vector_path = tc_dir + '/' + tc_file
					
					self.writeMessage('    Vectr Path: ' + vector_path + '\n')
					self.writeMessage('    DetermineTreeCrownIntersection\n')
					#
					# determine intersection points
					TCM_module.DetermineTreeCrownIntersection(
						hyper_path,
						fline_path, 
						vector_path
					);
						
					root = out_dir + '/' + tc_file[:tc_file.rfind('.')]
					fl_fname = hspec_file[:hspec_file.rfind('.')]
					
					if not os.path.exists(root):
						os.makedirs(root)
						
					self.writeMessage('    Hyper path: ' + hyper_path + '\n')
					self.writeMessage('    Out dir: ' + root + '\n')
					self.writeMessage('    Fl fname: ' + fl_fname + '\n')
					
					self.writeMessage("    Py_HyperspectralExtraction\n")
					TCM_module.HyperspectralExtraction(
						chm_file,
						albedo,
						hyper_path,
						root,
						fl_fname,
						ssbyh,
						n_to_ss,
						output_met,
						output_ext
					);
					
					self.writeMessage("Py_TeardownPerTCFile\n")
					TCM_module.TeardownPerTCFile()
				
				self.writeMessage("Py_TeardownPerHypFile\n")
				TCM_module.TeardownPerHypFile()
				
				first = 0
				
			self.writeMessage("Done.\n\n")
				
		except:
			self.messageWindow.insert(Tkinter.END, str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			return
		
