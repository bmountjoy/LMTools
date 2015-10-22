from Tkinter import *
from tkFileDialog import *
from TM_module import *
import os
from sys import *

class Init:
	
	def __init__(self, master, clipBoard):
		
		self.clipBoard = clipBoard

		###################################################################
		####################### Input Frame ###############################
		###################################################################
                
		frame = Frame(master);
		frame.grid(row = 0, column = 0, sticky= NW, padx = 10, pady = 10);
		
		#input file button
		inputFileButton = Button(frame, text="Input File", command=self.get_in_file)
		inputFileButton.grid(row=0, column=0, sticky = W)
		
		#input text field
		self.in_text = Entry(frame, width = 50)
		self.in_text.grid(row=0, column=1)
		
		#output file button
		outFileButton = Button(frame, text="Output Directory", command=self.get_out_directory)
		outFileButton.grid(row=1, column=0, sticky = W)
		
		#output text field
		self.out_text = out_text = Entry(frame, width = 50)
		self.out_text.grid(row=1, column=1)
		
		
		###################################################################
		####################### Window Frame ##############################
		###################################################################
		
		windowContain = Frame(master)
		windowContain.grid(row = 1, column = 0, sticky = W, padx = 10)
		
		windowLabel = Label(windowContain, text = "Bounding Region", pady = 10)
		windowLabel.grid(row = 0, column = 0, sticky = W)
		
		windowFrame = Frame(windowContain);
		windowFrame.grid(row = 1, column = 0, sticky = W, padx = 10)
		
		r = 0
		
		self.coordinate_window = IntVar()
		coordCheckbutton = Checkbutton(windowFrame, variable = self.coordinate_window, command = self.coordinate_window_selected)
		coordCheckbutton.grid(row = r, column = 0, sticky = W)
		
		#window specification
		ulLabel = Label(windowFrame, text="Upper Left :")
		ulLabel.grid(row = r, column = 1, sticky=W)
		#upper left easting entry
		self.ulEastingEntry = Entry(windowFrame)
		self.ulEastingEntry.grid(row = r, column = 2, sticky=W)
		#bottom right easting entry
		self.ulNorthingEntry = Entry(windowFrame)
		self.ulNorthingEntry.grid(row = r, column = 3, sticky=W)
		
		r = r + 1

		brLabel = Label(windowFrame, text="Bottom Right :")
		brLabel.grid(row = r, column = 1, sticky=W)
		#upper left northing entry
		self.brEastingEntry = Entry(windowFrame)
		self.brEastingEntry.grid(row = r, column = 2, sticky=W)
		#bottom right northing entry
		self.brNorthingEntry = Entry(windowFrame)
		self.brNorthingEntry.grid(row = r, column = 3, sticky=W)
		
		r = r + 1
		
		self.file_window = IntVar()
		fileCheckbutton = Checkbutton(windowFrame, variable = self.file_window, command = self.file_window_selected)
		fileCheckbutton.grid(row = r, column = 0, sticky = W);

		refFileButton = Button(windowFrame, text = "File", command = self.get_ref_file)
		refFileButton.grid(row = r, column = 1, sticky = W)

		self.refFileEntry = Entry(windowFrame, width = 42)
		self.refFileEntry.grid(row = r, column = 2, columnspan = 2, sticky = W)
		

		###################################################################
		###################### Metric Frame ###############################
		###################################################################
		
		metricContain = Frame(master)
		metricContain.grid(row = 2, column = 0, sticky = NW, padx = 10, pady = 10)
		
		calcMetricLabel = Label(metricContain, text = "Terrain Metrics")
		calcMetricLabel.grid(row = 0, column = 0, sticky = NW, pady = 10)
		
		metricFrame = Frame(metricContain)
		metricFrame.grid(row = 1, column = 0, sticky = NW, padx = 10)
		
		r = 0
		c = 0
		
		
		#
		# relief
		#
		self.relief_value = IntVar()
		relief_checkbutton = Checkbutton(
			metricFrame, text="Relief", variable = self.relief_value, command = self.relief_checked);
		relief_checkbutton.grid(row = r, column = c, sticky = NW)
		
		r = r + 1
		
		#glcm (input)
		relief_glcm_lev_label = Label(metricFrame, text="GLCM Levels : ")
		relief_glcm_lev_label.grid(row = r, column = c, sticky = E)

		self.relief_levels = StringVar()
		self.relief_levels.set('64');
		relief_glcm_lev_entry = Entry(metricFrame, width = 3, textvariable = self.relief_levels)
		relief_glcm_lev_entry.grid(row = r, column = c+1, sticky = W)
		
		relief_glcm_wnd_label = Label(metricFrame, text="GLCM Window Size (pixels): ")
		relief_glcm_wnd_label.grid(row = r+1, column = c, sticky = E)
		
		self.relief_wnd = StringVar()
		self.relief_wnd.set('10');
		relief_glcm_wnd_entry = Entry(metricFrame, width = 3, textvariable = self.relief_wnd)
		relief_glcm_wnd_entry.grid(row = r+1, column = c+1, sticky = W)
                
		#standard deviation (output)
		relief_range_label = Label(metricFrame, text="Range : ");
		relief_range_label.grid(row = r, column = c+2, sticky  = W)

		self.relief_range_txt = StringVar()
		relief_range_value = Label(metricFrame, textvariable = self.relief_range_txt)
		relief_range_value.grid(row = r, column = c+3, sticky = W)

		r = r + 1
		
		#range
		relief_std_dev_label  = Label(metricFrame, text="Standard Deviation : ")
		relief_std_dev_label.grid(row = r, column = c+2, sticky = W)

		self.relief_stddev_txt = StringVar()
		relief_std_dev_value = Label(metricFrame, textvariable = self.relief_stddev_txt)
		relief_std_dev_value.grid(row = r, column = c+3, sticky = W)

		r = r + 1

		#
		#flow D8 checkbox
		#
		self.flow_value = IntVar();
		d8_checkbutton = Checkbutton(
			metricFrame, text="D-8 Flow", variable = self.flow_value, command = self.flow_checked);
		d8_checkbutton.grid(row = r, column = c, sticky=W);
		
		r = r + 1
		
		#glcm
		d8_glcm_lev_label = Label(metricFrame, text="GLCM Levels : ")
		d8_glcm_lev_label.grid(row = r, column = c, sticky = E)
		
		self.d8_levels = StringVar()
		self.d8_levels.set('64')
		d8_glcm_lev_entry = Entry(metricFrame, width = 3, textvariable = self.d8_levels)
		d8_glcm_lev_entry.grid(row = r, column = c+1, sticky = W)
		
		r = r + 1
		
		d8_glcm_wnd_label = Label(metricFrame, text="GLCM Window Size (pixels): ")
		d8_glcm_wnd_label.grid(row = r, column = c, sticky = E)
		
		self.d8_wnd = StringVar()
		self.d8_wnd.set('10')
		d8_glcm_wnd_entry = Entry(metricFrame, width = 3, textvariable = self.d8_wnd)
		d8_glcm_wnd_entry.grid(row = r, column = c+1, sticky = W)
		
		r = r + 1

		#
		#flow DInfinity checkbox
		#
		self.flow_inf_value = IntVar();
		dinf_checkbutton = Checkbutton(
                        metricFrame, text = "D-Infinity Flow", variable = self.flow_inf_value, command = self.flow_inf_checked);
		dinf_checkbutton.grid(row = r, column = c, sticky=W);

		r = r + 1
		
		#glcm
		dinf_glcm_lev_label = Label(metricFrame, text="GLCM Levels : ")
		dinf_glcm_lev_label.grid(row = r, column = c, sticky = E)
		
		self.dinf_levels = StringVar()
		self.dinf_levels.set('64')
		dinf_glcm_lev_entry = Entry(metricFrame, width = 3, textvariable = self.dinf_levels)
		dinf_glcm_lev_entry.grid(row = r, column = c+1, sticky = W)
		
		r = r + 1
		
		dinf_glcm_wnd_label = Label(metricFrame, text="GLCM Window Size (pixels): ")
		dinf_glcm_wnd_label.grid(row = r, column = c, sticky = E)
		
		self.dinf_wnd = StringVar()
		self.dinf_wnd.set('10')
		dinf_glcm_wnd_entry = Entry(metricFrame, width = 3, textvariable = self.dinf_wnd)
		dinf_glcm_wnd_entry.grid(row = r, column = c+1, sticky = W)
		
		r = r + 1

		#
		#slope gradient checkbutton
		#
		self.slope_grad_value = IntVar();
		slope_grad_checkbutton = Checkbutton(
			metricFrame, text="Slope Gradient", variable = self.slope_grad_value, command = self.slope_grad_checked);
		slope_grad_checkbutton.grid(row=r, column=c, sticky=W);

		r=r+1

		#glcm (input)
		slope_glcm_lev_label = Label(metricFrame, text="GLCM Levels : ")
		slope_glcm_lev_label.grid(row = r, column = c, sticky  = E)

		self.slope_levels = StringVar();
		self.slope_levels.set('64')
		slope_glcm_lev_entry = Entry(metricFrame, width = 3, textvariable = self.slope_levels)
		slope_glcm_lev_entry.grid(row = r, column = c+1, sticky = W)
		
		slope_glcm_wnd_label = Label(metricFrame, text="GLCM Window Size (pixels): ")
		slope_glcm_wnd_label.grid(row = r+1, column = c, sticky = E)

		self.slope_wnd = StringVar();
		self.slope_wnd.set('10')
		slope_glcm_wnd_entry = Entry(metricFrame, width = 3, textvariable = self.slope_wnd)
		slope_glcm_wnd_entry.grid(row = r+1, column = c+1, sticky = W)
		

		#mean gradient (output)
		slope_mean_label = Label(metricFrame, text="Mean : ")
		slope_mean_label.grid(row = r, column = c+2, sticky  = W)

		self.slope_mean_txt = StringVar()
		slope_mean_value = Label(metricFrame, textvariable = self.slope_mean_txt)
		slope_mean_value.grid(row = r, column = c+3, sticky = W)

		r=r+1

		slope_range_label = Label(metricFrame, text="Range : ")
		slope_range_label.grid(row = r, column = c+2, sticky  = W)

		self.slope_range_txt = StringVar()
		slope_range_value = Label(metricFrame, textvariable = self.slope_range_txt)
		slope_range_value.grid(row = r, column = c+3, sticky = W)

		r=r+1

		slope_std_dev_label = Label(metricFrame, text="Standard deviation : ")
		slope_std_dev_label.grid(row = r, column = c+2, sticky  = W)

		self.slope_std_txt = StringVar()
		slope_std_dev_value = Label(metricFrame, textvariable = self.slope_std_txt)
		slope_std_dev_value.grid(row = r, column = c+3, sticky = W)		

		r=r+1

		#
		#wetness index
		#
		self.wetness_index = IntVar();
		wetness_checkbutton = Checkbutton(metricFrame, text = "Wetness Index", variable = self.wetness_index, command = self.wetness_index_checked);
		wetness_checkbutton.grid(row = r, column = c, sticky = W)
		
		r=r+1
		
		#glcm
		wetness_glcm_lev_label = Label(metricFrame, text="GLCM Levels : ")
		wetness_glcm_lev_label.grid(row = r, column = c, sticky = E)
		
		self.wetness_levels = StringVar()
		self.wetness_levels.set('64')
		wetness_glcm_lev_entry = Entry(metricFrame, width = 3, textvariable = self.wetness_levels)
		wetness_glcm_lev_entry.grid(row = r, column = c+1, sticky = W)
		
		r = r + 1
		
		wetness_glcm_wnd_label = Label(metricFrame, text="GLCM Window Size (pixels): ")
		wetness_glcm_wnd_label.grid(row = r, column = c, sticky = E)
		
		self.wetness_wnd = StringVar()
		self.wetness_wnd.set('10')
		wetness_glcm_wnd_entry = Entry(metricFrame, width = 3, textvariable = self.wetness_wnd)
		wetness_glcm_wnd_entry.grid(row = r, column = c+1, sticky = W)
		
		r = r + 1

		#
		#slope aspect checkbox
		#
		self.slope_aspect_value = IntVar();
		aspect_checkbutton = Checkbutton(metricFrame, text="Slope Aspect", variable = self.slope_aspect_value, command = self.slope_aspect_checked);
		aspect_checkbutton.grid(row=r, column=c, sticky=W);

		r=r+1
		
		#glcm levels
		aspect_glcm_lev_label = Label(metricFrame, text="GLCM Levels : ")
		aspect_glcm_lev_label.grid(row = r, column = c, sticky = E)
		
		self.aspect_levels = StringVar()
		self.aspect_levels.set('64')
		aspect_glcm_lev_entry = Entry(metricFrame, width = 3, textvariable = self.aspect_levels)
		aspect_glcm_lev_entry.grid(row = r, column = c+1, sticky = W)

		#linearize
		self.aspect_linearize = IntVar();
		linearize_aspect_checkbutton = Checkbutton(
			metricFrame, text="Linearize", variable = self.aspect_linearize);
		linearize_aspect_checkbutton.grid(row = r, column = c+2, sticky  = E)

		r=r+1
		
		#glcm window size
		aspect_glcm_wnd_label = Label(metricFrame, text="GLCM Window Size (pixels): ")
		aspect_glcm_wnd_label.grid(row = r, column = c, sticky = E)
		
		self.aspect_wnd = StringVar()
		self.aspect_wnd.set('10')
		aspect_glcm_wnd_entry = Entry(metricFrame, width = 3, textvariable = self.aspect_wnd)
		aspect_glcm_wnd_entry.grid(row = r, column = c+1, sticky = W)

		#linearize
		aspect_offset_label = Label(metricFrame, text = "Offset angle : ")
		aspect_offset_label.grid(row = r, column = c+2, sticky= E)

		self.aspect_offset_angle = IntVar();
		aspect_offset_entry = Entry(metricFrame, width = 3, textvariable = self.aspect_offset_angle);
		aspect_offset_entry.grid(row = r, column = c+3, sticky = W)
		
		r=r+1
                
		#
		#downslope curvature checkbox
		#
		self.downslope_curv_value = IntVar();
		dcurv_checkbutton = Checkbutton(metricFrame, text="Down Slope Curvature", variable = self.downslope_curv_value, command = self.downslope_curv_checked);
		dcurv_checkbutton.grid(row=r, column=c, sticky=W);

		r=r+1

		#glcm (input)
		dcurv_glcm_lev_label = Label(metricFrame, text="GLCM Levels : ")
		dcurv_glcm_lev_label.grid(row = r, column = c, sticky  = E)
		
		self.dcurv_levels = StringVar()
		self.dcurv_levels.set('64')
		dcurv_glcm_lev_entry = Entry(metricFrame, width = 3, textvariable = self.dcurv_levels)
		dcurv_glcm_lev_entry.grid(row = r, column = c+1, sticky = W)

		dcurv_glcm_wnd_label = Label(metricFrame, text="GLCM Window Size (pixels): ")
		dcurv_glcm_wnd_label.grid(row = r+1, column = c, sticky  = E)

		self.dcurv_wnd = StringVar()
		self.dcurv_wnd.set('10')
		dcurv_glcm_wnd_entry = Entry(metricFrame, width = 3, textvariable = self.dcurv_wnd)
		dcurv_glcm_wnd_entry.grid(row = r+1, column = c+1, sticky = W)
                
		#mean (ouput)
		dcurv_mean_label = Label(metricFrame, text="Mean : ")
		dcurv_mean_label.grid(row = r, column = c+2, sticky  = W)

		self.dcurv_txt = StringVar()
		self.dcurv_val = Label(metricFrame, textvariable = self.dcurv_txt)
		self.dcurv_val.grid(row = r, column = c+3, sticky = W)
		
		r=r+2
		
		#
		#acrossslope checkbox
		#
		self.acrossslope_curv_value = IntVar();
		self.acrossslope_curv_box = Checkbutton(metricFrame, text="Across Slope Curvature", variable = self.acrossslope_curv_value, command = self.acrossslope_curv_checked);
		self.acrossslope_curv_box.grid(row=r, column=c, sticky=W);

		r = r + 1

		#glcm levels (input)
		acurv_glcm_lev_label = Label(metricFrame, text="GLCM Levels : ")
		acurv_glcm_lev_label.grid(row = r, column = c, sticky  = E)

		self.acurv_levels = StringVar()
		self.acurv_glcm_val = Entry(metricFrame, width = 3, textvariable = self.acurv_levels)
		self.acurv_glcm_val.grid(row = r, column = c+1, sticky = W)
		self.acurv_levels.set('64')
		
		acurv_glcm_wnd_label = Label(metricFrame, text="GLCM Window Size (pixels): ")
		acurv_glcm_wnd_label.grid(row = r+1, column = c, sticky  = E)
		
		self.acurv_wnd = StringVar()
		self.acurv_glcm_wnd_val = Entry(metricFrame, width = 3, textvariable = self.acurv_wnd)
		self.acurv_glcm_wnd_val.grid(row = r+1, column = c+1, sticky = W)
		self.acurv_wnd.set('10')
        
		#mean (output)
		acurv_mean_label = Label(metricFrame, text="Mean : ")
		acurv_mean_label.grid(row = r, column = c+2, sticky  = W)

		self.acurv_txt = StringVar()
		acurv_mean_value = Label(metricFrame, textvariable = self.acurv_txt)
		acurv_mean_value.grid(row = r, column = c+3, sticky = W)

		r = r + 2
		
		#quit button
		self.quit_button = Button(metricFrame, text="Quit", command=master.quit)
		self.quit_button.grid(row=r, column=0, sticky=W)
		#start button
		self.start_button = Button(metricFrame, text='Start', command=self.execute)
		self.start_button.grid(row=r, column=1, stick=W)
		
		
		###################################################################
		###################### Message Frame ##############################
		###################################################################
		
		status_frame = Frame(master);
		status_frame.grid(row = 0, column = 1, sticky = N, rowspan=3, padx = 10, pady = 10)

		#status message label
		self.result_label = Label(status_frame, text="Status Message...")
		self.result_label.grid(row = 0, column = 0, sticky = NW)

		self.msg_frame = Frame(status_frame)
		#scrollbar
		self.scroll = Scrollbar(self.msg_frame)
		#status message box
		self.result_message = Text(self.msg_frame, height = 58, width = 80, background='white', borderwidth = 1, relief = RIDGE, yscrollcommand=self.scroll.set)
		self.result_message.pack(side=LEFT)

		self.scroll.pack(side=RIGHT, fill=Y)
		self.scroll.config(command=self.result_message.yview)
		self.msg_frame.grid(row = 1, column = 0)

		
		#
		#functions
		#
		self.functions = [0,0,0,0,0,0,0,0]
		
		self.metric_array = [self.relief_value,self.flow_value,self.flow_inf_value,self.slope_grad_value,self.wetness_index,self.slope_aspect_value,self.downslope_curv_value,self.acrossslope_curv_value]
		
	def update(self):
		self.ulEastingEntry.delete(0, END)
		self.ulEastingEntry.insert(0, self.clipBoard["ul_east"])
		self.ulNorthingEntry.delete(0, END)
		self.ulNorthingEntry.insert(0, self.clipBoard["ul_north"])
		self.brEastingEntry.delete(0, END)
		self.brEastingEntry.insert(0, self.clipBoard["br_east"])
		self.brNorthingEntry.delete(0, END)
		self.brNorthingEntry.insert(0, self.clipBoard["br_north"])
		
	def coordinate_window_selected(self) :
		if self.file_window.get() == 1 :
			self.file_window.set(0)
			
	def file_window_selected(self) :
		if self.coordinate_window.get() ==1 :
			self.coordinate_window.set(0)
			
	def get_ref_file(self):
		ref_file = askopenfilename()
		
		if ref_file == "":
			return
			
		self.refFileEntry.delete(0, END)
		self.refFileEntry.insert(0, ref_file)
		
	def relief_checked(self):
		self.functions[0] = self.metric_array[0].get()
		print self.functions

	def flow_checked(self):
		self.functions[1] = self.metric_array[1].get()
		print self.functions

	def flow_inf_checked(self):		
		self.functions[2] = self.metric_array[2].get()
		print self.functions
		
		if self.flow_inf_value.get() == 0:
			self.wetness_index.set(0)
	
	def slope_grad_checked(self):
		self.functions[3] = self.slope_grad_value.get()
		print self.functions
		
		if self.slope_grad_value.get() == 0:
			self.wetness_index.set(0)
			self.wetness_index_checked()
                
		

	def wetness_index_checked(self):
		self.functions[4] = self.wetness_index.get()
		print self.functions
		#all of these metrics are required to calculate wetness
		if self.wetness_index.get() == 1 :
				self.flow_inf_value.set(1)
				self.flow_inf_checked()
				self.slope_grad_value.set(1)
				self.slope_grad_checked()
		
	def slope_aspect_checked(self):
		self.functions[5] = self.slope_aspect_value.get()
		print self.functions
		
	def downslope_curv_checked(self):
		self.functions[6] = self.downslope_curv_value.get()
		print self.functions
		
	def acrossslope_curv_checked(self):
		self.functions[7] = self.acrossslope_curv_value.get()
		print self.functions
				
	def get_in_file(self):
		in_file = askopenfilename()
		
		if in_file == "":
			return
		
		self.in_text.delete(0, END)
		self.in_text.insert(0, in_file)
		
	def get_out_directory(self):
		out_directory = askdirectory(title="Select output directory.")
		
		if out_directory == "":
			return
		
		self.out_text.delete(0, END)
		self.out_text.insert(0, out_directory)
		
	def update_result_message(self, message):
		self.result_message.insert(END, message)
		self.result_message.update()
		self.result_message.yview(END)
		
	def handle_exception(self, exception):
		self.update_result_message("\n" + str(exception[0]) + " : "  + str(exception[1]) + "\n\n")
		self.start_button.configure(state=NORMAL)
		
	def handle_error(self, message):
		self.update_result_message(message)
		self.start_button.configure(state=NORMAL)
		
	def execute(self):

		self.start_button.configure(state=DISABLED)

		if self.in_text.get() == "" :
			self.handle_error("Input Error : Must select an input file.\n\n")
			return

		if self.out_text.get() == "" :
			self.handle_error("Input Error : Must select an output directory.\n\n")
			return
				

		if (1000 % int(self.relief_wnd.get()) != 0)  or \
		   (self.flow_value.get() == 1)             and (1000 % int(self.d8_wnd.get()) != 0) or \
		   (self.flow_inf_value.get() == 1)         and (1000 % int(self.dinf_wnd.get()) != 0) or \
		   (self.slope_grad_value.get() == 1)       and (1000 % int( self.slope_wnd.get()) != 0)  or \
		   (self.wetness_index.get() == 1)    	    and (1000 % int( self.wetness_wnd.get()) != 0)  or \
		   (self.slope_aspect_value.get() == 1)     and (1000 % int( self.aspect_wnd.get()) != 0)  or \
		   (self.downslope_curv_value.get() == 1)   and (1000 % int( self.dcurv_wnd.get()) != 0)  or \
		   (self.acrossslope_curv_value.get() == 1) and (1000 % int( self.acurv_wnd.get()) != 0):

			self.handle_error("Input Error : GLCM window size must divide 1000.\n\n")
			return
        
		bands = 0
		
		for x in self.metric_array:
			if x.get() == 1:
				bands = bands + 1
				
		if bands == 0 :
			self.handle_error("Must select a metric.\n\n")
			return
		
		#
		#empty fields from previous run
		#
		self.relief_stddev_txt.set("")
		self.relief_range_txt.set("")
		self.slope_range_txt.set("")
		self.slope_std_txt.set("")
		self.slope_mean_txt.set("");
		self.dcurv_txt.set("")
		self.acurv_txt.set("")
				
		self.update_result_message("Input file : " + self.in_text.get() + "\n")
		self.update_result_message("Running...\n")
                
                #
		#Setup ...
		#
		try:
			self.update_result_message("   - Setup\n")
			
			setup(self.in_text.get(), self.out_text.get(), self.flow_value.get(), self.flow_inf_value.get(), self.slope_grad_value.get(), self.wetness_index.get())
			
			if self.coordinate_window.get() == 1 :
				
				ule = float(self.ulEastingEntry.get())
				uln = float(self.ulNorthingEntry.get())
				bre = float(self.brEastingEntry.get())
				brn = float(self.brNorthingEntry.get())
				
				self.update_result_message("   - Window filter\n")
				
				windowFilter(ule, uln, bre, brn)
			
			elif self.file_window.get() == 1 :
				
				self.update_result_message("   - Window filter\n")
				fileFilter(self.refFileEntry.get())
				
		except:
			self.handle_exception(exc_info())
			return

		#
		#Relief ... 
		#
		if self.relief_value.get() == 1 :
			self.update_result_message("   - Relief\n")
		
			try:
				levels = int(self.relief_levels.get())
				window = int(self.relief_wnd.get())
			except:
				self.handle_exception(exc_info())
				return
			
			result = relief(levels, window)
			
			min = result[0]
			max = result[1]
			std = result[2]
	
			self.relief_stddev_txt.set('%.2f'%std)
			self.relief_range_txt.set("[" + '%.2f'%min + ", " + '%.2f'%max + "]")

		#
		#Median filter ... 
		#
		if self.flow_value.get() == 1 or self.flow_inf_value.get() == 1:
			
			self.update_result_message("   - Median filter\n")
			try:
				calculateMedian()
			except:
				self.handle_exception(exc_info())
				return   

		band = 0

		#
		#D8 upslope area / flow ... 
		#
		if self.flow_value.get() == 1:
			
			try:
				levels = int(self.d8_levels.get())
				window = int(self.d8_wnd.get())
			except:
				self.handle_exception(exc_info())
				return
			
			self.update_result_message("   - D-8 flow\n")				
			calculateFlow(band, levels, window)
			band = band + 1

		#
		#DInf upslope area / flow ... 
		#
		if self.flow_inf_value.get() == 1:
			try:
				levels = int(self.dinf_levels.get())
				window = int(self.dinf_wnd.get())
			except:
				self.handle_exception(exc_info())
				return
			
			self.update_result_message("   - D-Infinity flow\n")
			calculateFlowDInfinity(band, levels, window)
			band = band + 1

		#
		#Slope gradient ... 
		#
		if self.slope_grad_value.get() == 1:
			
			self.update_result_message("   - Slope gradient\n")

			try:
				levels = int(self.slope_levels.get())
				window = int(self.slope_wnd.get())
			except:
				self.handle_exception(exc_info())
				return
			
			result = slopeGradient(band, levels, window)

			min = result[0]
			max = result[1]
			std = result[2]
			mean  = result[3]
			
			self.slope_range_txt.set("[" + '%.2f'%min + ", " + '%.2f'%max + "]")
			self.slope_std_txt.set('%.2f'%std)
			self.slope_mean_txt.set('%.2f'%mean);
							
			band = band + 1

		#
		#Wetness Index ...
		#
		if self.wetness_index.get() == 1:
			
			self.update_result_message("   - Wetness index\n")
			try:
				levels = int(self.wetness_levels.get())
				window = int(self.wetness_wnd.get())
			except:
				self.handle_exception(exc_info())
				return
			
			calculateWetnessIndex(levels, window)

		#
		#Slope aspect ... 
		#
		if self.slope_aspect_value.get() == 1:
			
			self.update_result_message("   - Slope aspect\n")
			
			try:
				levels = int(self.aspect_levels.get())
				window = int(self.aspect_wnd.get()) 
				offset = self.aspect_offset_angle.get()
			except:
				self.handle_exception(exc_info())
				return 

			slopeAspect(band, levels, window, self.aspect_linearize.get(), offset)
                                
			band = band + 1

		#
		#Donwslope curvature ... 
		#
		if self.downslope_curv_value.get() == 1:
			
			self.update_result_message("   - Downslope curvature\n")

			try:
				levels = int(self.dcurv_levels.get())
				window = int(self.dcurv_wnd.get())
			except:
				self.handle_exception(exc_info())
				return

			mean = downSlopeCurvature(band, levels, window)

			self.dcurv_txt.set('%.2f'%mean)
                                
		band = band + 1

		#
		#Acrossslope curvature ... 
		#
		if self.acrossslope_curv_value.get() == 1:
			
			self.update_result_message("   - Acrossslope curvature\n")

			try:
				levels = int(self.acurv_levels.get())
				window = int(self.acurv_wnd.get())
			except:
				self.handle_exception(exc_info())
				return

			mean = acrossSlopeCurvature(band, levels, window)

			self.acurv_txt.set('%.2f'%mean)
                        
			band = band + 1

		self.update_result_message("   - Teardown\n")

		teardown()
		
		self.start_button.configure(state=NORMAL)
		self.update_result_message("Complete.\n\n")
		
"""
root = Tk()
app = App(root)
root.mainloop()
"""
