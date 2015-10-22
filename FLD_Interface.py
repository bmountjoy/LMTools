

import Tkinter
import sys
import os
import re
from os import path
import tkFileDialog
import FLD_module
import time

class Init:
	
	def __init__(self, master, clipBoard):
		
		self.clipBoard = clipBoard
	
		frame = Tkinter.Frame(master)
		frame.grid(row = 0, column = 0)
		
		###################### Pad Frame ##################################
		
		padframe1 = Tkinter.Frame(frame)
		padframe1.grid(row = 0, column = 0, pady = 16)
		
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
		
		projectNameLabel = Tkinter.Label(input_frame, text="Project Name")
		projectNameLabel.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.projectNameEntry = Tkinter.Entry(input_frame)
		self.projectNameEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		#self.projectNameEntry.insert(0, "afdaa")

		r = r + 1
		
		bandLabel = Tkinter.Label(input_frame, text="Band to Use")
		bandLabel.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.bandToUseEntry = Tkinter.Entry(input_frame)
		self.bandToUseEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		#self.bandToUseEntry.insert(0, "1")
		
		r = r + 1
	
		inEnviDirectoryButton = Tkinter.Button(input_frame, text="Input ENVI Directory", command = self.getInputEnviDirectory)
		inEnviDirectoryButton.grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inEnviDirectoryEntry = Tkinter.Entry(input_frame, width = 45)
		self.inEnviDirectoryEntry.grid(row = r, column = 1)
		#self.inEnviDirectoryEntry.insert(0, "/Users/Ben/Desktop/test_fl")
		
		r = r + 1
		
		outDirButton = Tkinter.Button(input_frame, text="Output Directory:", command = self.getOutputDirectory)
		outDirButton.grid(row = r, column = 0, pady = 10, sticky = Tkinter.W)
		
		self.outDirEntry = Tkinter.Entry(input_frame, width = 45)
		self.outDirEntry.grid(row = r, column = 1)
		#self.outDirEntry.insert(0, "/Users/Ben/Desktop/test_fl/out")
		
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
		
		copyImageButton = Tkinter.Button(exe_frame, text="Start", command = self.start)
		copyImageButton.grid(row = 0, column = 0, sticky = Tkinter.SW, pady = 10)
		
			
	def getInputEnviDirectory(self):
		infile = tkFileDialog.askdirectory()
		if(infile != ""):
			self.inEnviDirectoryEntry.delete(0,Tkinter.END)
			self.inEnviDirectoryEntry.insert(0,infile)
			
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
	def start(self):
		
		try:
			band    = int(self.bandToUseEntry.get())
			in_dir  = self.inEnviDirectoryEntry.get()
			out_dir = self.outDirEntry.get()
			proj_name = self.projectNameEntry.get()
			
			dir_list = os.listdir(in_dir)
			regex = re.compile('[-,_, ,0-9,a-z,A-Z]+\.dat$')
			
			for f_name in dir_list:
				
				if not regex.match(f_name):
					continue
					
				fl_path = in_dir + "/" + f_name[:f_name.rfind(".")] 	#base path
				vec_path = out_dir + "/" + f_name[:f_name.rfind(".")]
				
				self.writeMessage("Input flight line path : \n\t" + fl_path + "\n")
				self.writeMessage("Output vector delin. path : \n\t" + vec_path + "\n")
				
				FLD_module.delineateFlightLine(f_name[:f_name.rfind(".")], fl_path, vec_path, band)
				
			
			merged_filePath = out_dir + '/' + proj_name + '_merged';
			self.writeMessage('Output file : \n\t' + merged_filePath + '\n')
			
			regex = re.compile('[-,_, ,0-9,a-z,A-Z]+\.shp$')
			dir_list = os.listdir(out_dir)
			
			FLD_module.initMergedShapeFile(merged_filePath);
			
			for f_name in dir_list:
				if not regex.match(f_name):
					continue
				self.writeMessage('Merging ' + out_dir + '/' + f_name + '\n')
				FLD_module.mergeShapeFiles(out_dir + '/' + f_name, merged_filePath)
				
			self.writeMessage('Done.\n\n')
				
		except:
			self.messageWindow.insert(Tkinter.END, str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
			self.messageWindow.insert(Tkinter.END, "failed!\n\n");
			return
		
