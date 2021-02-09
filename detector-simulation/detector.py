#!python3

import numpy as np
import pandas as pd
from conversions import cyl2cart
import matplotlib.pyplot as plt


"""This file contains the code that simulates the detector."""


class Detector:
	def __init__(self):
		# a detector class that just has a list of all the vanes
		
		self.vanes = [Vane(angle) for angle in np.linspace(0,360,49)] # list of Vane objects
		
		self.isPop = 0 # a flag that keeps track of whether or not the detector has been populated
		
		self.dataPlot = pd.DataFrame(columns = ["r","z","phi"])
	def setHits(self,cylDf):
		"""
		Converts trajector data (in cylDf) and sets the hits on each vane
		"""
		# first setting all the hits
		for v in self.vanes:
			v.setHits(cylDf)
		
		# setting isPop to 1 to say Detector is nonempty
		self.isPop = 1

	def dataSet(self):
		"""
		A function that returns a table r | z | phi for machine learning purposes
		"""
		return pd.concat([vane.coordDump() for vane in self.vanes],ignore_index=True)
		
	def dataDump(self):
		"""
		A function that returns all of the vane /data/ as numpy array of vane pixelArrays
		"""
		return np.array([v.pixelArray for v in self.vanes])
	
	def showDetector(self,proj):
		"""
		Makes 3D plot based on the data within the detector right now.
		INCOMPLETE.
		"""
		if self.isPop==0:
			# if data has not been processed
			print("There is nothing to show")
			return -1
		
		dataPlot = self.dataSet()

		cartPlot = cyl2cart(dataPlot)
		#print(cartPlot.head())
		proj[1].plot(cartPlot.x,cartPlot.y,cartPlot.z,"bs")
		
		#print("Detector Data plotting functionality is not complete.")
		return -1

class Vane:
	def __init__(self,phi):

		# each vane is defined solely by its phi angle
		self.phi = phi
		# creating the empty mesh 
		self.pixelArray = np.zeros((2048,1024))
	
	def coordDump(self):
		"""
		A function that returns a pandas DataFrame with r z phi
		"""
		data = pd.DataFrame(columns=["r","z","phi"])
		if self.isUnhit():
			return data
		# check if the current vane is Unhit maybe
		else:
			pixel = np.nonzero(self.pixelArray) # returns indices where pixelArray is hit
			data = pd.DataFrame(columns=["r","z","phi"])
			data.r = pixel[1]*(190e-6) + 75.5e-3 # v
			data.z = (1024 - pixel[0])*(190e-6) # translation to convert pixel coords to real coords
			data.phi = self.phi*np.ones(len(data))
			
			return data
	
	def isUnhit(self):
		"""
		A function that returns True if all elements in the pixelArray is 0
		"""
		return np.all(self.pixelArray == 0)
	
	def setHits(self,cylDf):
		"""
		Converts trajectory data (in cylDf) to hits (1) on the pixelArray
		"""
		
		# selecting the data that has the same phi
		dataInterest = cylDf[np.abs(cylDf["phi"] - self.phi) < 1e-3]
		
		# we technically have a tolerance of nearly 7 degrees, but keeping it small
		# just in case the other parts of the program are not working as expected
		
		# each pixel has a width of 190um and height of 190m
		# in cylindrical coordinates, it just works out perfectly
		
		indices = pd.DataFrame(columns=["hor","ver"])
		
		indices["hor"] = ((dataInterest["r"]-75.5e-3)/(190e-6)).astype(int) # horizontal index (r axis)
		indices["ver"] = 1024 - (dataInterest["z"]/(190e-6)).astype(int) # vertical index (z axis)

		# translation by 1024 because otherwise we get negative indices
		for index in np.array(indices):
			self.pixelArray[index[1],index[0]] = 1
		# setting the "hit" pixel to 1
