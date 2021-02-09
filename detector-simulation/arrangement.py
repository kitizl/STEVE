#!python3

# This is better data arrangement code

from conversions import *
from detector import Detector
import pandas as pd
import time

from itertools import combinations

# firstly, we ar egoing to do some data arrangement

numOfFiles = 20

# creating an array of file names
filenames = [f"trackGroups/singlets/rzphi{i}.csv" for i in range(numOfFiles)]


for numOfTracks in range(1,numOfFiles+1):
	# here we are going to create numOfTracks number of tracks in each setup
	tcomb0 = time.time()
	fileGroups = list(combinations(filenames,numOfTracks))
	tcomb1 = time.time()
	print(f"Time taken for combinations : {tcomb1-tcomb0}")
	print(f"Now dealing with {numOfTracks} tracks...")
	t0 = time.time()
	for (i,group) in enumerate(fileGroups):
		
		if i ==1 :
			continue
		data = pd.concat([pd.read_csv(f) for f in group],ignore_index=True).drop_duplicates().reset_index(drop=True)
		data.to_csv(f"./trackGroups/tracks{len(group)}/rzphi{i}.csv")
	
	t1 = time.time()       
	print(f"\t{numOfTracks} track(s) completed. (Time taken : {t1-t0}).")
