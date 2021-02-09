#!python3

# this file contains some of the conversion functions

# code to turn "just data" to detector data

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def cart2cyl(cartDf):
	"""
		Converts cartesian coordinates to cylindrical coordinates in a pandas DataFrame.

		Keyword Arguments:       
			cartDf - a pandas DataFrame with data in Cartesian coordinates
		
		Initial tests show the function is working as expected
	"""
	
	cylDf = pd.DataFrame(columns=['r','z','phi']) # creating a cylindrical df
	
	# performing coordinate transform
	cylDf.r = np.sqrt(cartDf.x**2 + cartDf.y**2)
	cylDf.z = cartDf.z
	cylDf.phi = np.arctan2(cartDf.y,cartDf.x)
	
	# fixing the angle phi such that it runs from 0 to 2pi
	cylDf.phi[cylDf.phi<0] += 2*np.pi
	
	# converting from radians to degrees
	cylDf.phi *= 180/np.pi
		
	return cylDf

def cyl2cart(cylDf):
	"""
	Converts cylindrical coordinates to cartesian coordinates in a pandas DataFrame (for plotting purposes only).
	
	Input:
		- cylDf : a pandas DataFrame with data in cylindrical coordinates
	Output:
		- a pandas DataFrame with data in cartesian coordinates
	"""
	
	newDf = pd.DataFrame(columns=['x','y','z'])
	newDf.x, newDf.y= cylDf.r*(np.cos(cylDf.phi*(np.pi/180))), cylDf.r*(np.sin(cylDf.phi*(np.pi/180))) # performing coord transform
	newDf.z = cylDf.z
	
	return newDf

def hitFilter(cylDf, tol=1e-6):
	"""
		A function that determines, given a cylindrical dataset, the points (r,z,p) of a 
		dataset of each vane hit.
		
		Input:
			- cylDf : a pandas DataFrame with data in cylindrical coordinates
		Output:
			- a pandas DataFrame including only rows that correspond to a hit
		
	"""
	numVanes = 48 # number of vanes
	angPos = 360/numVanes # the angular distance between two vanes - exactly 7.5 degrees

	# calculates the remainders of the angles when divided by where each detector plate is
	residueArray = np.array(cylDf.phi%angPos)
	
	# calculating a discrete "derivative" to detect discontinuities 
	diffArray = np.zeros(residueArray.shape)
	for i in range(len(residueArray)-1):
		diffArray[i] = residueArray[i+1]-residueArray[i]
	
	# returning the parts of the dataset that does get triggered
	return cylDf[np.abs(diffArray) >= 0.02]

	# WARNING : This _might_ return two points with very close values of phi.
	# But this is not that big a deal because when it comes to the hit detection
	# All of these "duplicate" values collapse to one.

def plotDetector(detectorBounds=True,vanes=True, numVanes=48):
	"""
		A helper subroutine that will only plot the detector rings and the vanes
	"""
	# plotting the two black rings
	t = np.linspace(0,2*np.pi,1000)
	plt.plot((293e-3)*np.cos(t),(293e-3)*np.sin(t),0,'k')
	plt.plot((75.5e-3)*np.cos(t),(75.5e-3)*np.sin(t),0,'k')
	
	# plotting the vanes
	if vanes:
		angles = np.arange(0,2*np.pi,7.5*np.pi/180)
		params = np.linspace(0,293e-3, 2)
		for angle in angles:
			plt.plot(params*np.cos(angle),params*np.sin(angle),'k', alpha=0.5)