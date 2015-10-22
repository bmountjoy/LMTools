

import webbrowser
import matplotlib
matplotlib.use('tkagg')
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure # top level container for all plot elements
import matplotlib.pyplot as pp
import Tkinter
import tkFileDialog
import math
import sys
import numpy

import scipy
from scipy.stats import pearsonr
from scipy.spatial.distance import cosine

class Init:

	def __init__(self, root, clipBoard):
		
		self.clipBoard = clipBoard
		self.cur = 0
		self.n_lib_classes = 0
		self.rec = []
		self.idx = []
		self.cls = []
		self.cnt = {}
		self.sam = []
		self.pc  = []
		self.sam_string = "Spectral_Anlge_Mapping"
		self.mtc_string = "Spectral_Matching"
		
		self.helpMessage = "LIB VS LIB\n\n" + \
			"Library Spectra: CSV file with the spectra organized across each row.\n" + \
			" - the first row is a header and should contain column labels\n" +\
			" - each subsequent row contains contains the spectra, where the first column of each row contains the spectrum label\n\n" +\
			"Output Directory: Root directory to which performance statistics are written.\n\n" + \
			"Subset Spectra by Wavelength\n\n" + \
			" - if clicked, performs SAM and Matching calculations on the wavelenths within the provided range"
		
		#resize frame
		Tkinter.Grid.rowconfigure(root, 0, weight = 1)
		Tkinter.Grid.columnconfigure(root, 0, weight = 1)
		
		frame = Tkinter.Frame(root)
		frame.grid(row = 0, column = 0, sticky = Tkinter.N + Tkinter.S + Tkinter.E + Tkinter.W)
		#resize left_frame and right_frame
		Tkinter.Grid.rowconfigure(frame, 0, weight = 1)
		Tkinter.Grid.columnconfigure(frame, 0, weight = 1)		
		
		# input, message, and execute frames
		left_frame = Tkinter.Frame(frame)
		left_frame.pack(side = Tkinter.LEFT, fill = Tkinter.BOTH, expand = 0)
		
		# for plot frame
		right_frame = Tkinter.Frame(frame)
		right_frame.pack(side = Tkinter.RIGHT, fill = Tkinter.BOTH, expand = 1)
		
		###################################################################
		###################### Input Frame ################################
		###################################################################
		
		input_frame = Tkinter.Frame(left_frame)
		input_frame.grid(row = 0, column = 0)
		
		#grid row
		r = 0
		
		Tkinter.Button(input_frame, text="Library Spectra", command = self.get_lib_file).grid(row = r, column = 0, sticky = Tkinter.W)
		
		self.in_lib_entry = Tkinter.Entry(input_frame, width = 70)
		self.in_lib_entry.grid(row = r, column = 1)
		self.in_lib_entry.insert(0, "/Users/ben/Desktop/lib_vs_lib_forroger/IndexAllLines_joined_OUT4.csv")
		
		r += 1
		
		Tkinter.Button(input_frame, text="Output Directory:", command = self.get_out_dir).grid(row = r, column = 0, pady = 0, sticky = Tkinter.W)
		
		self.out_dir_entry = Tkinter.Entry(input_frame, width = 70)
		self.out_dir_entry.grid(row = r, column = 1)
		self.out_dir_entry.insert(0, "/Users/ben/Desktop/lib_vs_lib_forroger/out")
		
		r += 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, columnspan = 2, pady = 10)
		
		r += 1
		
		Tkinter.Label(input_frame, text = "Display Options").grid(row = r, column = 0, sticky = Tkinter.NW)
		
		r += 1
		
		self.show_sam_intvar = Tkinter.IntVar()
		self.show_sam_intvar.set(1)
		Tkinter.Checkbutton(input_frame, text="Spectral Angle Mapping", variable = self.show_sam_intvar, command = self.show_sam_clicked).grid(row = r, column = 0, columnspan = 2, sticky= Tkinter.NW, padx = 10)
		
		r += 1
		
		self.show_pc_intvar = Tkinter.IntVar()
		Tkinter.Checkbutton(input_frame, text="Spectral Matching", variable = self.show_pc_intvar, command = self.show_pc_clicked).grid(row = r, column = 0, columnspan = 2, sticky= Tkinter.NW, padx = 10)
		
		r += 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, columnspan = 2, pady = 5)
		
		r += 1
		
		#wavelength subset
		
		self.subset_range_intvar = Tkinter.IntVar()
		Tkinter.Checkbutton(input_frame, variable = self.subset_range_intvar, text = "Subset Spectra by Wavelength").grid(row = r, column = 0, columnspan = 2, sticky = Tkinter.NW)
		
		r += 1
		
		ssr_frame = Tkinter.Frame(input_frame)
		ssr_frame.grid(row = r, column = 0, columnspan = 2, padx = 20, sticky = Tkinter.NW)
		
		self.low_range_entry = Tkinter.Entry(ssr_frame)
		self.low_range_entry.pack(side = Tkinter.LEFT, fill = Tkinter.BOTH, expand = 1)
		
		self.high_range_entry = Tkinter.Entry(ssr_frame)
		self.high_range_entry.pack(side = Tkinter.RIGHT, fill = Tkinter.BOTH, expand = 1)
		
		r += 1
		
		Tkinter.Frame(input_frame).grid(row = r, column = 0, columnspan = 2, pady = 5)
		
		r += 1
		
		Tkinter.Label(input_frame, text = "DERIVED CLASS THRESHOLDS").grid(row = r, column = 0, pady = 5, sticky = Tkinter.NW)
		
		r += 1
		
		self.n_thresholds = 5
		self.sam_thresholds = []
		self.cls_thresh_frame = Tkinter.Frame(input_frame)
		self.cls_thresh_frame.grid(row = r, column = 1, sticky = Tkinter.NW)
		
		
		###################################################################
		###################### Message Frame ##############################
		###################################################################
		
		## status frame
		messageFrame = Tkinter.Frame(left_frame)
		messageFrame.grid(row = 1, column = 0, sticky = Tkinter.NW, padx = 10, pady =10)
		
		resultLabel = Tkinter.Label(messageFrame, text = "Status Message...")
		resultLabel.grid(row = 0, column = 0, sticky = Tkinter.NW)
		
		self.textFrame = Tkinter.Frame(messageFrame)
		self.scroll = Tkinter.Scrollbar(self.textFrame)
		
		self.messageWindow = Tkinter.Text(self.textFrame, height = 14, width = 110, background = 'white', borderwidth = 1, relief = Tkinter.RIDGE, yscrollcommand=self.scroll.set)
		self.messageWindow.pack(side=Tkinter.LEFT);
		
		self.scroll.pack(side=Tkinter.RIGHT, fill=Tkinter.Y)
		self.scroll.config(command=self.messageWindow.yview)
		self.textFrame.grid(row = 1, column = 0);
		
		
		###################################################################
		######################### Plot Frame ##############################
		###################################################################
		
		plot_frame = Tkinter.Frame(right_frame)
		plot_frame.pack(fill = Tkinter.BOTH, expand = 1)
		
		f = Figure(figsize=(5,4), dpi=100)
		self.fig = f
		self.subplot = f.add_subplot(1,1,1)
		
		canvas = FigureCanvasTkAgg(f, master=plot_frame)
		canvas.show()
		canvas.get_tk_widget().pack(side = Tkinter.TOP, fill = Tkinter.BOTH, expand = 1)
		self.canvas = canvas
		
		toolbar = NavigationToolbar2TkAgg( canvas, plot_frame )
		toolbar.update()
		canvas._tkcanvas.pack(side=Tkinter.TOP, fill=Tkinter.BOTH, expand=1)
		
		#add control buttons
		self.prev_plot_button = Tkinter.Button(plot_frame, text = "<<", command = self.plot_prev, state = Tkinter.DISABLED)
		self.prev_plot_button.pack(side = Tkinter.LEFT)
		self.next_plot_button = Tkinter.Button(plot_frame, text = ">>", command = self.plot_next, state = Tkinter.DISABLED)
		self.next_plot_button.pack(side = Tkinter.LEFT)
		
		
		###################################################################
		###################### Execute Frame ##############################
		###################################################################
		
		exe_frame = Tkinter.Frame(left_frame)
		exe_frame.grid(row = 2, column = 0, sticky = Tkinter.W, pady = 10)
		
		Tkinter.Button(exe_frame, text="Start!", command = self.start).grid(row = 0, column = 0, sticky = Tkinter.SW)
		Tkinter.Button(exe_frame, text="HELP!", command = self.showHelp).grid(row = 0, column = 1, sticky = Tkinter.SW)
		
			
	##
	## Interface functions
	##
	
	def showHelp(self):
		top = Tkinter.Toplevel()
		top.title("Spectral Metrics Help")
		txt = Tkinter.Text(top)
		txt.insert(Tkinter.END, self.helpMessage)
		txt.update()
		txt.pack(fill = Tkinter.BOTH, expand = 1)
		
		
	def add_threshold(self, row_id, lbl, thr):
		
		frame = Tkinter.Frame(self.cls_thresh_frame)
		frame.grid(row = row_id, column = 0, sticky = Tkinter.NW)
		
		thr_label = Tkinter.Label(frame, text = lbl, font=("Courier"))
		thr_label.grid(row = 0, column = 0, sticky = Tkinter.NW)
		thr_entry = Tkinter.Entry(frame, width = 12)
		thr_entry.grid(row = 0, column = 1, sticky = Tkinter.NW, padx = 5)
		thr_entry.insert(0, str(thr))
		
		self.sam_thresholds.append({"frame": frame, "sam_label": thr_label, "sam_entry": thr_entry})
		
	def get_lib_file(self):
		f = tkFileDialog.askopenfilename()
		if(f != ""):
			self.in_lib_entry.delete(0,Tkinter.END)
			self.in_lib_entry.insert(0,f)
			
	def get_out_dir(self):
		outdir = tkFileDialog.askdirectory()
		if(outdir != ""):
			self.out_dir_entry.delete(0,Tkinter.END)
			self.out_dir_entry.insert(0,outdir)
			
	def write_message(self, text):
		self.messageWindow.insert(Tkinter.END, text)
		self.messageWindow.update()
		self.messageWindow.yview(Tkinter.END)
		
	
	def show_sam_clicked(self):
		if self.show_sam_intvar.get() == 1:
			self.show_pc_intvar.set(0)
			self.update_thresholds(self.sam)
			self.make_plots()
			
	def show_pc_clicked(self):
		if self.show_pc_intvar.get() == 1:
			self.show_sam_intvar.set(0)
			self.update_thresholds(self.pc)
			self.make_plots()
	
	##
	## Plotting functions
	##
	
	def plot_prev(self):
		self.cur = (self.cur - 1) % self.n_lib_classes
		self.make_plots()
			
	def plot_next(self):
		
		self.cur = (self.cur +  1) % self.n_lib_classes
		self.make_plots()
		
	def mean(self, x):
		x_mean = 0.0
		for item in x:
			x_mean += item
		return x_mean / len(x)
	
	def compute_SAM(self, x, y):
		
		xx = yy = xy = 0.0
		
		for i in xrange(len(x)):
			xy += x[i]*y[i]
			xx += x[i]*x[i]
			yy += y[i]*y[i]
			
		return xy / math.sqrt(xx * yy)
		#return math.acos(xy / math.sqrt(xx * yy))
	
	def compute_PC(self, x,y):
		
		if len(x) == 0:
			print "len(x) == 0 wtf!!"
			
		x_mean = self.mean(x)
		y_mean = self.mean(y)
		
		xy = sx = sy = 0.0
		
		for i in xrange(len(x)):
			dx = x[i] - x_mean
			dy = y[i] - y_mean
			xy += dx * dy
			sx += dx * dx
			sy += dy * dy
			
		if sx == 0 or sy == 0:
			return 1
		return xy / (math.sqrt(sx) * math.sqrt(sy))
		
		
	def buffer_lib(self, path):
		
		rec = [] #list of records in the library
		idx = [] #maps record to class name
		cls = [] #list of classes
		cnt = {} #maintains a count of each class
		cls_to_cls_idx = {} #tracks the index of the class name of the associated classs in cls
		
		f = open(path, "r")
		
		#read header
		hdr = f.readline().strip().split(",")
		hdr = hdr[1:]
		
		#determine wavelengths
		wave = [s for s in hdr]
		spec = []
		
		lines = [line.strip().split(',') for line in f.readlines()]
		
		cls_idx = 0
		for line in lines:
			r = line[0] 		#library record
			pos = r.find("_")
			cl  = ""			#class of record
			if pos == -1:
				cl = r
			else:
				cl = r[:pos]
				
			if cl in cnt:
				cnt[cl] = cnt[cl] + 1
			else:
				cnt[cl] = 1
				cls.append(cl)
				cls_to_cls_idx[cl] = cls_idx
				cls_idx += 1
			
			rec.append(r)
			idx.append(cls_to_cls_idx[cl])
			
			line = line[1:]
			spec.append([float(s) for s in line])
					
		f.close()
		
		self.rec = rec
		self.idx = idx
		self.cls = cls
		self.cnt = cnt
		self.n_lib_classes = len(cls)
		
		return (rec, idx, cls, cnt, wave, spec)
	
	def compute_extents(self, wl_low, wl_high, wave):
		
		n_wave = len(wave)
		
		if n_wave < 3:
			raise Exception("Error: library members must have at least 3 values")
		
		if wave[0] > wl_low or wl_low >= wl_high or wave[n_wave-1] < wl_high:
			raise Exception("Error: bad subset extents")
		
		min = 9999.0
		low = 0
		
		for i in range(n_wave):
			diff = math.fabs(wave[i] - wl_low)
			if diff < min:
				low = i
				min = diff
		
		min  = 9999.0
		high = 0
		
		for i in range(n_wave):
			diff = math.fabs(wave[i] - wl_high)                                             
			if diff < min:
				high = i
				min = diff
		
		return (low, high+1)
	
	def subset_by_extent(self, low, high, wave, spec):
		
		self.write_message("subset_by_extent\n")
		
		wave = wave[low:high]
		
		for i in range(len(spec)):
			spec[i] = spec[i][low:high]
			
		return (wave, spec)
	
	def lib_vs_lib(self, spectra):
			
		pc = [range(len(spectra)) for i in range(len(spectra))]
		sm = [range(len(spectra)) for i in range(len(spectra))]    
		
		#symmetric matrix with 0's along the diagonal
		for i in xrange(len(spectra)):
			for j in xrange(len(spectra)):
				if i == j:
					pc[i][j] = 0.0
					sm[i][j] = 0.0
				else:
					#pc[i][j],temp = pearsonr(scipy.array(spectra[i]), scipy.array(spectra[j]))
					#sm[i][j] = math.acos(cosine(scipy.array(spectra[i]), scipy.array(spectra[j])))
					pc[i][j] = self.compute_PC (spectra[i], spectra[j])
					sm[i][j] = self.compute_SAM(spectra[i], spectra[j])
					
		self.sam = sm;
		self.pc  = pc;
		return(sm, pc)
				
				
	def min_index(self, x):
		mini = 0
		mine = 999.0
		for i in range(len(x)):
			if x[i] < mine:
				mini = i
				mine = x[i]
		return mini
		
	def max_index(self, x):
		maxi = 0
		maxe = -999.0
		for i in range(len(x)):
			if x[i] > maxe:
				maxi = i
				maxe = x[i]
		return maxi
		
	
	def print_header(self, f, hdr):
		
		l_hdr = len(hdr)
		
		f.write(",")
		for i in range(l_hdr):
			if i == l_hdr-1:
				f.write(hdr[i] + "\n")
			else:
				f.write(hdr[i] + ",")
				
	def print_table_with_header(self, f, hdr, tbl):
		
		self.print_header(f, hdr)
		
		for i in range(len(tbl)):
			f.write(hdr[i] + ",")
			for j in range(len(tbl)):
				if j == len(tbl)-1:
					f.write("%.4f\n" % tbl[i][j])
				else:
					f.write("%.4f\t" % tbl[i][j])
			
	def write_classification_table(self, f, rec, mtr):
		
		f.write("<table border = 1>")
		f.write("<tr><td></td>")
		for s in rec:
			f.write("<td>%s</td>" % (s))
		f.write("</tr>\n")
		
		i = 0
		for row in mtr:
			f.write("<tr><td>%s</td>"%(rec[i]))
			for x in row:
				f.write("<td>%.4f</td>"%(x))
			f.write("</tr>\n")
			i += 1
			
		f.write("</table>\n")
		
	def write_classification_table_csv(self, f, rec, mtr):
		
		f.write(",")
		for i in xrange(len(rec)):
			s = rec[i]
			if i == len(rec)-1:
				f.write("%s\n" % (s))
			else:
				f.write("%s," % (s))
		
		i = 0
		for row in mtr:
			f.write("%s," % (rec[i]))
			j = 0
			for x in row:
				if j == len(row)-1:
					f.write("%.4f\n" % (x))
				else:
					f.write("%.4f," % (x))
				j += 1
			i += 1
			
		f.write("\n")
		
	def write_best_matches_table(self, f, mtr, rec): #, metric_is_sam):
		
		f.write("<table border = 1><tr><td>Know</td><td>Best Match</td><td>Score</td></tr>")
		
		i = 0
		for row in mtr:
			j = 0
			"""
			if metric_is_sam:
				j = self.min_index(row)
			else:
				j = self.max_index(row)
			"""
			j = self.max_index(row)
			f.write("<tr><td>%s</td><td>%s</td><td>%.4f</td></tr>\n" % (rec[i], rec[j], mtr[i][j]))
			i += 1
		
		f.write("</table>\n")
		
	def write_best_matches_table_csv(self, f, mtr, rec): #, metric_is_sam):
		
		f.write("Know,Best Match,Score\n")
		
		i = 0
		for row in mtr:
			j = 0
			"""
			if metric_is_sam:
				j = self.min_index(row)
			else:
				j = self.max_index(row)
			"""
			j = self.max_index(row)
			f.write("%s,%s,%.4f\n" % (rec[i], rec[j], mtr[i][j]))
			i += 1
		
		f.write("\n")
		
	def write_confusion_matrix(self, f, scores, cls):
		
		f.write("<table>")
		f.write("<tr><td></td>")
		for s in cls:
			f.write("<td>%s</td>" % (s))
		f.write("<td></td></tr>\n")
		
		for s in cls:
			f.write("<tr><td>%s</td>" % (s))
			tot = 0
			for ss in cls:
				f.write("<td>%d</td>" % (scores[s][ss]))
				tot += scores[s][ss]
			if tot == 0:
				f.write("<td>0.0</td></tr>\n")
			else:
				f.write("<td>%.4f</td></tr>\n" % (float(scores[s][s])/float(tot)))
			
		f.write("</table>\n")
		
	def write_confusion_matrix_csv(self, f, scores, cls):
		
		n_cls = len(cls)
		
		f.write(",")
		for i in range(n_cls):
			s = cls[i]
			if i == n_cls-1:
				f.write(s + "\n")
			else:
				f.write(s + ",")
		
		for i in range(n_cls):
			s = cls[i]
			
			f.write("%s," % (s))
			tot = 0
			for j in range(n_cls):
				ss = cls[j]
				f.write("%d," % (scores[s][ss]))
				tot += scores[s][ss]
			if tot == 0:
				f.write("0.0\n")
			else:
				f.write("%.4f\n" % (float(scores[s][s])/float(tot)))
		
	def report(self, path, rec, idx, cls, cnt, metric, metric_name):
		
		
		#metric_is_sam = False
		
		#if metric_name == self.sam_string:
		#	metric_is_sam = True
		
		#
		# generate confusion matrix scores
		#
		scores = {}
		for i in range(len(cls)):
			scores[cls[i]] = {}
			for j in range(len(cls)):
				scores[cls[i]][cls[j]] = 0.0
		
		if False:
			pass
			"""
			i = 0
			for row in metric:
				j = self.min_index(row)
				scores[cls[idx[i]]][cls[idx[j]]] += 1
				i += 1
			"""
		else:
		
			i = 0
			for row in metric:
				j = self.max_index(row)
				scores[cls[idx[i]]][cls[idx[j]]] += 1
				i += 1
		
		#
		# create .html report
		#
		f = open(path, "w")
		f.write("<!DOCTYPE html><html>\n")
		f.write("<head><style>table, td, th{border:1px solid black;}td{text-align:center;padding: 2px;}</style></head><body>\n")
		f.write("<h1>%s Classification Table</h1>" % (metric_name))
		
		self.write_classification_table(f, rec, metric)
		
		f.write("<h1>%s Best Matchs</h1>\n" %(metric_name))
		
		self.write_best_matches_table(f, metric, rec) #, metric_is_sam)
		
		f.write("<h1>%s Confusion Matrix</h1>\n" % (metric_name))
				
		self.write_confusion_matrix(f, scores, cls)	
		
		f.write("</body></html>")
		f.close()
		webbrowser.open_new("file://" + path)
		
		
		#
		# create .csv report
		#
		path = path[:path.rfind(".html")] + ".csv"
		f = open(path, "w")
		self.write_classification_table_csv(f, rec, metric)
		self.write_best_matches_table_csv(f, metric, rec) #, metric_is_sam)
		self.write_confusion_matrix_csv(f, scores, cls)	
		f.close()
		
		if (self.show_sam_intvar.get() == 1 and metric_name == self.sam_string) or \
		   (self.show_pc_intvar.get() == 1 and metric_name == self.mtc_string):
		   	   
			self.update_thresholds(metric)
			
	def update_thresholds(self, metric):
		
		idx = self.idx
		cls = self.cls
		
		if len(metric) == 0:
			return
		
		print len(self.sam_thresholds)
		
		for i in range(len(self.sam_thresholds)):
			ff = self.sam_thresholds[i]["frame"]
			ff.destroy()
			
		thresholds = []
		for cl in cls:
			cls_scores = []
			for i in range(len(metric)):
				for j in range(i):
					if cls[idx[i]] == cl and cls[idx[j]] == cl:
						cls_scores.append(metric[i][j])
			if len(cls_scores) == 0:
				thresholds.append(0.0)
			else:
				mini = self.min_index(cls_scores)
				thresholds.append(round(cls_scores[mini], 5))
				

		
		cbthrs = self.clipBoard["sam_classifier_thresholds"]
		cbclns = self.clipBoard["sam_class_names"]
		
		for i in range(len(cbthrs)):
			cbthrs.pop()
		for i in range(len(cbclns)):
			cbclns.pop()
		
		for i in range(len(cls)):
			self.add_threshold(i, cls[i], thresholds[i])
			cbthrs.append(thresholds[i])
			cbclns.append(cls[i])
		
	def make_plots(self):
		
		self.subplot.clear()
		
		met = None
		if self.show_sam_intvar.get() == 1:
			met = self.sam
		elif self.show_pc_intvar.get() == 1:
			met = self.pc
		
		if met == None or len(met) == 0:
			return
		
		x = []
		c = self.cls[self.cur]
		
		for i in range(len(met)):
			for j in range(i):
				if self.cls[self.idx[i]] == c and self.cls[self.idx[j]] == c:
					x.append(round(met[i][j],5))
		
		n = self.cnt[c]
		nplots = (n*n - n)/2 #-> len(x)
		
		self.subplot.set_xlabel("[%d/%d] %s vs %s : %d samples; %d plots" % (self.cur + 1, self.n_lib_classes, c, c, n, (n*n - n)/2))
		self.subplot.set_ylabel("Frequency")
		
		if nplots > 0:
			xx = numpy.array(x)
			self.subplot.hist(xx,15)
			
		self.canvas.draw()
		
		
	def start(self):
		
		try:
			in_file = self.in_lib_entry.get()
			out_dir = self.out_dir_entry.get()
			
			self.clipBoard["spectral_library"] = in_file
			
			self.write_message("Lib vs Lib\n")
			self.write_message("- buffering " + in_file[in_file.rfind("/")+1:] + "...\n")
			
			(rec, idx, cls, cnt, wave, spec) = self.buffer_lib(in_file)
			
			#
			# subset the spectra if user specifies so
			#
			if self.subset_range_intvar.get() == 1:
				
				#convert to floating point numbers
				for i in range(len(wave)):
					wave[i] = float(wave[i])
				
				l_range = float(self.low_range_entry.get())
				h_range = float(self.high_range_entry.get())
				
				(low, high)  = self.compute_extents(l_range, h_range, wave)
				
				if high - low < 3:
					self.write_message("Error: subset range must contain at least 3 wavelengths")
					
				(wave, spec) = self.subset_by_extent(low, high, wave, spec)
				
			
			self.write_message("- lib vs lib...\n")
			#
			# perform leave-one-out test on library
			#
			(sm, pc) = self.lib_vs_lib(spec)
			
			
			self.write_message("- generating report...\n")
			#
			# generate report and write results to file
			#
			out_sam = out_dir + "/" + self.sam_string + ".html"
			out_pc  = out_dir + "/" + self.mtc_string + ".html"
			
			self.report(out_sam, rec, idx, cls, cnt, sm, self.sam_string)
			
			self.report(out_pc , rec, idx, cls, cnt, pc, self.mtc_string)
			
			#
			# plot the results
			#
			self.make_plots()
			
			if len(cls) >= 1:
				self.prev_plot_button.config(state = Tkinter.NORMAL)
				self.next_plot_button.config(state = Tkinter.NORMAL)
			else:
				self.prev_plot_button.config(state = Tkinter.DISABLED)
				self.next_plot_button.config(state = Tkinter.DISABLED)
				
			self.write_message("Done!\n")
			
		except:
			self.clipBoard["spectral_library"] = ""
			self.clipBoard["sam_class_names"]  = []
			self.clipBoard["sam_classifier_thresholds"] = []
			self.write_message(sys.exc_info())
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
