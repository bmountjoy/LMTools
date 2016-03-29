

import Tkinter
import sys
import os
import datetime
import tkFileDialog
import TCD_module
import re


class Init:
	
	def __init__(self, master, clipBoard):
		
		self.clipBoard = clipBoard
	
		frame = Tkinter.Frame(master)
		frame.grid(row = 0, column = 0)
		
		###################### Pad Frame ##################################
		
		Tkinter.Frame(frame).grid(row = 0, column = 0, pady = 17)
		
		###################################################################
		###################### Input Frame ################################
		###################################################################
		
		input_frame = Tkinter.Frame(frame)
		input_frame.grid(row = 1, column = 0, sticky = Tkinter.NW)
		
		#grid row
		r = 0
		
		inTiffButton = Tkinter.Button(input_frame, text="Input TIFF", command = self.getInTiffFile);
		inTiffButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inTiffEntry = Tkinter.Entry(input_frame, width = 45)
		self.inTiffEntry.grid(row = r, column = 1)
		#self.inTiffEntry.insert(0, "/Users/Ben/Desktop/Sooke/day206/dem/CHM_Sooke_FLOAT.tif")
		
		r = r + 1
		
		inDirButton = Tkinter.Button(input_frame, text="Input Directory", command=self.getInDir);
		inDirButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inDirEntry = Tkinter.Entry(input_frame, width = 45)
		self.inDirEntry.grid(row = r, column = 1)
		self.inDirEntry.insert(0, "")
		
		r = r + 1
		
		###################################################################
		#################### Smoothing Frame ##############################
		###################################################################
		
		Tkinter.Label(input_frame, text = "Smoothing (pixels)").grid(row = r, column = 0, sticky = Tkinter.NW)
		
		smooth_frame = Tkinter.Frame(input_frame)
		smooth_frame.grid(row = r, column = 1, sticky = Tkinter.W)
		
		self.no_smooth = Tkinter.IntVar()
		Tkinter.Checkbutton(smooth_frame, variable = self.no_smooth).grid(row = 0, column = 0, sticky = Tkinter.W)
		Tkinter.Label(smooth_frame, text = "None").grid(row = 0, column = 1, sticky = Tkinter.W)
		
		
		self.mean_smooth = Tkinter.IntVar()
		Tkinter.Checkbutton(smooth_frame, variable = self.mean_smooth).grid(row = 1, column = 0, sticky = Tkinter.W)
		Tkinter.Label(smooth_frame, text = "3x3 Mean").grid(row = 1, column = 1, sticky = Tkinter.W)
		
		
		self.gaus3_smooth = Tkinter.IntVar()
		Tkinter.Checkbutton(smooth_frame, variable = self.gaus3_smooth).grid(row = 2, column = 0, sticky = Tkinter.W)
		Tkinter.Label(smooth_frame, text = "3x3 Guassian").grid(row = 2, column = 1, sticky = Tkinter.W)
		
		
		self.gaus5_smooth = Tkinter.IntVar()
		Tkinter.Checkbutton(smooth_frame, variable = self.gaus5_smooth).grid(row = 3, column = 0, sticky = Tkinter.W)
		Tkinter.Label(smooth_frame, text = "5x5 Guassian").grid(row = 3, column = 1, sticky = Tkinter.W)
		
		"""
		self.no_smooth = Tkinter.IntVar()
		Tkinter.Checkbutton(smooth_frame, variable = self.no_smooth).grid(row = 0, column = 0, sticky = Tkinter.W)
		Tkinter.Label(smooth_frame, text = "None").grid(row = 0, column = 1, sticky = Tkinter.W)

		self.mean_smooth = Tkinter.IntVar()
		Tkinter.Checkbutton(smooth_frame, variable = self.mean_smooth).grid(row = 0, column = 2, sticky = Tkinter.W)
		Tkinter.Label(smooth_frame, text = "3x3M").grid(row = 0, column = 3, sticky = Tkinter.W)
		
		
		self.gaus3_smooth = Tkinter.IntVar()
		Tkinter.Checkbutton(smooth_frame, variable = self.gaus3_smooth).grid(row = 0, column = 4, sticky = Tkinter.W)
		Tkinter.Label(smooth_frame, text = "3x3G").grid(row = 0, column = 5, sticky = Tkinter.W)
		
		
		self.gaus5_smooth = Tkinter.IntVar()
		Tkinter.Checkbutton(smooth_frame, variable = self.gaus5_smooth).grid(row = 0, column = 6, sticky = Tkinter.W)
		Tkinter.Label(smooth_frame, text = "5x5G").grid(row = 0, column = 7, sticky = Tkinter.W)
		"""
		
		r = r + 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 10)
		
		r = r + 1
		
		treetopLabel = Tkinter.Label(input_frame, text = "Treetops")
		treetopLabel.grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 5)
		
		r = r + 1
		
		outDirButton = Tkinter.Button(input_frame, text="Output Directory:", command = self.getOutDirectory)
		outDirButton.grid(row = r, column = 0, pady = 0, sticky = Tkinter.W)
		
		
		self.outDirEntry = Tkinter.Entry(input_frame, width = 45)
		self.outDirEntry.grid(row = r, column = 1)
		#self.outDirEntry.insert(0, "/Users/Ben/Desktop/Sooke/day206/treetops")
		
		r = r + 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 5)
		
		#
		#
		# Insert range from for treetops
		#
		
		r = r + 1

		noise_frame = Tkinter.Frame(input_frame)
		noise_frame.grid(row = r, column = 1, sticky = Tkinter.W)

		self.add_noise = Tkinter.IntVar()
		self.add_noise.set(1)
		Tkinter.Checkbutton(noise_frame, variable = self.add_noise).grid(row = 0, column = 0)
		Tkinter.Label(noise_frame, text = "Apply Noise to Treetop Heights").grid(row = 0, column = 1)

		r = r + 1
		
		Tkinter.Label(input_frame, text="Crown Size (pixels)").grid(row = r, column = 0, sticky = Tkinter.NW)
		
		###################################################################
		################ Treetop Range Frame ##############################
		###################################################################
		
		range_frame = Tkinter.Frame(input_frame)
		range_frame.grid(row = r, column = 1, sticky = Tkinter.W)
		
		#region frame row
		rf_r = 0
		
		range3Label = Tkinter.Label(range_frame, text="Range 3x3")
		range3Label.grid(row = rf_r, column = 0)
		
		self.run_ttr1 = Tkinter.IntVar()
		Tkinter.Checkbutton(range_frame, variable = self.run_ttr1).grid(row = rf_r, column = 1)
		
		self.r3minEntry = Tkinter.Entry(range_frame, width = 10)
		self.r3minEntry.grid(row = rf_r, column = 2)
		self.r3minEntry.insert(0, "3")
		
		self.r3maxEntry = Tkinter.Entry(range_frame, width = 10)
		self.r3maxEntry.grid(row = rf_r, column = 3)
		self.r3maxEntry.insert(0, "6")
		
		rf_r = rf_r + 1
		
		range5Label = Tkinter.Label(range_frame, text="Range 5x5")
		range5Label.grid(row = rf_r, column = 0)
		
		self.run_ttr2 = Tkinter.IntVar()
		Tkinter.Checkbutton(range_frame, variable = self.run_ttr2).grid(row = rf_r, column = 1)
		
		self.r5minEntry = Tkinter.Entry(range_frame, width = 10)
		self.r5minEntry.grid(row = rf_r, column = 2)
		self.r5minEntry.insert(0, "6")
		
		self.r5maxEntry = Tkinter.Entry(range_frame, width = 10)
		self.r5maxEntry.grid(row = rf_r, column = 3)
		self.r5maxEntry.insert(0, "35")
		
		rf_r = rf_r + 1
		
		range7Label = Tkinter.Label(range_frame, text="Range 7x7")
		range7Label.grid(row = rf_r, column = 0)
		
		self.run_ttr3 = Tkinter.IntVar()
		Tkinter.Checkbutton(range_frame, variable = self.run_ttr3).grid(row = rf_r, column = 1)        
		
		self.r7minEntry = Tkinter.Entry(range_frame, width = 10)
		self.r7minEntry.grid(row = rf_r, column = 2)
		self.r7minEntry.insert(0, "35")
		
		self.r7maxEntry = Tkinter.Entry(range_frame, width = 10)
		self.r7maxEntry.grid(row = rf_r, column = 3)
		self.r7maxEntry.insert(0, "100")
		
		rf_r = rf_r + 1
		
		range9Label = Tkinter.Label(range_frame, text="Range 9x9")
		range9Label.grid(row = rf_r, column = 0)
		
		self.run_ttr9 = Tkinter.IntVar()
		Tkinter.Checkbutton(range_frame, variable = self.run_ttr9).grid(row = rf_r, column = 1)        
		
		self.r9minEntry = Tkinter.Entry(range_frame, width = 10)
		self.r9minEntry.grid(row = rf_r, column = 2)
		self.r9minEntry.insert(0, "35")
		
		self.r9maxEntry = Tkinter.Entry(range_frame, width = 10)
		self.r9maxEntry.grid(row = rf_r, column = 3)
		self.r9maxEntry.insert(0, "100")
		
		rf_r = rf_r + 1
		
		range11Label = Tkinter.Label(range_frame, text="Range 11x11")
		range11Label.grid(row = rf_r, column = 0)
		
		self.run_ttr11 = Tkinter.IntVar()
		Tkinter.Checkbutton(range_frame, variable = self.run_ttr11).grid(row = rf_r, column = 1)        
		
		self.r11minEntry = Tkinter.Entry(range_frame, width = 10)
		self.r11minEntry.grid(row = rf_r, column = 2)
		self.r11minEntry.insert(0, "35")
		
		self.r11maxEntry = Tkinter.Entry(range_frame, width = 10)
		self.r11maxEntry.grid(row = rf_r, column = 3)
		self.r11maxEntry.insert(0, "100")
		
		
		
		###################################################################
		################ Treetop Metrics Processing #######################
		###################################################################
		
		r = r + 1
		
		brLabel = Tkinter.Label(input_frame, text = "Apply Treetop Biometrics", pady = 5)
		brLabel.grid(row = r, column = 0, columnspan = 2, sticky = Tkinter.W)
		
		r = r + 1
		
		refFileButton = Tkinter.Button(input_frame, text = "Reference File", command = self.get_ref_file)
		refFileButton.grid(row = r, column = 0, padx = 5, sticky = Tkinter.W)
		
		self.refFileEntry = Tkinter.Entry(input_frame, width = 42)
		self.refFileEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		
		r = r + 1
		
		tcDirButton = Tkinter.Button(input_frame, text = "Treetop Directory", command = self.get_tt_dir)
		tcDirButton.grid(row = r, column = 0, padx = 5, sticky = Tkinter.W)
		
		self.ttDirEntry = Tkinter.Entry(input_frame, width = 42)
		self.ttDirEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		
		r = r + 1
		
		ttmDirButton = Tkinter.Button(input_frame, text = "Output Metrics Directory", command = self.get_ttm_dir)
		ttmDirButton.grid(row = r, column = 0, padx = 5, sticky = Tkinter.W)
		
		self.ttmDirEntry = Tkinter.Entry(input_frame, width = 42)
		self.ttmDirEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		
		r = r + 1
		
		runTtmButton = Tkinter.Button(input_frame, text = "Run Treetop Metrics", command = self.run_treetop_metrics)
		runTtmButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		
		###################################################################
		################# Treetop Crown Delineation #######################
		###################################################################
		
		r = r + 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 10)
		
		r = r + 1
		
		Tkinter.Label(input_frame, text="Tree Crowns").grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 5)
		
		r = r + 1
		
		inputTreetopButton = Tkinter.Button(input_frame, text="Treetops (SHP):", command = self.getInTreetops)
		inputTreetopButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inputTreetopEntry = Tkinter.Entry(input_frame, width =45)
		self.inputTreetopEntry.grid(row = r, column = 1)
		
		r = r + 1
		
		outTreeCrownDirButton = Tkinter.Button(input_frame, text="Output Directory:", command = self.getOutTreeCrownDirectory)
		outTreeCrownDirButton.grid(row = r, column = 0, pady = 0, sticky = Tkinter.W)
		
		self.outTreeCrownDirEntry = Tkinter.Entry(input_frame, width = 45)
		self.outTreeCrownDirEntry.grid(row = r, column = 1)
		
		#
		# Insert radius and hlc percentage info
		#
		r = r + 1
		
		# Add sorting here
		sort_frame = Tkinter.Frame(input_frame)
		sort_frame.grid(row = r, column = 1, sticky = Tkinter.W)
		
		self.sort_asc = Tkinter.IntVar()
		Tkinter.Checkbutton(sort_frame, variable = self.sort_asc).grid(row = 0, column = 0)
		Tkinter.Label(sort_frame, text = "Sort Ascending").grid(row = 0, column = 1)

		self.sort_dsc = Tkinter.IntVar()
		Tkinter.Checkbutton(sort_frame, variable = self.sort_dsc).grid(row = 0, column = 2)
		Tkinter.Label(sort_frame, text = "Sort Descending").grid(row = 0, column = 3)

		r = r + 1

## useful >>
		shape_frame = Tkinter.Frame(input_frame)
		shape_frame.grid(row = r, column = 1, sticky = Tkinter.W)
		
		self.shape_crown = Tkinter.IntVar()
		Tkinter.Checkbutton(shape_frame, variable = self.shape_crown).grid(row = 0, column = 0)
		Tkinter.Label(shape_frame, text = "Shape Crown").grid(row = 0, column = 1)
		
		r = r + 1
## useful <<

		Tkinter.Label(input_frame, text = "Crown Info:").grid(row = r, column = 0, sticky = Tkinter.N)
		
		
		tcInfoFrame = Tkinter.Frame(input_frame)
		tcInfoFrame.grid(row = r, column = 1, sticky = Tkinter.W)
		
		rr = 0
		
		Tkinter.Label(tcInfoFrame, text="Height (m)").grid(row = rr, column = 0)
		Tkinter.Label(tcInfoFrame, text="Percentage [0,1]").grid(row = rr, column = 1)
		Tkinter.Label(tcInfoFrame, text="Radius (m)").grid(row = rr, column = 2)
		
		rr = rr + 1
		
		h1_frame = Tkinter.Frame(tcInfoFrame)
		h1_frame.grid(row = rr, column = 0)
		
		self.run_tcr1 = Tkinter.IntVar()
		Tkinter.Checkbutton(h1_frame, variable = self.run_tcr1).grid(row = 0, column = 0)
		
		self.h1MinEntry = Tkinter.Entry(h1_frame, width = 5)
		self.h1MinEntry.grid(row = 0, column = 1)
		self.h1MinEntry.insert(0, "3")
		Tkinter.Label(h1_frame, text=" - ").grid(row = 0, column = 2)
		self.h1MaxEntry = Tkinter.Entry(h1_frame, width = 5)
		self.h1MaxEntry.grid(row = 0, column = 3)
		self.h1MaxEntry.insert(0, "6")
		
		self.h1PercentageEntry = Tkinter.Entry(tcInfoFrame, width =10)
		self.h1PercentageEntry.grid(row = rr, column = 1)
		self.h1PercentageEntry.insert(0, "0.65")
		
		self.h1RadiusEntry = Tkinter.Entry(tcInfoFrame, width = 10)
		self.h1RadiusEntry.grid(row = rr, column = 2)
		self.h1RadiusEntry.insert(0, "9")
		
		rr = rr + 1
		
		h2_frame = Tkinter.Frame(tcInfoFrame)
		h2_frame.grid(row = rr, column = 0)
		
		self.run_tcr2 = Tkinter.IntVar()
		Tkinter.Checkbutton(h2_frame, variable = self.run_tcr2).grid(row = 0, column = 0)
		
		self.h2MinEntry = Tkinter.Entry(h2_frame, width = 5)
		self.h2MinEntry.grid(row = 0, column = 1)
		self.h2MinEntry.insert(0, "6")
		Tkinter.Label(h2_frame, text=" - ").grid(row = 0, column = 2)
		self.h2MaxEntry = Tkinter.Entry(h2_frame, width = 5)
		self.h2MaxEntry.grid(row = 0, column = 3)
		self.h2MaxEntry.insert(0, "35")
		
		self.h2PercentageEntry = Tkinter.Entry(tcInfoFrame, width =10)
		self.h2PercentageEntry.grid(row = rr, column = 1)
		self.h2PercentageEntry.insert(0, "0.65")
		
		self.h2RadiusEntry = Tkinter.Entry(tcInfoFrame, width = 10)
		self.h2RadiusEntry.grid(row = rr, column = 2)
		self.h2RadiusEntry.insert(0, "15")
		
		rr = rr + 1
		
		h3_frame = Tkinter.Frame(tcInfoFrame)
		h3_frame.grid(row = rr, column = 0)
		
		self.run_tcr3 = Tkinter.IntVar()
		Tkinter.Checkbutton(h3_frame, variable = self.run_tcr3).grid(row = 0, column = 0)
		
		self.h3MinEntry = Tkinter.Entry(h3_frame, width = 5)
		self.h3MinEntry.grid(row = 0, column = 1)
		self.h3MinEntry.insert(0, "35")
		Tkinter.Label(h3_frame, text=" - ").grid(row = 0, column = 2)
		self.h3MaxEntry = Tkinter.Entry(h3_frame, width = 5)
		self.h3MaxEntry.grid(row = 0, column = 3)
		self.h3MaxEntry.insert(0, "100")
		
		self.h3PercentageEntry = Tkinter.Entry(tcInfoFrame, width =10)
		self.h3PercentageEntry.grid(row = rr, column = 1)
		self.h3PercentageEntry.insert(0, "0.65")
		
		self.h3RadiusEntry = Tkinter.Entry(tcInfoFrame, width = 10)
		self.h3RadiusEntry.grid(row = rr, column = 2)
		self.h3RadiusEntry.insert(0, "21")
		
		#
		# End
		#
		
		
		###################################################################
		###################### Message Frame ##############################
		###################################################################
		
		## status frame
		messageFrame = Tkinter.Frame(master)
		messageFrame.grid(row = 0, column = 1, sticky = Tkinter.N, padx = 10, pady =10)
		
		resultLabel = Tkinter.Label(messageFrame, text = "Status Message...")
		resultLabel.grid(row = 0, column = 0, sticky = Tkinter.NW)
		
		self.textFrame = Tkinter.Frame(messageFrame)
		self.scroll = Tkinter.Scrollbar(self.textFrame)
		
		self.messageWindow = Tkinter.Text(self.textFrame, height = 37, width = 80, background = 'white', borderwidth = 1, relief = Tkinter.RIDGE, yscrollcommand=self.scroll.set)
		self.messageWindow.pack(side=Tkinter.LEFT);
		
		self.scroll.pack(side=Tkinter.RIGHT, fill=Tkinter.Y)
		self.scroll.config(command=self.messageWindow.yview)
		self.textFrame.grid(row = 1, column = 0);
		
		
		###################################################################
		###################### Execute Frame ##############################
		###################################################################
		
		exe_frame = Tkinter.Frame(master)
		exe_frame.grid(row = 4, column = 0, sticky = Tkinter.W)
		
		treetopButton = Tkinter.Button(exe_frame, text="Treetops", command = self.start)
		treetopButton.grid(row = 0, column = 0, sticky = Tkinter.SW, pady = 10)
		
		treecrownButton = Tkinter.Button(exe_frame, text="Treecrown", command = self.treeCrownDelineation)
		treecrownButton.grid(row = 0, column = 1, sticky = Tkinter.SW, pady = 10)
		
		
	def checkSmoothing(self):
		
		count = 0
		vars = [self.no_smooth.get(), self.mean_smooth.get(), self.gaus3_smooth.get(), self.gaus5_smooth.get()]
		
		for var in vars:
			if var == 1:
				count = count + 1
		
		if count == 0:
			self.writeMessage("No smoothing strategy selected!\n")
			return 0
		elif count > 1:
			self.writeMessage("Only select one smoothing strategy!\n")
			return 0
		
		return 1

	def checkSorting(self):
		if self.sort_asc.get() == 1 and self.sort_dsc.get() == 1:
			self.writeMessage("Error: Select at most 1 sorting strategy")
			return 0
		return 1
		
	def getSmoothingStrategy(self):
		
		count = 1
		vars  = [self.no_smooth.get(), self.mean_smooth.get(), self.gaus3_smooth.get(), self.gaus5_smooth.get()]
		
		for var in vars:
			if var == 1:
				return count
			count = count + 1
			
		return 0
	
	
	#
	# For range check to work user must use consective ranges.
	#
	def rangeCheck(self, run_h1, h1_min, h1_max, run_h2, h2_min, h2_max, run_h3, h3_min, h3_max, run_h9, h9_min, h9_max, run_h11, h11_min, h11_max):
		
		heights = []
		
		#singles
		if run_h1 :
			heights.append(h1_min)
			heights.append(h1_max)
		
		if run_h2 :
			heights.append(h2_min)
			heights.append(h2_max)
			
		if run_h3 :
			heights.append(h3_min)
			heights.append(h3_max)
			
		if run_h9 :
			heights.append(h9_min)
			heights.append(h9_max)
			
		if run_h11 :
			heights.append(h11_min)
			heights.append(h11_max)
			
		i = 0
		while i < len(heights):
			if heights[i] > heights[i+1]:
				return 0
			i = i + 2
			
		return 1
		
		
	#
	# For range check to work user must use consective ranges.
	#
	def rangeCheckTC(self, run_h1, h1_min, h1_max, run_h2, h2_min, h2_max, run_h3, h3_min, h3_max):
		
		heights = []
		
		#singles
		if run_h1 :
			heights.append(h1_min)
			heights.append(h1_max)
		
		if run_h2 :
			heights.append(h2_min)
			heights.append(h2_max)
			
		if run_h3 :
			heights.append(h3_min)
			heights.append(h3_max)
			
		i = 0
		while i < len(heights):
			if heights[i] > heights[i+1]:
				return 0
			i = i + 2
			
		return 1
		
		
	def get_ref_file(self):
		ref_file = tkFileDialog.askopenfilename()
		if(ref_file != ""):
			self.refFileEntry.delete(0, Tkinter.END)
			self.refFileEntry.delete(0,Tkinter.END)
			self.refFileEntry.insert(0,ref_file)
			
	def get_tt_dir(self):
		indir = tkFileDialog.askdirectory()
		if(indir != ""):
			self.ttDirEntry.delete(0, Tkinter.END)
			self.ttDirEntry.delete(0, Tkinter.END)
			self.ttDirEntry.insert(0, indir)
			
	def get_ttm_dir(self):
		indir = tkFileDialog.askdirectory()
		if(indir != ""):
			self.ttmDirEntry.delete(0, Tkinter.END)
			self.ttmDirEntry.delete(0, Tkinter.END)
			self.ttmDirEntry.insert(0, indir)
			
			
	def getInTiffFile(self):
		intiff = tkFileDialog.askopenfilename()
		if(intiff != ""):
			self.inDirEntry.delete(0, Tkinter.END)
			self.inTiffEntry.delete(0,Tkinter.END)
			self.inTiffEntry.insert(0,intiff)
			
	def getInDir(self):
		indir = tkFileDialog.askdirectory()
		if(indir != ""):
			self.inTiffEntry.delete(0, Tkinter.END)
			self.inDirEntry.delete(0, Tkinter.END)
			self.inDirEntry.insert(0, indir)
			
	def getInTreetops(self):
		inshp = tkFileDialog.askopenfilename()
		if(inshp != ""):
			self.inputTreetopEntry.delete(0, Tkinter.END)
			self.inputTreetopEntry.insert(0, inshp)
			
	def getOutDirectory(self):
		outdir = tkFileDialog.askdirectory()
		if(outdir != ""):
			self.outDirEntry.delete(0,Tkinter.END)
			self.outDirEntry.insert(0,outdir)
			
	def getOutTreeCrownDirectory(self):
		outdir = tkFileDialog.askdirectory()
		if(outdir != ""):
			self.outTreeCrownDirEntry.delete(0, Tkinter.END)
			self.outTreeCrownDirEntry.insert(0, outdir)
			
	def writeMessage(self, text):
		self.messageWindow.insert(Tkinter.END, text)
		self.messageWindow.update()
		self.messageWindow.yview(Tkinter.END)
		
	def handle_exception(self, exception):
		self.writeMessage("\n" + str(exception[0]) + ": " + str(exception[1]) + "\n\n")
		
	
	def createTreetopLogFile(self,out_file, input_chm, smooth_type,\
		range3,  r3_min,  r3_max,\
		range5,  r5_min,  r5_max,\
		range7,  r7_min,  r7_max,\
		range9,  r9_min,  r9_max,\
		range11, r11_min, r11_max):
	
	
		S = ["None", "3x3 Mean", "3x3 Gaussian", "5x5 Gaussian"]
		L = [[range3,"3x3",r3_min,r3_max],[range5,"5x5",r5_min,r5_max],[range7,"7x7",r7_min,r7_max],[range9,"9x9",r9_min,r9_max],[range11,"11x11",r11_min,r11_max]]
		
		
		fp = open(out_file, 'w')
		
		fp.write("Input CHM: " + input_chm + "\n")
		fp.write("Smoothing strategy: " + str(S[smooth_type-1])+"\n")
		
		for info in L:
			if info[0] == 1:
				fp.write("Range " + info[1] + " " + str(info[2]) + " - " + str(info[3]) + "\n")
				
		fp.close()
		
	
	def createTreeCrownLogFile(self, out_file, input_chm, treetop_file, smooth_type,\
		h1_run, h1_min, h1_max,\
		h2_run, h2_min, h2_max,\
		h3_run, h3_min, h3_max,\
		perc_1, perc_2, perc_3, rad_1, rad_2, rad_3,\
		shape_crown):
	
	
		smoothing_strats = ["None", "3x3 Mean", "3x3 Gaussian", "5x5 Gaussian"]
		
		
		fp = open(out_file, 'w')
		
		fp.write("Input CHM: " + input_chm+"\n")
		fp.write("Treetop file: " + treetop_file+"\n")
		fp.write("Smoothing strategy: " + str(smoothing_strats[smooth_type-1])+"\n")
		
		if shape_crown == 1:
			fp.write("Crown Shaped: yes\n")
		else:
			fp.write("Crown Shaped: no\n")
			
		L = [[h1_run,h1_min,h1_max,perc_1,rad_1],[h2_run,h2_min,h2_max,perc_2,rad_2],[h3_run,h3_min,h3_max,perc_3,rad_3]]
		
		for info in L:
			if info[0] == 1:
				fp.write("Height Range " + str(info[1]) + " - " + str(info[2]) + "\n")
				fp.write("\Percentage: "+ str(info[3])+ "\n")
				fp.write("\tCrown Radius: "+ str(info[4])+ "\n")
				
		fp.close()
			
		
		
	
	##
	## ####### TREETOP METRICS
	##
	def run_treetop_metrics(self):
			
		tt_dir = self.ttDirEntry.get()
		ttm_dir = self.ttmDirEntry.get()
		ref_file = self.refFileEntry.get()
		
		if tt_dir == "":
			self.writeMessage("Error. Input treetop directory is null.\n")
			return
			
		if ttm_dir == "":
			self.writeMessage("Error. Output treetop metrics directory is null.\n")
			return
			
		if ref_file == "":
			self.writeMessage("Error. Reference file is null.\n")
			return
		
		ttm_file = ttm_dir + "/treetop-metrics.tiff"
		
		#init procedure
		self.writeMessage(ref_file + "\n")
		TCD_module.treetopMetricsInit(ref_file);
		
		for tt_file in os.listdir(tt_dir):
			
			if tt_file.endswith(".shp"):
				tt_path = tt_dir + "/" + tt_file[:tt_file.find(".shp")]
				self.writeMessage(tt_path)
				self.writeMessage("\t" + tt_path + "\n")
				TCD_module.treetopMetricsGrid(tt_path);
			
		#finsih
		self.writeMessage(ttm_file + "\n");
		TCD_module.treetopMetricsFinish(ttm_file);
		self.writeMessage("Done!\n\n")
	
	##
	## ###### TREE CROWNS
	##
	def treeCrownDelineation(self):
		
		try:
			
			if self.checkSmoothing() == 0:
				return
			if self.checkSorting() == 0:
				return
				
			in_shp = self.inputTreetopEntry.get()
			in_tif = self.inTiffEntry.get()
			tc_out = self.outTreeCrownDirEntry.get()
			
			h1_run = int(self.run_tcr1.get())
			h2_run = int(self.run_tcr2.get())
			h3_run = int(self.run_tcr3.get())
			
			h1_min = float(self.h1MinEntry.get())
			h1_max = float(self.h1MaxEntry.get())
			h2_min = float(self.h2MinEntry.get())
			h2_max = float(self.h2MaxEntry.get())
			h3_min = float(self.h3MinEntry.get())
			h3_max = float(self.h3MaxEntry.get())
			
			perc_1 = float(self.h1PercentageEntry.get())
			perc_2 = float(self.h2PercentageEntry.get())
			perc_3 = float(self.h3PercentageEntry.get())
			
			rad_1  = int(self.h1RadiusEntry.get())
			rad_2  = int(self.h2RadiusEntry.get())
			rad_3  = int(self.h3RadiusEntry.get())

			sort_type = 0
			if self.sort_asc.get() == 1:
				sort_type = 1
			elif self.sort_dsc.get() == 1:
				sort_type = 2
			
			shape_crown = self.shape_crown.get()
			
			
			
			#
			# Error checking
			#
			if(self.rangeCheckTC(h1_run, h1_min, h1_max, h2_run, h2_min, h2_max, h3_run, h3_min, h3_max) == 0):
				self.writeMessage("Error. Bad ranges. \n\n")
				return
				
			if(in_shp == "" or in_tif == ""):
				self.writeMessage("Error. Specify input shape and tiff file..\n\n")
				return
				
			if(tc_out == ""):
				self.writeMessage("Error. Must specify output directory.\n\n")
				return
			
			
			#
			# get smoothing strategy
			#
			smooth_type = self.getSmoothingStrategy()
			date_time_string = datetime.datetime.now().strftime("_%m_%d_%y_%H_%M_%S")
			
			in_shp   = in_shp[:in_shp.rfind(".")]
			out_file = tc_out + "/TC_" + in_tif[in_tif.rfind("/")+1:]
			out_shp  = tc_out + "/TC_" + in_tif[in_tif.rfind("/")+1:in_tif.rfind(".")] + date_time_string
			
			self.createTreeCrownLogFile(out_shp + ".txt", in_tif, in_shp, smooth_type,\
				h1_run, h1_min, h1_max,\
				h2_run, h2_min, h2_max,\
				h3_run, h3_min, h3_max,\
				perc_1, perc_2, perc_3, rad_1, rad_2, rad_3,\
				shape_crown)
				
			self.writeMessage(in_shp + "\n")
			self.writeMessage(in_tif + "\n")
			self.writeMessage(tc_out + "\n")
			self.writeMessage(out_file + "\n")
			self.writeMessage(out_shp + "\n\n")
			
			TCD_module.treeCrownDelineation(in_shp, out_shp, in_tif, out_file, 
				h1_run, h1_min, h1_max, 
				h2_run, h2_min, h2_max,
				h3_run, h3_min, h3_max,
				perc_1, perc_2, perc_3, rad_1, rad_2, rad_3,
				smooth_type, shape_crown, sort_type)
			
			self.writeMessage("Done!\n\n")
			
				
		except:
			self.handle_exception(sys.exc_info())
	
	##
	## ########### TREETOPS
	##
	def start(self):
		
		try:
				
			inTiff  = self.inTiffEntry.get()
			inDir   = self.inDirEntry.get()
			outDir  = self.outDirEntry.get()
			
			r1run   = int(self.run_ttr1.get())
			r2run   = int(self.run_ttr2.get())
			r3run   = int(self.run_ttr3.get())
			r9run   = int(self.run_ttr9.get())
			r11run  = int(self.run_ttr11.get())
			
			r3min   = float(self.r3minEntry.get())
			r3max   = float(self.r3maxEntry.get())
			r5min   = float(self.r5minEntry.get())
			r5max   = float(self.r5maxEntry.get())
			r7min   = float(self.r7minEntry.get())
			r7max   = float(self.r7maxEntry.get())
			r9min   = float(self.r9minEntry.get())
			r9max   = float(self.r9maxEntry.get())
			r11min  = float(self.r11minEntry.get())
			r11max  = float(self.r11maxEntry.get())

			add_noise = int(self.add_noise.get())
			
			
			#
			# Error checking
			#
			if self.checkSmoothing() == 0:
				return
				
			if(self.rangeCheck(r1run, r3min, r3max, r2run, r5min, r5max, r3run, r7min, r7max, r9run, r9min, r9max, r11run, r11min, r11max) == 0):
				self.writeMessage("Error. Bad ranges. \n\n")
				return
				
			if(inTiff == "" and inDir == ""):
				self.writeMessage("Error. Must specify an input file or directory.\n\n")
				return
				
			if(outDir == ""):
				self.writeMessage("Error. Must specify output directory.\n\n")
				return
				
			#
			# get smoothing strategy + date/time string
			#
			smooth_strat = self.getSmoothingStrategy()
			
			#
			# process specified tiff file
			#
			if(inTiff != ""):
				self.run_file(inTiff, outDir, r1run, r3min, r3max, r2run, r5min, r5max, r3run, r7min, r7max, r9run, r9min, r9max, r11run, r11min, r11max, smooth_strat, add_noise)
				
			#
			# process all tiff's in specified directory
			#
			else:
				
				prog    = 0
				count   = 0
				regex   = re.compile('[-,_, ,0-9,a-z,A-Z]+\.tiff?$')
				dirList = os.listdir(inDir)
				
				for f in dirList:
					if regex.match(f):
						count = count + 1
				
				for entry in dirList:
					if regex.match(entry):
						
						prog = prog + 1
						self.writeMessage("Processing " + str(prog) + " of " + str(count) + "\n")
						
						tiff = inDir + "/" + entry
						self.run_file(tiff, outDir, r1run, r3min, r3max, r2run, r5min, r5max, r3run, r7min, r7max, r9run, r9min, r9max, r11run, r11min, r11max, smooth_strat, add_noise)
			
			self.writeMessage("Done.\n\n")
				
		except:
			self.handle_exception(sys.exc_info())
			
	def run_file(self, inTiff, outdir, r1run, r3min, r3max, r2run, r5min, r5max, r3run, r7min, r7max, r9run, r9min, r9max, r11run, r11min, r11max, smooth_strat, add_noise):
		
		try:
			#
			# Generate base name for output shape file
			#
			a = inTiff.rfind("/")
			b = inTiff.rfind(".")
			date_time_string = datetime.datetime.now().strftime("_%m_%d_%y_%H_%M_%S")
			outfile = outdir + "/TT_" + inTiff[a+1:b] + date_time_string
			
			
			self.createTreetopLogFile(outfile + ".txt", inTiff, smooth_strat,\
				r1run,  r3min,  r3max,\
				r2run,  r5min,  r5max,\
				r3run,  r7min,  r7max,\
				r9run,  r9min,  r9max,\
				r11run, r11min, r11max)
			
			#
			# Remove output shape file if it already exists
			#
			postfix = [".shp", ".shx", ".dbf"]
			for pf in postfix:
				file = outfile + pf
				if os.path.exists(file):
					os.remove(file)
			
			self.writeMessage("Input:  " + inTiff + "\n")
			self.writeMessage("Output: " + outfile + "\n")
			
			TCD_module.findTreeTops(inTiff, outfile, r1run, r3min, r3max, r2run, r5min, r5max, r3run, r7min, r7max, r9run, r9min, r9max, r11run, r11min, r11max, 0, smooth_strat, add_noise)
			
			
			#########################################################
			#                 Disable Partitioning                  #
			#########################################################
			#
			#imgsize = TCD_module.getImageSize(inTiff)
			#self.writeMessage("Image Size: " + str(imgsize))
			
			#if(imgsize <= 1000000000):
				
			#	TCD_module.findTreeTops(inTiff, outfile, r1run, r3min, r3max, r2run, r5min, r5max, r3run, r7min, r7max, r9run, r9min, r9max, r11run, r11min, r11max, 0, smooth_strat)
			#else:
			#	self.run_large(inTiff, outfile, r1run, r3min, r3max, r2run, r5min, r5max, r3run, r7min, r7max, r9run, r9min, r9max, r11run, r11min, r11max, smooth_strat)
				
		except:
			self.handle_exception(sys.exc_info())
	
	def run_large(self, inTiff, outfile, r1run, r3min, r3max, r2run, r5min, r5max, r3run, r7min, r7max, r9run, r9min, r9max, r11run, r11min, r11max, smooth_strat):
		
		try:
			partdir = inTiff[:inTiff.rfind(".")] + "_parts"
			
			self.writeMessage("\nPartition Directory:\n\t" + partdir + "\n")
			                                        
			if not os.path.exists(partdir):
				os.mkdir(partdir)
			
			self.writeMessage("Partitioning:\n\t" + inTiff + " ...\n\n")
			
			TCD_module.partitionTiff(inTiff, partdir)
			
			regex   = re.compile('[0-9]+_[0-9]+\.tiff?$')
			dirList = os.listdir(partdir)
			
			for part in dirList:
				if regex.match(part):
					tiff = partdir + "/" + part
					self.writeMessage("\t" + tiff + "\n");
					TCD_module.findTreeTops(tiff, outfile, r1run, r3min, r3max, r2run, r5min, r5max, r3run, r7min, r7max, r9run, r9min, r9max, r11run, r11min, r11max, 1, smooth_strat)
						
		except:
			self.handle_exception(sys.exc_info())
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
