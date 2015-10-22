
from   Tkinter import *
import tkFileDialog
import Bio_module
import sys
import os

class Init:
	
	def __init__(self, master, clipBoard):
		
		self.clipBoard = clipBoard
		
                #
		# frame 1 - user input parameters
		#
		frame1 = Frame(master);
		frame1.grid(row = 0, column = 0)
		
		r = 0;
		c = 0;
		
		minEastingLabel = Label(frame1, text="Min. Easting : ")
		minEastingLabel.grid(row = r, column = c, sticky=W);
		
		self.minEastingEntry = Entry(frame1)
		self.minEastingEntry.grid(row = r, column = c+1)
		self.minEastingEntry.insert(0, "1121323.8050")
		
		r = r + 1
		
		minNorthingLabel = Label(frame1, text="Min. Northing : ")
		minNorthingLabel.grid(row = r, column = c, sticky=W)
		
		self.minNorthingEntry = Entry(frame1)
		self.minNorthingEntry.grid(row = r, column = c+1)
		self.minNorthingEntry.insert(0, "396909.7500")
		
		r = r + 1
		
		blkSizeLabel = Label(frame1, text="Block Size : ")
		blkSizeLabel.grid(row = r, column = c, sticky = W)
		
		self.blkSizeEntry = Entry(frame1)
		self.blkSizeEntry.grid(row = r, column = c+1)
		self.blkSizeEntry.insert(0, "500")
		
		r = r + 1
		
		outResLabel = Label(frame1, text="Output Resolution : ")
		outResLabel.grid(row = r, column = c, sticky = W)
		
		self.outResEntry = Entry(frame1)
		self.outResEntry.grid(row = r, column = c+1)
		self.outResEntry.insert(0, "20")
		
		r = r + 1
		
		htThresholdLabel = Label(frame1, text="Height Threshold : ")
		htThresholdLabel.grid(row = r, column = c, sticky = W)
		
		self.htThresholdEntry = Entry(frame1)
		self.htThresholdEntry.grid(row = r, column = c+1)
		self.htThresholdEntry.insert(0,"2")

		#
		# frame rename - renaming input and output folders
		#
		
		renameFrame = Frame(master)
		renameFrame.grid(row = 1, column = 0, sticky=W, padx = 5)

		r = 0
		c = 0

		renameLabel = Label(renameFrame, text="Rename Files")
		renameLabel.grid(row = r, column = c, pady = 10, sticky=W)

		r = r+1

		renameInDirButton = Button(renameFrame, text="Input Directory", command = self.get_rename_input_dir)
		renameInDirButton.grid(row = r, column = c, sticky=W, padx=10)
		
		self.inRenameEntry = Entry(renameFrame, width=40)
		self.inRenameEntry.grid(row = r, column = c+1)

		r = r+1

		renameOutDirButton = Button(renameFrame, text="Output Directory", command = self.get_rename_output_dir)
		renameOutDirButton.grid(row = r, column = c, sticky=W, padx=10)

		self.outRenameEntry = Entry(renameFrame, width=40)
		self.outRenameEntry.grid(row = r, column = c+1)

		r = r+1
		
		bbbLabel = Label(renameFrame, text=" B Cubed Filter:")
		bbbLabel.grid(row = r, column = c, sticky = W, padx = 10)
		
		self.bbbEntry = Entry(renameFrame)
		self.bbbEntry.grid(row = r, column = c+1, sticky = W)
		self.bbbEntry.insert(0, "100.0")

		r = r + 1
		
		Label(renameFrame, text = "Ouput renamed file to:").grid(row = r, column = 0, sticky = W)
		
		r = r + 1
		
		self.outputAsText = IntVar()
		
		frame1 = Frame(renameFrame)
		frame1.grid(row = r, column = 1, sticky = W)
		textCheckbutton = Checkbutton(frame1, variable = self.outputAsText)
		textCheckbutton.grid(row = 0, column = 0, sticky = W)
		
		textLabel = Label(frame1, text = "Text")
		textLabel.grid(row = 0, column = 1, sticky = W)
		
		r = r + 1
		
		self.outputAsBin = IntVar()
		
		frame2 = Frame(renameFrame)
		frame2.grid(row = r, column = 1, sticky = W)
		binCheckbutton = Checkbutton(frame2, variable = self.outputAsBin)
		binCheckbutton.grid(row = 0, column = 0, sticky = W)
		
		binLabel = Label(frame2, text = "Bin")
		binLabel.grid(row = 0, column = 1, sticky = W)
		
		r = r + 1
		
		renameButton = Button(renameFrame, text="Rename", command = self.execute_rename)
		renameButton.grid(row = r, column = c, sticky=W, pady=5, padx =10)
		
		#
		# frame 2 - biometric checkbuttons
		#
		
		containFrame2 = Frame(master)
		containFrame2.grid(row = 2, column = 0, sticky = W, padx = 5)
		
		bioLabel = Label(containFrame2, text="Calculate Biometrics")
		bioLabel.grid(row = 0, column = 0, sticky=W)
		
		frame2 = Frame(containFrame2)
		frame2.grid(row = 2, column = 0, sticky = W, padx = 10, pady = 10)
		
		r = 0
		c = 0

		inFileButton     = Button(frame2, text="Input File", command=self.get_input_file)
		inFileButton.grid(row = r, column = c, sticky=W)
		
		self.inFileEntry  = Entry (frame2, width=40)
		self.inFileEntry.grid(row = r, column = c+1)
		
		r = r + 1

		inDirButton     = Button(frame2, text="Input Directory", command=self.get_input_directory)
		inDirButton.grid(row = r, column = c, sticky = W)

		self.inDirEntry = Entry(frame2, width = 40)
		self.inDirEntry.grid(row = r, column = c+1)

		r = r + 1
		
		outDirButton     = Button(frame2, text="Output Directory", command=self.get_output_directory)
		outDirButton.grid(row = r, column = c, sticky=W)
		
		self.outDirEntry = Entry (frame2, width=40)
		self.outDirEntry.grid(row = r, column = c+1)
		
		r = r + 1
		
		self.computeAll = IntVar()
		allCheckbutton = Checkbutton(frame2, text="Select All", variable=self.computeAll, command=self.all_checked);
		allCheckbutton.grid(row = r, column = c, sticky=W, padx=30)
		
		r = r + 1
		
		self.computeBiometrics = IntVar()
		bioCheckbutton = Checkbutton(frame2, text="Biometrics", variable=self.computeBiometrics, command=self.bio_checked);
		bioCheckbutton.grid(row = r, column = c, sticky=W, padx=30)
		
		r = r + 1
		
		self.computeLhq = IntVar()
		lhqCheckbutton = Checkbutton(frame2, text="LHQ", variable = self.computeLhq, command=self.lhq_checked)
		lhqCheckbutton.grid(row = r, column = c, sticky=W, padx=30)
		
		r = r + 1
		
		self.computeCcf = IntVar()
		ccfCheckbutton = Checkbutton(frame2, text="CCF", variable = self.computeCcf, command=self.ccf_checked)
		ccfCheckbutton.grid(row = r, column = c, sticky=W, padx=30)

		r = r+1

		executeButton = Button(frame2, text="Compute Biometrics", command=self.execute)
		executeButton.grid(row = r, column = 0, sticky=W, pady=5)
                
		#
		# frame 3 - scroll bar and status message window
		#

		frame3 = Frame(master)
		frame3.grid(row = 0, column = 1, rowspan = 3, padx = 10, sticky=NW)
		
		resultLabel = Label(frame3, text="Status Message...")
		resultLabel.grid(row = 0, column = 0, sticky = NW)
		
		frame4 = Frame(frame3)
		frame4.grid(row = 1, column = 0, sticky=NW)
		
		scroll = Scrollbar(frame4)
		scroll.pack(side=RIGHT, fill=Y)
		
		self.resultText = Text(frame4, height = 34, width = 80, background='white', borderwidth = 1, relief = RIDGE, yscrollcommand=scroll.set)
		self.resultText.pack(side=LEFT)
		
		scroll.config(command=self.resultText.yview)
		
		#
		# master frame
		#
		
		quitButton = Button(master, text="Quit", command=master.quit)
		quitButton.grid(row = 3, column = 0, sticky = W, padx = 5, pady = 5)
		
	def all_checked(self):
		if self.computeAll.get() == 1 :
			self.computeBiometrics.set(1)
			self.computeLhq.set(1)
			self.computeCcf.set(1)
		elif self.computeAll.get() == 0 and self.computeBiometrics.get() == 1 and self.computeLhq.get() == 1 and self.computeCcf.get() == 1 :
			self.computeAll.set(1)
			
	def bio_checked(self):
		if self.computeBiometrics.get() == 0 :
			self.computeAll.set(0)
		elif self.computeBiometrics.get() == 1 and self.computeLhq.get() == 1 and self.computeCcf.get() == 1 :
			self.computeAll.set(1)
			
	def lhq_checked(self):
		if self.computeLhq.get() == 0 :
			self.computeAll.set(0)
		elif self.computeBiometrics.get() == 1 and self.computeLhq.get() == 1 and self.computeCcf.get() == 1 :
			self.computeAll.set(1)
			
	def ccf_checked(self):
		if self.computeCcf.get() == 0 :
			self.computeAll.set(0)
		elif self.computeBiometrics.get() == 1 and self.computeLhq.get() == 1 and self.computeCcf.get() == 1 :
			self.computeAll.set(1)
		
	def get_input_file(self):
		in_file = tkFileDialog.askopenfilename()
		if in_file == "":
			return
		self.inDirEntry.delete(0,END)
		self.inFileEntry.delete(0,END)
		self.inFileEntry.insert(0, in_file)

	def get_input_directory(self):
		in_dir = tkFileDialog.askdirectory()        
		if in_dir == "":
                	return	
		self.inFileEntry.delete(0, END)
		self.inDirEntry.delete(0, END)
		self.inDirEntry.insert(0, in_dir)
		
	def get_output_directory(self):
		out_dir = tkFileDialog.askdirectory()
		
		if out_dir == "":
			return
		
		self.outDirEntry.delete(0,END)
		self.outDirEntry.insert(0, out_dir)
		
	def update_result_text(self, message):
		self.resultText.insert(END, message)
		self.resultText.update()
		self.resultText.yview(END)
		
	def handle_exception(self, exception):
		self.update_result_text("\n" + str(exception[0]) + " : "  + str(exception[1]) + "\n\n")
		
	def get_rename_input_dir(self):
		in_dir = tkFileDialog.askdirectory()
		if in_dir == "":
			return
		self.inRenameEntry.delete(0, END)
		self.inRenameEntry.insert(0, in_dir)

	def get_rename_output_dir(self):
		out_dir =  tkFileDialog.askdirectory()
		if out_dir == "":
			return
		self.outRenameEntry.delete(0, END)
		self.outRenameEntry.insert(0, out_dir)

	def execute_rename(self):

		if self.inRenameEntry.get() == "":
			self.update_result_text("Must select an input directory.\n\n")
			return

		if self.outRenameEntry.get() == "":
			self.update_result_text("Must select an ouput directory.\n\n")
			return
			
		if self.outputAsText.get() == 0 and self.outputAsBin.get() == 0:
			self.update_result_text("Must selexct output file type.\n\n")
			return

		self.update_result_text("Renaming files...\n")
		self.update_result_text("  From : " + self.inRenameEntry.get() + "\n")
		self.update_result_text("  To   : " + self.outRenameEntry.get() + "\n")
		self.update_result_text("Running file...\n")
		
		try:
			minE = float(self.minEastingEntry.get())
			minN = float(self.minNorthingEntry.get());
			blkS = int(self.blkSizeEntry.get());
			bDir = self.inRenameEntry.get();
			outDir = self.outRenameEntry.get()
			zThr = float(self.bbbEntry.get())
			outTxt = self.outputAsText.get()
			outBin = self.outputAsBin.get()
			
			if outTxt == 1:
				if not(os.path.isdir(outDir + "/text")):
					os.makedirs(outDir + "/text")
					
			if outBin == 1:
				if not(os.path.isdir(outDir + "/bin")):
					os.makedirs(outDir + "/bin")
			
			logFile = open(outDir + "/log.txt", "w");
			self.update_result_text(str(logFile) + "\n")
                
			dirList  = os.listdir(self.inRenameEntry.get())
			badFiles = []
			
			for fname in dirList:
				if not(fname.endswith(".bin")):
					badFiles.append(fname)
			
			for badFile in badFiles:
				dirList.remove(badFile)
				
			fileCount = len(dirList)
			currIndex = 1
				
			for fname in dirList:
					
				self.update_result_text("   - " + str(currIndex) + " of " + str(fileCount) + "\n")
				
				try:
					outFile = Bio_module.rename(bDir + "/" + fname, outDir, minE, minN, blkS, zThr, outTxt, outBin)
					logFile.write(bDir + "/" + fname + " " + outFile + "\n")
				except:
					self.update_result_text("Error renaming file " + fname + ".\n")
					
				currIndex = currIndex + 1
			
			logFile.close()
		except:
			self.handle_exception(sys.exc_info())

		self.update_result_text("Complete.\n\n")
		
		
	def execute(self):
		
		if self.inFileEntry.get() == "" and self.inDirEntry.get() == "":
			self.update_result_text("Must select an input file or input directory.\n\n")
			return
			
		if self.outDirEntry.get() == "" :
			self.update_result_text("Must select an output directory.\n\n")
			return
			
		if self.computeBiometrics.get() == 0 and self.computeLhq.get() == 0 and self.computeCcf.get() == 0 :
			self.update_result_text("Nothing selected to calculate.\n\n")
			return
		
		self.update_result_text("Running biometrics...\n")
			
		if self.inFileEntry.get() != "":
			
			self.update_result_text("\n")
			self.update_result_text("Running...\n")	
			self.update_result_text("File 1 of 1\n")
			
			try:
				result = self.process_file(self.inFileEntry.get())
			except:
				self.handle_exception(sys.exc_info())
				return
			
			if result != None :
				self.clipBoard["ul_east"] = result[0]
				self.clipBoard["br_east"] = result[1]
				self.clipBoard["ul_north"] = result[2]
				self.clipBoard["br_north"] = result[3]
			
		
		else:
			try:
				dirList   = os.listdir(self.inDirEntry.get())
				badFiles  = []
				
				for fname in dirList:
					if not(fname.endswith(".bin")) :
						badFiles.append(fname)
				
				for badFile in badFiles:
					dirList.remove(badFile)
					
				fileCount = len(dirList)
				
				baseDir = self.inDirEntry.get()
				fileIdx = 1
				for fname in dirList:
					self.update_result_text("File " + str(fileIdx) + " of " + str(fileCount) + " " + fname + "\n")
					self.process_file(baseDir + "/" + fname)
					fileIdx = fileIdx + 1
			except:
				self.handle_exception(sys.exc_info())
				return
				
		self.update_result_text("Complete.\n\n")
		
		
	def process_file(self, filePath):
		
		self.update_result_text("    - Setup\n");
		
		try:
			minE = float(self.minEastingEntry.get());
			minN = float(self.minNorthingEntry.get());
			blkS = int(self.blkSizeEntry.get());
			outR = int(self.outResEntry.get());
			zThr = float(self.htThresholdEntry.get());
			zmax = float(self.bbbEntry.get())
		
			result = Bio_module.setup(filePath, self.outDirEntry.get(), minE, minN, blkS, outR, zThr, zmax)
		except:
			self.handle_exception(sys.exc_info())
			return
		
		if self.computeBiometrics.get() == 1 :
			
			self.update_result_text("    - Biometrics\n")
			try:
				Bio_module.bio()
			except:
				self.handle_exception(sys.exc_info())
				return
			
		if self.computeLhq.get() == 1 : 
			
			self.update_result_text("    - LHQ\n")
			try:
				Bio_module.lhq()
			except:
				self.handle_exception(sys.exc_info())
				return
			
		if self.computeCcf.get() == 1 :
			
			self.update_result_text("    - CCF\n")
			try:
				Bio_module.ccf()
			except:
				self.handle_exception(sys.exc_info())
				return
				
		self.update_result_text("    - Teardown\n")
		
		Bio_module.teardown()
		
		return result;
