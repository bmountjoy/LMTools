
from Tkinter import *
import tkFileDialog
import Bio_module
import random

class Init:
	
	def __init__(self, master, clipBoard):
		
		self.clipBoard = clipBoard
		
		c = 0
		
		#
		# user input frame
		#
		
		userInputFrame = Frame(master)
		userInputFrame.grid(row = 0, column = 0, padx = 10)
		
		r = 0
		
		minEastingLabel = Label(userInputFrame, text="Min. Easting : ")
		minEastingLabel.grid(row = r, column = c, sticky = W);
		
		self.minEastingEntry = Entry(userInputFrame)
		self.minEastingEntry.grid(row = r, column = c+1)
		self.minEastingEntry.insert(0, "651999.9")
		
		r = r + 1
		
		minNorthingLabel = Label(userInputFrame, text="Min. Northing : ")
		minNorthingLabel.grid(row = r, column = c, sticky = W)
		
		self.minNorthingEntry = Entry(userInputFrame)
		self.minNorthingEntry.grid(row = r, column = c+1)
		self.minNorthingEntry.insert(0, "5599999.95")
		
		r = r + 1
		
		blockSizeLabel = Label(userInputFrame, text="Block Size : ")
		blockSizeLabel.grid(row = r, column = c, sticky = W)
		
		self.blockSizeEntry = Entry(userInputFrame)
		self.blockSizeEntry.grid(row = r, column = c+1)
		self.blockSizeEntry.insert(0, "1000")
		
		r = r + 1
		
		htThresholdLabel = Label(userInputFrame, text="Height Threshold : ")
		htThresholdLabel.grid(row = r, column = c, sticky = W)
		
		self.htThresholdEntry = Entry(userInputFrame)
		self.htThresholdEntry.grid(row = r, column = c+1)
		
		#
		# bounding region frame
		#
		
		boundingRegionFrame = Frame(master)
		boundingRegionFrame.grid(row = 1, column = 0, sticky = NW)
		
		r = 0
		
		boundingRegionLabel = Label(boundingRegionFrame, text="Bounding Region")
		boundingRegionLabel.grid(row = r, column = c, sticky = W, pady = 10)
		
		r = r + 1
		
		self.radiusBound = IntVar()
		radiusCheckbutton = Checkbutton(boundingRegionFrame, text = "Circle", variable = self.radiusBound, command=self.radiusBoundChecked)
		radiusCheckbutton.grid(row = r, column = c, padx = 10, sticky = W)
		
		self.radiusEntry =  Entry(boundingRegionFrame)
		self.radiusEntry.grid(row = r, column = c+1, sticky = W)
		self.radiusEntry.insert(0, "11")
		
		r = r + 1
		
		self.boxBound = IntVar()
		boxCheckbutton = Checkbutton(boundingRegionFrame, text = "Square", variable = self.boxBound, command=self.squareBoundChecked)
		boxCheckbutton.grid(row = r, column = c, padx = 10, sticky = W)
		
		self.boxEntry = Entry(boundingRegionFrame)
		self.boxEntry.grid(row = r, column = c+1, sticky = W)
		self.boxEntry.insert(0, "21")
		
		#
		# chm input directory / out file frame
		#
		
		chmOutFrame = Frame(master)
		chmOutFrame.grid(row = 2, column = 0, pady = 10, sticky = NW)
		
		r = 0
		
		chmOutLabel = Label(chmOutFrame, text = "CHM Directory / Output File")
		chmOutLabel.grid(row = r, column = c, pady = 10)
		
		r = r + 1
		
		pointDirButton = Button(chmOutFrame, text = "CHM Directory", command = self.get_input_chm_directory)
		pointDirButton.grid(row = r, column = c, sticky = W, padx = 10)
		
		self.pointDirEntry = Entry(chmOutFrame, width = 42)
		self.pointDirEntry.grid(row = r, column = c+1, columnspan = 2, sticky = W)
		
		r = r + 1
		
		outFileButton = Button(chmOutFrame, text="Output File", command = self.get_output_file)
		outFileButton.grid(row = r, column = c, sticky = W, padx = 10)
		
		self.outFileEntry = Entry(chmOutFrame, width = 42)
		self.outFileEntry.grid(row = r, column = c+1, columnspan = 2, sticky = W)
		
		
                #
		# centroid frame - user input parameters
		#
		centroidFrame = Frame(master);
		centroidFrame.grid(row = 3, column = 0, sticky = NW)
		
		r = 0;
		
		centroidInputLabel = Label(centroidFrame, text="Centroid Points Input")
		centroidInputLabel.grid(row = r, column = c, sticky = W, pady = 10)
		
		r = r + 1
		
		centroidLabel = Label(centroidFrame, text="Centroid :")
		centroidLabel.grid(row = r, column = c, sticky = W, padx = 10)
		
		self.centEastEntry = Entry(centroidFrame)
		self.centEastEntry.grid(row = r, column = c+1, sticky = W)
		
		self.centNorthEntry = Entry(centroidFrame)
		self.centNorthEntry.grid(row = r, column = c+2, sticky = W)
		
		r = r + 1
		
		pointFileButton = Button(centroidFrame, text = "Input File", command = self.get_centroid_points_file)
		pointFileButton.grid(row = r, column = c, sticky = W, padx = 10)
		
		self.pointFileEntry = Entry(centroidFrame, width = 42)
		self.pointFileEntry.grid(row = r, column = c+1, columnspan = 2, sticky = W)
		
		r = r + 1
		
		executeButton = Button(centroidFrame, text = "Compute Biometrics", command = self.execute)
		executeButton.grid(row = r, column = c, padx = 10, pady=10)
		
		
		#
		# random points frame
		#
		
		randomContainFrame = Frame(master)
		randomContainFrame.grid(row = 4, column = 0, sticky = W)
		
		grpLabel = Label(randomContainFrame, text="Generate Random Centroid Points")
		grpLabel.grid(row = 0, column = 0, sticky = W, pady=10)
		
		randomFrame = Frame(randomContainFrame)
		randomFrame.grid(row = 1, column = 0, sticky = NW, padx = 10)
		
		r = 0
		
		numPointsLabel = Label(randomFrame, text = "Number of Ponints : ")
		numPointsLabel.grid(row = r, column = c, sticky = W)
		
		self.numPointsEntry = Entry(randomFrame)
		self.numPointsEntry.grid(row = r, column = c + 1, sticky = W)
		
		r = r + 1
		
		eastingRangeLabel = Label(randomFrame, text="Easting Range : ")
		eastingRangeLabel.grid(row = r, column = c, sticky = W)
		
		self.minEastingRangeEntry = Entry(randomFrame)
		self.minEastingRangeEntry.grid(row = r, column = c + 1, sticky = W)
		
		self.maxEastingRangeEntry = Entry(randomFrame)
		self.maxEastingRangeEntry.grid(row = r, column = c + 2, sticky = W)
		
		r = r + 1
		
		northingRangeLabel = Label(randomFrame, text="Northing Range : ")
		northingRangeLabel.grid(row = r, column = c, sticky = W)
		
		self.minNorthingRangeEntry = Entry(randomFrame)
		self.minNorthingRangeEntry.grid(row = r, column = c + 1, sticky = W)
		
		self.maxNorthingRangeEntry = Entry(randomFrame)
		self.maxNorthingRangeEntry.grid(row = r, column = c + 2, sticky = W)
		
		r = r + 1
		
		self.processLater = IntVar()
		plCheckbutton = Checkbutton(randomFrame, text="Write to file (process later).", variable = self.processLater)
		plCheckbutton.grid(row = r, column = c, sticky = W, columnspan = 2)
		
		r = r + 1
		
		randFileButton = Button(randomFrame, text = "File", command = self.get_random_points_file)
		randFileButton.grid(row = r, column = c, padx = 10, sticky = E)
		
		self.randFileEntry = Entry(randomFrame, width = 42)
		self.randFileEntry.grid(row = r, column = c + 1, sticky = W, columnspan = 2)
		
		r = r + 1
		
		generateRandomPointsButton = Button(randomFrame, text = "Generate Points (+ Compute Biometrics)", command = self.generate_random_points)
		generateRandomPointsButton.grid(row = r, column = c, sticky = W, columnspan = 2, pady=10)
		
		#
		# execute frame
		#
		
		executeFrame = Frame(master)
		executeFrame.grid(row = 5, column = 0, sticky = W)
		
		r = 0
		
		quitButton = Button(executeFrame, text = "Quit", command = master.quit)
		quitButton.grid(row = r, column = c+1)
		
		#
		# message frame
		#
		
		outerFrame = Frame(master)
		outerFrame.grid(row = 0, column = 1, rowspan = 5, padx = 10, sticky = NW)
		
		resultLabel = Label(outerFrame, text="Status Message...")
		resultLabel.grid(row = 0, column = 0, sticky = NW)
		
		scrollFrame = Frame(outerFrame)
		scrollFrame.grid(row = 1, column = 0)
		
		scroll = Scrollbar(scrollFrame)
		scroll.pack(side=RIGHT, fill=Y)
		
		self.resultText = Text(scrollFrame, width = 80, height = 44, background='white', borderwidth = 1, relief = RIDGE, yscrollcommand=scroll.set)
		self.resultText.pack(side=LEFT)
		
		scroll.config(command=self.resultText.yview)
		
	def get_random_points_file(self):
		out_file = tkFileDialog.askopenfilename()
		if out_file == "":
			return
		self.randFileEntry.delete(0, END)
		self.randFileEntry.insert(0, out_file)
		
	def radiusBoundChecked(self):
		if self.radiusBound.get() == 1:
			self.boxBound.set(0)
			
	def squareBoundChecked(self):
		if self.boxBound.get() == 1:
			self.radiusBound.set(0)
		
	def get_centroid_points_file(self):
		in_file = tkFileDialog.askopenfilename()
		if in_file == "":
			return
		self.pointFileEntry.delete(0, END)
		self.pointFileEntry.insert(0, in_file)
		
	def get_input_chm_directory(self):
		chm_dir = tkFileDialog.askdirectory()
		if chm_dir == "":
			return
		self.pointDirEntry.delete(0, END)
		self.pointDirEntry.insert(0,chm_dir)
		
	def get_output_file(self):
		out_file = tkFileDialog.askopenfilename()
		if out_file == "":
			return
		self.outFileEntry.delete(0, END)
		self.outFileEntry.insert(0,out_file)
		
		
	def update_message_text(self, text):
		self.resultText.insert(END, text)
		self.resultText.update()
		self.resultText.yview(END)
		
	def handle_exception(self, exception):
		self.update_message_text("\n" + str(exception[0]) + " : "  + str(exception[1]) + "\n\n")
		
	def write_centroid_header(self, fpath):
		try:
			header = ["Point", "Easting", "Northing", "Rugosity", "Gap", "Percentile", "L_Mean", "L_Cov", "L_Skew", "L_Kurt", "85th_Percentile",\
				"LHQ_0", "LHQ_5", "LHQ_10", "LHQ_15", "LHQ_20", "LHQ_25", "LHQ_30", "LHQ_35", "LHQ_40", "LHQ_45", "LHQ_50", "LHQ_55", "LHQ_60", "LHQ_65", "LHQ_70", "LHQ_75", "LHQ_80", "LHQ_85", "LHQ_90", "LHQ_95", "LHQ_100",\
				"CCF_0", "CCF_5", "CCF_10", "CCF_15", "CCF_20", "CCF_25", "CCF_30", "CCF_35", "CCF_40", "CCF_45", "CCF_50", "CCF_55", "CCF_60", "CCF_65", "CCF_70", "CCF_75", "CCF_80", "CCF_85", "CCF_90", "CCF_95", "CCF_100", ]
			
			f = open(fpath, "w")
			
			for label in header:
				f.write(label + " ")
			f.write("\n")
			f.close()
		except:
			self.handle_exception(sys.exc_info())
		
	def generate_random_points(self):
		
		try:
			if self.numPointsEntry.get() == "":
				self.update_message_text("Error. Must specify the number of points to generate.\n\n")
				return
				
			if self.minEastingRangeEntry.get() == "" or self.maxEastingRangeEntry.get() == "" or\
			   self.minNorthingRangeEntry.get() == "" or self.maxNorthingRangeEntry.get() == "":
				self.update_message_text("Error. Must specify range of the random numbers to generate.\n\n")
				return
				
			numPoints   = int(self.numPointsEntry.get())
			minEasting  = float(self.minEastingRangeEntry.get())
			maxEasting  = float(self.maxEastingRangeEntry.get())
			minNorthing = float(self.minNorthingRangeEntry.get())
			maxNorthing = float(self.maxNorthingRangeEntry.get())
			
			self.update_message_text("Generating " + str(numPoints) + " random points...\n")
			
			eRange = maxEasting - minEasting
			nRange = maxNorthing - minNorthing
			
			randPoints = []
			
			for i in range(numPoints):
				
				x = random.random()
				x = x * eRange
				x = x + minEasting
				y = random.random()
				y = y * nRange
				y = y + minNorthing
				randPoints.append([x,y])
			
			if self.processLater.get() == 1:
				
				f = open(self.randFileEntry.get(), "w")
				
				self.update_message_text("Writing points to " + self.randFileEntry.get() + "...\n")
				
				#write to file
				for point in randPoints:
					f.write(str(point[0]) + ' ' + str(point[1]) + '\n')
				
				f.close()
				
				self.update_message_text("Complete!\n")
					
			else:
				
				#process all the points
				if self.pointDirEntry.get() == "" or self.outFileEntry.get() == "":
					self.update_message_text("Error: Must specify input CHM directory and output directory.\n\n")
					return
				
				if self.minEastingEntry.get() == ""  or self.minNorthingEntry.get() == "" or \
			           self.blockSizeEntry.get() == ""   or self.htThresholdEntry.get() == "":
			           	   self.update_message_text("Error: Missing input.\n\n")
			           	   return
				
				if self.radiusBound.get() == 0 and self.boxBound.get() == 0:
					self.update_message_text("Error: Must specify bounding region.\n\n")
					return
				
				minE = float(self.minEastingEntry.get())
				minN = float(self.minNorthingEntry.get())
				blkS = int(self.blockSizeEntry.get())
				zThr = int(self.htThresholdEntry.get())
				chmD = self.pointDirEntry.get()
				outF = self.outFileEntry.get()
				
				self.write_centroid_header(outF)
				
				pointIdx = 1
				numPoints = len(randPoints)
				
				for point in randPoints:
					
					cntE = point[0]
					cntN = point[1]
					
					self.update_message_text("Point " + str(pointIdx) + " of " + str(numPoints))
					self.update_message_text("   (" + str(cntE) + ", " + str(cntN) + ")\n")
					
					if self.radiusBound.get() == 1:
						
						radius = int(self.radiusEntry.get())
						Bio_module.computeBiometricsAroundCentroid(pointIdx, cntE, cntN, minE, minN, radius, blkS, zThr, chmD, outF)
					else:
						
						length = int(self.boxEntry.get())
						Bio_module.computeBiometricsInBoundingBox(pointIdx, cntE, cntN, minE, minN, length, blkS, zThr, chmD, outF)
					
					pointIdx = pointIdx + 1
				
				self.update_message_text("Complete.\n\n")
		except:
			self.handle_exception(sys.exc_info())
		
	def execute(self):
		
		try:
			pointFile    = self.pointFileEntry.get()
			centEasting  = self.centEastEntry.get()
			centNorthing = self.centNorthEntry.get()
			
			#
			# ensure valid input
			#
			if pointFile == "":
				if centEasting == "" or centNorthing == "":
					self.update_message_text("Error: Must specify a centroid or input file.\n\n")
					return
			
			if self.pointDirEntry.get() == "" or self.outFileEntry.get() == "":
				self.update_message_text("Error: Must specify input CHM directory and output directory.\n\n")
			   	return
			   	
			if self.radiusBound.get() == 0 and self.boxBound.get() == 0:
				self.update_message_text("Error: Must specify bounding region.\n\n")
			   	return
					
			if self.minEastingEntry.get() == ""  or self.minNorthingEntry.get() == "" or \
			   self.blockSizeEntry.get() == "" or self.htThresholdEntry.get() == "":
			   	
				self.update_message_text("Error: Missing input.\n\n")
			   	return
			
			points = []
		
			if not(pointFile == ""):
			
				pointFile = open(pointFile, "r")
				
				line = pointFile.readline()
				
				while not(line == ""):
					
					self.update_message_text("line:"+line)
					line  = line.strip()
					self.update_message_text("strip:"+line + "\n")
					point = line.split()
					
					if len(point) != 2:
						self.update_message_text("Error: Input centroid points file format error. " + str(len(point)) + "\n\n")
						return
					
					points.append( [ float(point[0]), float(point[1]) ] )
					
					line = pointFile.readline()
		
			else:
				easting  = float(self.centEastEntry.get())
				northing = float(self.centNorthEntry.get())
				
				points.append([easting, northing])
						
			curPoint  = 1
			numPoints = len(points)
			
			self.write_centroid_header(self.outFileEntry.get())
			
			for point in points:
				
				centroidEasting  = point[0]
				centroidNorthing = point[1]
				minEasting 	 = float(self.minEastingEntry.get())
				minNorthing 	 = float(self.minNorthingEntry.get())
				blockSize 	 = int(self.blockSizeEntry.get())
				zThreshold 	 = int(self.htThresholdEntry.get())
				chmDir 		 = self.pointDirEntry.get()
				outFile 	 = self.outFileEntry.get()
				
				self.update_message_text("Point " + str(curPoint) + " of " + str(numPoints))
				self.update_message_text("   (" + str(centroidEasting) + ", " + str(centroidNorthing) + ")\n")
				
				if self.radiusBound.get() == 1:
					
					radius = int(self.radiusEntry.get())
					Bio_module.computeBiometricsAroundCentroid(curPoint, centroidEasting, centroidNorthing, minEasting, minNorthing, radius, blockSize, zThreshold, chmDir, outFile)
	
					
				else:
					
					length = int(self.boxEntry.get())
					Bio_module.computeBiometricsInBoundingBox(curPoint, centroidEasting, centroidNorthing, minEasting, minNorthing, length, blockSize, zThreshold, chmDir, outFile)
				
				curPoint = curPoint + 1
				
			self.update_message_text("Complete.\n\n")
			
		except:
			self.handle_exception(sys.exc_info())
