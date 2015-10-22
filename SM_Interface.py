

import Tkinter
import sys
import os
from os import path
import tkFileDialog
import envi_header_io
import SM_module


class Init:
	
	def __init__(self, master, clipBoard):
		
		self.clipBoard = clipBoard
		self.helpMessage = "SPECTRAL METRICS\n\n" + \
			"Library Spectra: CSV file with the spectra organized across each row.\n" + \
			" - the first row is a header and should contain column labels\n" +\
			" - each subsequent row contains contains the spectra, where the first column of each row contains the spectrum label\n\n" +\
			"Observed Spectra: ENVI header file or CSV file.\n\n" + \
			"Output Directory: Root directory to which the spectral metrics are written. A seperate directory is created for each compare range (below).\n\n" + \
			"COMPARISON RANGES\n\n" + \
			" - wavelength ranges for which to apply the SAM and Spectral Matching between the library and observed spectra"
			
		self.class_thresholds = []
			
		
		frame = Tkinter.Frame(master)
		frame.grid(row = 0, column = 0, padx = 10, sticky = Tkinter.NW)
		
		left_frame = Tkinter.Frame(frame)
		left_frame.pack(side = Tkinter.LEFT, fill = Tkinter.BOTH, expand = 1)
		
		right_frame = Tkinter.Frame(frame)
		right_frame.pack(side = Tkinter.RIGHT, fill = Tkinter.BOTH, expand = 1)
		
		###################################################################
		###################### Input Frame ################################
		###################################################################
		
		input_frame = Tkinter.Frame(left_frame)
		input_frame.grid(row = 0, column = 0, sticky = Tkinter.NW)
		
		#grid row
		r = 0
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, pady = 16, sticky = Tkinter.W)
		
		r = r + 1
		
		Tkinter.Button(input_frame, text="Library Spectra", command = self.getInputLibraryFile).grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inputLibraryEntry = Tkinter.Entry(input_frame, width = 70)
		self.inputLibraryEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.inputLibraryEntry.insert(0,"/Users/ben/Desktop/test_sm/lib_spec_csv/0816-2139_.csv")
		
		r = r + 1
		
		Tkinter.Button(input_frame, text="Observed Spectra", command = self.getInputObservedFile).grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.inputObservedEntry = Tkinter.Entry(input_frame, width = 70)
		self.inputObservedEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.inputObservedEntry.insert(0,"/Users/ben/Desktop/test_sm/obs_spec_csv/0816-2139.csv")
		
		r = r + 1
		
		Tkinter.Button(input_frame, text="Output Directory", command = self.getOutputDirectory).grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.outDirectoryEntry = Tkinter.Entry(input_frame, width = 70)
		self.outDirectoryEntry.grid(row = r, column = 1, sticky = Tkinter.W)
		self.outDirectoryEntry.insert(0,"/Users/ben/Desktop/test_sm/SAM")
		
		r += 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, columnspan = 2, pady = 10, sticky = Tkinter.NW)
		
		r += 1
		
		Tkinter.Label(input_frame, text = "Note: used only if observed spectra are in CSV format. Click 'HELP' for details.").grid(row = r, column = 0, columnspan = 2, sticky = Tkinter.NW)
		
		r += 1
		
		Tkinter.Label(input_frame, text="Columns to skip").grid(row = r, column = 0, sticky = Tkinter.NW)
		self.columns_to_skip_entry = Tkinter.Entry(input_frame, width = 7)
		self.columns_to_skip_entry.grid(row = r, column = 1, sticky = Tkinter.NW)
		
		r += 1
		
		Tkinter.Label(input_frame, text="Column as ID").grid(row = r, column = 0, sticky = Tkinter.NW)
		self.column_as_id_entry = Tkinter.Entry(input_frame, width = 7)
		self.column_as_id_entry.grid(row = r, column = 1, sticky = Tkinter.NW)
		
		###################################################################
		# Execute options, range frame, threshold frame
		###################################################################
		
		f = Tkinter.Frame(left_frame)
		f.grid(row = 1, column = 0, sticky = Tkinter.NW, pady = 10)
		
		
		#
		# range frame
		#
		range_frame = Tkinter.Frame(f)
		range_frame.grid(row = 0, column = 0, sticky = Tkinter.NW)
		
		r = 0
		
		Tkinter.Label(range_frame, text = "COMPARISON RANGES").grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		self.max_ranges = 5
		self.static_ranges = []
		self.static_range_frame = Tkinter.Frame(range_frame)
		self.static_range_frame.grid(row = r, column = 0, sticky = Tkinter.W, padx = 10)
		Tkinter.Label(self.static_range_frame, text = "From").grid(row = 0, column = 0, sticky = Tkinter.W)
		Tkinter.Label(self.static_range_frame, text = "To").grid(row = 0, column = 1, sticky = Tkinter.W)
		
		for i in range(self.max_ranges):
			self.add_compare_range(i+1)
		
		
		
		#
		# execute options frame
		#
		exe_opt_frame = Tkinter.Frame(f)
		exe_opt_frame.grid(row = 1, column = 0, pady = 10, sticky = Tkinter.NW)
			
		r = 0
		
		Tkinter.Label(exe_opt_frame, text = "EXECUTION OPTIONS").grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		self.runSAM = Tkinter.IntVar()
		Tkinter.Checkbutton(exe_opt_frame, text = "Spectral Angle Mapping", variable = self.runSAM).grid(row = r, column = 0, sticky = Tkinter.W)
		
		r = r + 1
		
		self.runMatching = Tkinter.IntVar()
		Tkinter.Checkbutton(exe_opt_frame, text = "Spectral Matching", variable = self.runMatching).grid(row = r, column = 0, sticky = Tkinter.W)
		
		Tkinter.Frame(f).grid(row = 0, column = 1, padx = 10)
		#
		# threshold frame
		#
		threshold_frame = Tkinter.Frame(f)
		threshold_frame.grid(row = 0, column = 2, rowspan = 2, sticky = Tkinter.NE)
		
		r = 0
		
		Tkinter.Label(threshold_frame, text = "THRESHOLD OPTIONS").grid(row = r, column = 0, sticky = Tkinter.W)
		
		r += 1
		
		self.no_threshold_intvar = Tkinter.IntVar()
		self.no_threshold_intvar.set(1)
		Tkinter.Checkbutton(threshold_frame, text = "None", variable = self.no_threshold_intvar, command = self.no_thresh_clicked).grid(row = r, column = 0, sticky = Tkinter.W)
		
		r += 1
		
		self.gb_threshold_intvar = Tkinter.IntVar()
		Tkinter.Checkbutton(threshold_frame, text = "Global", variable = self.gb_threshold_intvar, command = self.gb_thresh_clicked).grid(row = r, column = 0, sticky = Tkinter.W)
		
		r += 1
		
		self.gb_threshold_entry = Tkinter.Entry(threshold_frame, width = 12)
		self.gb_threshold_entry.grid(row = r, column = 0, padx = 5)
		
		r += 1
		
		self.pc_threshold_intvar = Tkinter.IntVar()
		Tkinter.Checkbutton(threshold_frame, text = "Per Class", variable = self.pc_threshold_intvar, command = self.pc_thresh_clicked).grid(row = r, column = 0, sticky = Tkinter.W)
		
		r += 1
		
		self.n_thresholds = 0
		self.sam_thresholds = []
		self.cls_thresh_frame = Tkinter.Frame(threshold_frame)
		self.cls_thresh_frame.grid(row = r, column = 0, sticky = Tkinter.NW, padx = 10)
		
		
		###################################################################
		###################### Message Frame ##############################
		###################################################################
		
		## status frame
		messageFrame = Tkinter.Frame(right_frame)
		messageFrame.grid(row = 0, column = 1, sticky = Tkinter.N, rowspan = 3, padx = 10, pady =10)
		
		resultLabel = Tkinter.Label(messageFrame, text = "Status Message...")
		resultLabel.grid(row = 0, column = 0, sticky = Tkinter.NW)
		
		self.textFrame = Tkinter.Frame(messageFrame)
		self.scroll = Tkinter.Scrollbar(self.textFrame)
		
		self.messageWindow = Tkinter.Text(self.textFrame, height = 31, width = 80, background = 'white', borderwidth = 1, relief = Tkinter.RIDGE, yscrollcommand=self.scroll.set)
		self.messageWindow.pack(side=Tkinter.LEFT);
		
		self.scroll.pack(side=Tkinter.RIGHT, fill=Tkinter.Y)
		self.scroll.config(command=self.messageWindow.yview)
		self.textFrame.grid(row = 1, column = 0);
		
		
		###################################################################
		###################### Execute Frame ##############################
		###################################################################
		
		exe_frame = Tkinter.Frame(left_frame)
		exe_frame.grid(row = 3, column = 0, padx = 0, pady = 0, sticky = Tkinter.NW)
		
		Tkinter.Button(exe_frame, text= "Start", command = self.compute_metrics).grid(row = 0, column = 0, sticky = Tkinter.SW)
		Tkinter.Button(exe_frame, text= "HELP", command = self.showHelp).grid(row = 0, column = 1)
		
		
	def add_threshold(self, row_id, label, thr):
		
		print "add_threshold:" + str(row_id) + "," + label + "," + thr
		
		frame = Tkinter.Frame(self.cls_thresh_frame)
		frame.grid(row = row_id, column = 0, sticky = Tkinter.NW)
		
		lbl_frame = Tkinter.Frame(frame)
		lbl_frame.grid(row = 0, column = 0, sticky = Tkinter.NW)
		thr_label = Tkinter.Label(lbl_frame, text = label, font = ("Courier"))
		thr_label.grid(row = 0, column = 0, sticky = Tkinter.NW, padx = 5)
		thr_entry = Tkinter.Entry(frame, width = 12)
		thr_entry.grid(row = 0, column = 1, sticky = Tkinter.NW, padx = 5)
		thr_entry.insert(0, str(thr))
		
		self.sam_thresholds.append({"sam_frame": frame, "sam_label": label, "sam_entry": thr_entry})
		
	def update_perclass_thr(self, path):
		
		print "update_perclass_thr:" + str(len(self.sam_thresholds))
		
		# clear current per class threshold values
		for i in range(len(self.sam_thresholds)):
			d = self.sam_thresholds[i]
			f = d["sam_frame"]
			f.destroy()
			
		self.sam_thresholds = []
		
		# set per class thresholds to 0.0 - thresholds are only set by 
		# lib vs lib
		f = open(path)
		c = []
		f.readline()
		for line in f.readlines():
			cls = line.strip().split(",")[0]
			pos = cls.find("_")
			if pos != -1:
				cls = cls[:pos]
			if not cls in c:
				c.append(cls)
		
		for i in xrange(len(c)):
			self.add_threshold(i, c[i], "0.0")
			
		f.close()
		
			
	def update(self):
		
		# clear current per class threshold values
		for i in range(len(self.sam_thresholds)):
			d = self.sam_thresholds[i]
			f = d["sam_frame"]
			f.destroy()
			
		self.sam_thresholds = []
		
		# update per class threshold values if they have been set by lib vs lib
		if self.clipBoard["spectral_library"] != "":
			
			self.inputLibraryEntry.delete(0, Tkinter.END)
			self.inputLibraryEntry.insert(0, self.clipBoard["spectral_library"])
			
			thresholds = self.clipBoard["sam_classifier_thresholds"]
			labels = self.clipBoard["sam_class_names"]
			
			if len(labels) != len(thresholds):
				self.writeMessage(str(len(labels)) + ", " + str(len(thresholds)))
				return
				
			for i in range(len(thresholds)):
				self.add_threshold(i, labels[i], str(thresholds[i]))
		
		# clear the clip board
		self.clipBoard["spectral_library"] = ""
		self.clipBoard["sam_classifier_thresholds"] = []
		self.clipBoard["sam_class_names"] = []
		
	def showHelp(self):
		top = Tkinter.Toplevel()
		top.title("Spectral Metrics Help")
		txt = Tkinter.Text(top)
		txt.insert(Tkinter.END, self.helpMessage)
		txt.update()
		txt.pack(fill = Tkinter.BOTH, expand = 1)
		
	def no_thresh_clicked(self):
		if self.no_threshold_intvar.get() == 1:
			self.gb_threshold_intvar.set(0)
			self.pc_threshold_intvar.set(0)
			
	def gb_thresh_clicked(self):
		if self.gb_threshold_intvar.get() == 1:
			self.no_threshold_intvar.set(0)
			self.pc_threshold_intvar.set(0)
	
	def pc_thresh_clicked(self):
		if self.pc_threshold_intvar.get() == 1:
			self.no_threshold_intvar.set(0)
			self.gb_threshold_intvar.set(0)
		
		
	def add_compare_range(self, row_id):
		
		lr_entry  = Tkinter.Entry(self.static_range_frame, width = 15)
		lr_entry.grid(row = row_id, column = 0, sticky = Tkinter.W, padx = 5)
		hr_entry  = Tkinter.Entry(self.static_range_frame, width = 15)
		hr_entry.grid(row = row_id, column = 1, sticky = Tkinter.W, padx = 5)
		
		self.static_ranges.append({"lr_entry": lr_entry, "hr_entry": hr_entry})
		
	def get_compare_ranges(self):	
		
		static_ranges = []
		for d in self.static_ranges:
			try:				
				static_ranges.append([float(d["lr_entry"].get()), float(d["hr_entry"].get())])
			except ValueError:
				if d["lr_entry"].get() != "" or d["hr_entry"].get() != "":
						self.writeMessage("Warning: invalid range; %s,%s" % (d["lr_entry"].get(), d["hr_entry"].get()))
				continue
				
		return static_ranges
		
	def get_lib_class_names(self, path):
		
		f = open(path)
		lines = f.readlines()
		f.close()
		
		lines = [line.strip().split(",") for line in lines[1:]]
		S     = []
		
		for line in lines:
			cls = line[0].strip()
			pos = cls.find("_")
			if pos != -1:
				cls = cls[:pos]
			S.append(cls)
		
		return S
		
	def getInputLibraryFile(self):
		infile = tkFileDialog.askopenfilename()
		if(infile != ""):
			self.inputLibraryEntry.delete(0,Tkinter.END)
			self.inputLibraryEntry.insert(0,infile)
			self.update_perclass_thr(infile)
			
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
	## Get library spectra sample names
	##
	def get_lib_sample_names(self, lib_path):
		
		f = open(lib_path, "r")
		lines = f.readlines()
		f.close()
		
		lines  = lines[1:]
		cnames = []
		
		for line in lines:
			l = line.strip().split(",")
			cnames.append(l[0].strip())
			
		return cnames
		
	##
	## Build headers for output of 'envi_metrics'.
	##
	def write_headers(self, keys, keyval, rule_hdr_path, csfy_hdr_path, lib_path):
		
		keep_fields = ["description","samples","lines","bands","header offset","file type","data type","interleave","byte order","band names","x start","y start"]
		remo_fields = []
		
		#
		# removel all fields not in 'keep_fields
		#
		for key in keyval.keys():
			if not( key in keep_fields ):
				remo_fields.append(key)
				
		for key in remo_fields:
			keyval.pop(key)
			keys.remove(key)
			
		
		header = self.get_lib_sample_names(lib_path)
		
		#
		# build 'band names' string
		#
		h_str = "{"
		for i in range(len(header)):
			if i == len(header) - 1:
				h_str = h_str + header[i] + "}\n"
			else:
				h_str = h_str + header[i] + ","
			
		if not( "byte order" in keys ):
			keys.append("byte order")
			
		keyval["byte order"] = "0\n"
		
		if not( "band names" in keys ):
			keys.append("band names")
		
		keyval["band names"] = h_str
		keyval["bands"] = str(len(header)) + "\n"
		keyval["interleave"] = "bsq\n"
		keyval["data type"] = "4\n"
		
		envi_header_io.write_as_is(rule_hdr_path, keys, keyval)
		
		#
		# modify key-value header pairs for classification file
		#
		keyval["bands"] = "1\n"
		keyval["band names"] = "{Classification}\n"
		keyval["data type"] = "12\n"
		
		envi_header_io.write_as_is(csfy_hdr_path, keys, keyval)
		
	
	##
	## Compute SAM and/or Spectral Matching on ENVI input.
	##
	def envi_metrics(self, fnm_base, out_sam_base, out_mtc_base, lib_path, obs_path, wl_low, wl_high, run_sam, run_mtc, class_thr):
		
		#change to base path
		obs_path = obs_path[:obs_path.rfind(".")]
		
		sam_rule_dat_path = out_sam_base + "/Rule_" + fnm_base + ".dat"
		sam_rule_hdr_path = out_sam_base + "/Rule_" + fnm_base + ".hdr"
		sam_csfy_dat_path = out_sam_base + '/Classified_' + fnm_base + ".dat"
		sam_csfy_hdr_path = out_sam_base + '/Classified_' + fnm_base + ".hdr"
		
		mtc_rule_dat_path = out_mtc_base + "/Rule_" + fnm_base + ".dat"
		mtc_rule_hdr_path = out_mtc_base + "/Rule_" + fnm_base + ".hdr"
		mtc_csfy_dat_path = out_mtc_base + '/Classified_' + fnm_base + ".dat"
		mtc_csfy_hdr_path = out_mtc_base + '/Classified_' + fnm_base + ".hdr"
		
		self.writeMessage("   Spectral Metrics ENVI")
		SM_module.SpectralMetrics(
			lib_path,
			obs_path,
			sam_rule_dat_path,
			sam_csfy_dat_path,
			mtc_rule_dat_path,
			mtc_csfy_dat_path,
			wl_low,
			wl_high,
			run_sam,
			run_mtc,
			class_thr,
			len(class_thr))
		
		self.writeMessage("   Generating ENVI headers");
		
		if run_sam == 1:
			
			(keys, key_val) = envi_header_io.read_as_is(obs_path + ".hdr")
			self.write_headers(keys, key_val, sam_rule_hdr_path, sam_csfy_hdr_path, lib_path)
			
		if run_mtc == 1:
			
			(keys, key_val) = envi_header_io.read_as_is(obs_path + ".hdr")
			self.write_headers(keys, key_val, mtc_rule_hdr_path, mtc_csfy_hdr_path, lib_path)
		
	##
	## Compute SAM and/or Spectral Matching on CSV input.
	##
	def csv_metrics(self, obs_base, lib_base, out_sam_base, out_mtc_base, lib_path, obs_path, wl_low, wl_high, run_sam, run_mtc, class_thr):
		
		col_skip = int(self.columns_to_skip_entry.get())
		col_id   = int(self.column_as_id_entry.get())-1 #count from 0
		
		sam_path = out_sam_base + "/SAM_" + obs_base + "_" + lib_base + ".csv"
		mtc_path = out_mtc_base + "/PC_"  + obs_base + "_" + lib_base + ".csv"
		
		self.writeMessage("   Spectral Metrics CSV")
		SM_module.SpectralMetrics_CSV(
			lib_path,
			obs_path,
			sam_path,
			mtc_path,
			wl_low,
			wl_high,
			run_sam,
			run_mtc,
			class_thr,
			len(class_thr),
			col_skip,
			col_id)
	
	##
	## Compute the SAM and/or Spectral Matching
	##
	def compute_metrics(self):
		
		try:
			lib_path = self.inputLibraryEntry.get()		#csv
			obs_path = self.inputObservedEntry.get()	#csv or envi
			out_dir  = self.outDirectoryEntry.get()
			run_sam  = self.runSAM.get()
			run_mtc  = self.runMatching.get()
			
			if run_sam == 0 and run_mtc == 0:
				self.writeMessage("Must select one of SAM or Spectral Matching")
				return
			
			no_thr 	 = self.no_threshold_intvar.get()
			gb_thr   = self.gb_threshold_intvar.get()
			pc_thr   = self.pc_threshold_intvar.get()
			
			if no_thr == 0 and gb_thr == 0 and pc_thr == 0:
				self.writeMessage("Error: select a thresholding option.")
				return
				
			
			S = self.get_lib_class_names(lib_path)
			
			#
			# build sample threshold list
			#
			class_thr = [0.]*len(S)
			
			if gb_thr == 1:
				thr = float(self.gb_threshold_entry.get())
				for i in xrange(len(class_thr)):
					class_thr[i] = thr
			
			if pc_thr == 1:
				d = {}
				
				print "LEN:" + str(len(self.sam_thresholds))
				
				for dic in self.sam_thresholds:
					
					key = dic["sam_label"]
					
					if not "sam_entry" in dic:
						print "sam_entry not in dic"
						
					print "key:" + key
					print "preval: " + dic["sam_entry"].get()
					
					val = float( dic["sam_entry"].get() )
					
					print "val:" + str(val)
					
					d[key] = val
					
					print key, " = ", val
					
				for i in xrange(len(class_thr)):
					class_thr[i] = d[S[i]]
			
			print "done loop"
			#
			# loop through each compare range
			#
			compare_ranges = self.get_compare_ranges()
			
			if len(compare_ranges) == 0:
				self.writeMessage("Error: no valid compare range.")
				return
			
			for compare_range in compare_ranges:
				
				wl_low  = compare_range[0]
				wl_high = compare_range[1]
				
				self.writeMessage("Processing range: " + str(wl_low) + " => " + str(wl_high))
				
				lib_fnm_base = lib_path[lib_path.rfind('/')+1:lib_path.rfind('.')]
				obs_fnm_base = obs_path[obs_path.rfind('/')+1:obs_path.rfind('.')]
				out_sam_base = out_dir + "/SAM/" + str(wl_low) + "-" + str(wl_high)
				out_mtc_base = out_dir + "/SpectralMatching/" + str(wl_low) + "-" + str(wl_high)
				
				if not(os.path.exists(out_sam_base)) and run_sam == 1:
					os.makedirs(out_sam_base)
					
				if not(os.path.exists(out_mtc_base)) and run_mtc == 1:
					os.makedirs(out_mtc_base)
				
				if obs_path.endswith(".hdr"):
					try:
						self.envi_metrics(lib_fnm_base, out_sam_base, out_mtc_base, lib_path, obs_path, wl_low, wl_high, run_sam, run_mtc, class_thr)
					except:
						self.writeMessage("Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
					
				elif obs_path.endswith(".csv"):
					try:
						self.csv_metrics(obs_fnm_base, lib_fnm_base, out_sam_base, out_mtc_base, lib_path, obs_path, wl_low, wl_high, run_sam, run_mtc, class_thr)
					except:
						self.writeMessage("Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
				
				else:
					self.writeMessage("Error: file should be '.hdr' or '.csv'")
					return
			
			self.writeMessage("Done!")

		except:
			self.writeMessage("Error: " + str(sys.exc_info()[0]) + str(sys.exc_info()[1]) + str(sys.exc_info()[2]))
		
		
		
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
