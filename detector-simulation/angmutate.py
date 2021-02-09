#!python3

# a helper code that takes in a given file and then creates 36 new files by altering the angle by 10 degrees

import pandas as pd
import numpy as np
import re

def changeAngle(fileDf, dAngle):
	# fileDf is a DataFrame that contains the x, y, z data
	newDf = pd.DataFrame(columns=["x","y","z"])
	newDf["x"] = fileDf["x"]*np.cos(dAngle) - fileDf["y"]*np.sin(dAngle)
	newDf["y"] = fileDf["x"]*np.sin(dAngle) + fileDf["y"]*np.cos(dAngle)
	newDf["z"] = fileDf["z"]

	return newDf

def mutateFiles(file):
	data = pd.read_csv(file)
	# extract the number from the filename using regex or something
	num = re.findall(r"\d+",file)[0]
	name = re.findall(r".*-",file)[0]
	angles = [10 + 10*i for i in range(35)]
	for theta in angles:
		print(f"Mutating by angle {theta}")
		translated = changeAngle(data, theta*(np.pi/180))
		
		translated.to_csv(f"{name}{num}-{theta}.csv", index=False) # with the modified filename $DIR-trackNum-angleDev.csv

