

import Tkinter
import sys
import os
from os import path
import tkFileDialog
import TCR_module


class Init:
	
	def __init__(self, master, clipBoard):
		
		self.clipBoard = clipBoard
	
		frame = Tkinter.Frame(master)
		frame.grid(row = 0, column = 0)
		
		###################### Pad Frame ##################################
		
		padframe1 = Tkinter.Frame(frame)
		padframe1.grid(row = 0, column = 0, pady = 16)
		
		#emptyLabel = Tkinter.Label(padframe1, text = "")
		#emptyLabel.grid(row = 0, column = 0)
		
		###################################################################
		###################### Input Frame ################################
		###################################################################
		
		input_frame = Tkinter.Frame(frame)
		input_frame.grid(row = 1, column = 0)
		
		#grid row
		r = 0
		
		##
		## added new button to ask for chm dir instead a single file
		##
		inChmDirButton = Tkinter.Button(input_frame, text="CHM Dir", command = self.getInChmDir);
		inChmDirButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inChmDirEntry = Tkinter.Entry(input_frame, width = 45)
		self.inChmDirEntry.grid(row = r, column = 1)
		#self.inChmDirEntry.insert(0, "/Users/Ben/Desktop/uwr_test/chm")
		
		
		r = r + 1
		
		inShpFileButton = Tkinter.Button(input_frame, text="Input Shapefile", command = self.getInShpFile)
		inShpFileButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inShpFileEntry = Tkinter.Entry(input_frame, width = 45)
		self.inShpFileEntry.grid(row = r, column = 1)
		self.inShpFileEntry.delete(0, Tkinter.END)
		#self.inShpFileEntry.insert(0, "/Users/Ben/Desktop/Proj_Strategic_UWR/Archive/PointCanopyIntersectionMerge.sbx")
		
		r = r + 1
		
		outFileButton = Tkinter.Button(input_frame, text="Output Directory:", command = self.getOutDirectory)
		outFileButton.grid(row = r, column = 0, pady = 10, sticky = Tkinter.W)
		
		self.outDirEntry = Tkinter.Entry(input_frame, width = 45)
		self.outDirEntry.grid(row = r, column = 1)
		self.outDirEntry.delete(0, Tkinter.END)
		#self.outDirEntry.insert(0, "/Users/Ben/Desktop/uwr_test/chm_tcr_dbio")
		
		
		###################### Pad Frame ##################################
		
		padframe2 = Tkinter.Frame(frame)
		padframe2.grid(row = 2, column = 0, pady = 4)
		
		###################################################################
		###################### Region Frame ###############################
		###################################################################
		
		region_frame = Tkinter.Frame(frame)
		region_frame.grid(row = 3, column = 0, sticky = Tkinter.W)
		
		#grid row
		r = 0
		
		titleLabel = Tkinter.Label(region_frame, text = "Region Parameters")
		titleLabel.grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1

		minEastingLabel = Tkinter.Label(region_frame, text = "Min Easting")
		minEastingLabel.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.minEastingEntry = Tkinter.Entry(region_frame) 
		self.minEastingEntry.grid(row = r, column = 1)
		#self.minEastingEntry.insert(0, '1089323.805')
		
		r = r + 1
		
		minNorthingLabel = Tkinter.Label(region_frame, text = "Min Northing")
		minNorthingLabel.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.minNorthingEntry = Tkinter.Entry(region_frame) 
		self.minNorthingEntry.grid(row = r, column = 1)
		#self.minNorthingEntry.insert(0, '455409.75')
		
		r = r + 1
		
		blockSizeLabel = Tkinter.Label(region_frame, text = "Block Size")
		blockSizeLabel.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.blockSizeEntry = Tkinter.Entry(region_frame)
		self.blockSizeEntry.grid(row = r, column = 1)
		#self.blockSizeEntry.insert(0, '500')
		
		###################################################################
		######################### CCF Extras ##############################
		###################################################################
		
		r = r + 1
		
		self.runCcf = Tkinter.IntVar()
		self.ccfCheckbutton = Tkinter.Checkbutton(region_frame, text = "Compute difference CCF ?", variable = self.runCcf)
		self.ccfCheckbutton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		outResolutionLabel = Tkinter.Label(region_frame, text = "Output Resolution")
		outResolutionLabel.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.outResolutionEntry = Tkinter.Entry(region_frame)
		self.outResolutionEntry.grid(row = r, column = 1)
		self.outResolutionEntry.insert(0, '20')
		
		r = r + 1
		
		heightThresholdLabel = Tkinter.Label(region_frame, text = "Height Threshold")
		heightThresholdLabel.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.heightThresholdEntry = Tkinter.Entry(region_frame)
		self.heightThresholdEntry.grid(row = r, column = 1)
		self.heightThresholdEntry.insert(0, '2')
		
		r = r + 1
		
		outCCFDirButton = Tkinter.Button(region_frame, text="Output CCF Directory", command = self.getOutCCFDir);
		outCCFDirButton.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		
		self.outCCFDirEntry = Tkinter.Entry(region_frame, width = 45)
		self.outCCFDirEntry.grid(row = r, column = 1)
		#self.outCCFDirEntry.insert(0, "/Users/Ben/Desktop/uwr_test/diff_bio")
		
		
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
		
		self.messageWindow = Tkinter.Text(self.textFrame, height = 14, width = 80, background = 'white', borderwidth = 1, relief = Tkinter.RIDGE, yscrollcommand=self.scroll.set)
		self.messageWindow.pack(side=Tkinter.LEFT);
		
		self.scroll.pack(side=Tkinter.RIGHT, fill=Tkinter.Y)
		self.scroll.config(command=self.messageWindow.yview)
		self.textFrame.grid(row = 1, column = 0);
		
		
		###################################################################
		###################### Execute Frame ##############################
		###################################################################
		
		exe_frame = Tkinter.Frame(master)
		exe_frame.grid(row = 4, column = 0, sticky = Tkinter.W)
		
		##
		## copyImageButtonn has been modified to call start and not copy
		## currently the input chm file field does nothing
		##
		
		copyImageButton = Tkinter.Button(exe_frame, text="Remove Tree Crowns", command = self.start)
		copyImageButton.grid(row = 0, column = 0, sticky = Tkinter.SW, pady = 10)
		
	def updateRegion(self):
		
		try:
			indir 	 = self.inChmDirEntry.get()
			
			datafile = open(indir + "/metadata.txt", "r")
			
			east 	 = datafile.readline().rstrip('\n')
			north 	 = datafile.readline().rstrip('\n')
			blocksize= datafile.readline().rstrip('\n')
			
			self.minEastingEntry.delete(0, Tkinter.END)
			self.minEastingEntry.insert(0, east)
			self.minNorthingEntry.delete(0, Tkinter.END)
			self.minNorthingEntry.insert(0, north)
			self.blockSizeEntry.delete(0, Tkinter.END)
			self.blockSizeEntry.insert(0, blocksize)
			
		except:
			self.writeMessage(str(sys.exc_info()[1]) + "\n\n")
			return
		
	def getOutCCFDir(self):
		
		outdir = tkFileDialog.askdirectory()
		
		if(outdir != ""):
			self.outCCFDirEntry.delete(0, Tkinter.END)
			self.outCCFDirEntry.insert(0, outdir)
		
	def getInChmDir(self):
		
		
		indir = tkFileDialog.askdirectory()
		
		if(indir != ""):
			self.inChmDirEntry.delete(0,Tkinter.END)
			self.inChmDirEntry.insert(0,indir)
			self.updateRegion()
			
			
	def getInShpFile(self):
		
		inshp = tkFileDialog.askopenfilename()
		
		if(inshp != ""):
			self.inShpFileEntry.delete(0,Tkinter.END)
			self.inShpFileEntry.insert(0,inshp)
			
	def getOutDirectory(self):
		
		outdir = tkFileDialog.askdirectory()
		
		if(outdir != ""):
			self.outDirEntry.delete(0,Tkinter.END)
			self.outDirEntry.insert(0,outdir)
			
	def writeMessage(self, text):
		self.messageWindow.insert(Tkinter.END, text)
		self.messageWindow.update()
		self.messageWindow.yview(Tkinter.END)
		
	##
	## Start now called instead of copy.. This must be fixed at some point
	## TODO: ADD ERROR CHECKING!!!!!!!!!
	##
	def start(self):
		
		try:
			shpfile   = self.inShpFileEntry.get()
			outdir    = self.outDirEntry.get()
			indir 	  = self.inChmDirEntry.get()
			mine 	  = float(self.minEastingEntry.get())
			minn	  = float(self.minNorthingEntry.get())
			blksize   = int(self.blockSizeEntry.get())
			runccf    = int(self.runCcf.get())
			outres    = 0
			zthresh   = 0.0
			ccfoutdir = self.outCCFDirEntry.get()
			
			if (runccf == 1) and (ccfoutdir == ""):
				self.writeMessage("Must specify an output directory for the output CCF tiff file.\n\n");
				return
				
			if runccf == 1 and self.heightThresholdEntry.get() == "":
				self.writeMessage("Must specify a height threshold.\n\n");
				return
				
			if runccf == 1 and self.outResolutionEntry.get() == "":
				self.writeMessage("Must specify an output resolution.\n\n");
				return
				
			if runccf == 1:
				outres = int(self.outResolutionEntry.get())
				zthresh = float(self.heightThresholdEntry.get())
			
			self.messageWindow.insert(Tkinter.END, "Setup\n");
			
			TCR_module.setup(shpfile, blksize, mine, minn, outres, zthresh)
			
			self.messageWindow.insert(Tkinter.END, "here0\n")
			
			dirList = os.listdir(indir)
			if "metadata.txt" in dirList:
				dirList.remove("metadata.txt")
			if "log.txt" in dirList:
				dirList.remove("log.txt")
			if ".DS_Store" in dirList:
				dirList.remove(".DS_Store")
			
			i = 1
			for chmfile in dirList:
				
				self.messageWindow.insert(Tkinter.END, chmfile + "\n")
				infile  = indir + "/" + chmfile
				outfile = outdir + "/" + chmfile # used to be '/OUT_' not just '/'
				a    = chmfile.find("_")
				b    = chmfile.rfind("_")
				c    = chmfile.find(".")
				
				if (a < 0) or (b < 0) or (c < 0):
					self.writeMessage("File format error: File name must be of the form CHM_x_y.txt")
					return
				
				self.writeMessage(chmfile[a+1:b] + "\n")
				self.writeMessage(chmfile[b+1:c] + '\n')
				
				xblk = int(chmfile[a+1:b])
				yblk = int(chmfile[b+1:c])
				
				ccfout = ccfoutdir + "/dbio_" + str(xblk) + "_" + str(yblk) + ".tif"
				
				self.messageWindow.insert(Tkinter.END, "In file: " + infile + "\t" + str(i) + " of " + str(len(dirList)) + "\n");
				self.messageWindow.insert(Tkinter.END, "Out file: " + outfile + "\n");
				self.messageWindow.update()
				self.messageWindow.yview(Tkinter.END)
				
				TCR_module.execute(infile, outfile, xblk, yblk, runccf, ccfout)
				
				i = i + 1
				
			TCR_module.finish()
			self.writeMessage("Done.\n\n")
				
		except:
			self.messageWindow.insert(Tkinter.END, str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			self.messageWindow.insert(Tkinter.END, "failed!\n\n");
			return
		
