
from Tkinter import *

import TM_Interface
import Bio_Interface
import SB_Interface
import TCR_Interface
import TCD_Interface
import FLD_Interface
import TCM_Interface
import CR_Interface
import CSP_Interface
#import LvL_Interface
#import SM_Interface
import ttk
import tkFont

class Tab(Frame):
	
	def __init__(self, master, name):
		
		Frame.__init__(self, master)
		self.tab_name = name

class TabBar(Frame):
	
	def __init__(self, master = None):
		
		Frame.__init__(self, master)
		
		self.tabs = {}
		self.buttons = {}
		self.current_tab = None
		self.update_functions = {}
		
	def show(self, name):
		
		self.pack(pady = 10)
		if name in self.tabs.keys() :
			self.switch_tab(name)
		
	def add(self, tab, update = None):
		
		self.tabs[tab.tab_name] = tab
		
		self.update_functions[tab.tab_name] = update
		
		b = Button(self, text=tab.tab_name, command = (lambda: self.switch_tab(tab.tab_name)))
		b.pack(side=LEFT)
		self.buttons[tab.tab_name] = b
		
	def switch_tab(self, name):
		#reset window resizing behaviour
		self.winfo_toplevel().wm_geometry("")
		
		if self.current_tab:
			
			if self.current_tab == name:
				return
			
			self.tabs[self.current_tab].pack_forget()
			
			self.buttons[self.current_tab].config(font=("TkTextFont"))
		#self.buttons[self.current_tab].config(font=("Courier",12,"bold"))
			
		self.tabs[name].pack(side=BOTTOM, fill = BOTH, expand = 1)
		self.current_tab = name
		
		#execute update function
		update = self.update_functions[name]
		if not(update == None) :
			update()
			
		#button color
		
		self.buttons[name].config(font=("Courier",14,"bold"))
		
#
# Create root
#
root = Tk()
root.title("LMTools")

#
# Dictionary that stores global data shared between modules.
#
clipBoard = {"ul_east": 0.0, "ul_north": 0.0, "br_east": 0.0, "br_north" : 0.0, "sam_class_names" : [], "sam_classifier_thresholds" : [],\
	"spectral_library" : ""}

#
# Create and initialize terrain modelling tab
#
tm_tab = Tab(root, "Terrain Modelling")
tmi = TM_Interface.Init(tm_tab, clipBoard)

#
# Create and initialize biometrics tab
#
bio_tab = Tab(root, "Biometrics")
Bio_Interface.Init(bio_tab, clipBoard)

#
# Create and initialize bounded biometrics tab
#
sb_tab = Tab(root, "Bounded Biometrics")
SB_Interface.Init(sb_tab, clipBoard)

#
# Create and initialize tree top tab
tcr_tab = Tab(root, "TC Removal")
TCR_Interface.Init(tcr_tab, clipBoard)

#
#Create and initialize tree crown delineation tab
tcd_tab = Tab(root, "TT/TC Delineation")
TCD_Interface.Init(tcd_tab, clipBoard)

#
#Create and initialize flight line delineation tab
fld_tab = Tab(root, "Flightline Delineation")
FLD_Interface.Init(fld_tab, clipBoard)

#
#Create and initialize tree crown metrics tab
tcm_tab= Tab(root, "TC Metrics")
TCM_Interface.Init(tcm_tab, clipBoard)

#
#Create and initialize continuum removal tab
cr_tab = Tab(root, "Cont. Removal")
CR_Interface.Init(cr_tab, clipBoard)

#
#Create and initialize convolution tab
csp_tab = Tab(root, "Convolution")
CSP_Interface.Init(csp_tab, clipBoard)

#
#Create and initialize lib vs lib
#lvl_tab = Tab(root, "Lib vs Lib")
#LvL_Interface.Init(lvl_tab, clipBoard)


#
#Create and initialize spectral mapping/matching
#sm_tab = Tab(root, "Spectral Metrics")
#smi = SM_Interface.Init(sm_tab, clipBoard)
	
#
# Create tab bar
#
tabbar = TabBar(root)

#
# Add tabs to the tab bar
#
tabbar.add(tm_tab, tmi.update)
tabbar.add(bio_tab)
tabbar.add(sb_tab)
tabbar.add(tcr_tab)
tabbar.add(tcd_tab)
tabbar.add(fld_tab)
tabbar.add(tcm_tab)
tabbar.add(cr_tab)
tabbar.add(csp_tab)
#tabbar.add(lvl_tab)
#tabbar.add(sm_tab, smi.update)


#
# Show the tab bar with the terrain modelling tab selected
#
tabbar.show("TC Metrics")

root.mainloop()



		

