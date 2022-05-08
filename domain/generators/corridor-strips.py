#!/usr/bin/env python
# coding: utf-8

import sys
import numpy as np
import random

def main():
	try:
		from_nth = int( sys.argv[1] )
		to_nth = int( sys.argv[2] )
		out_folder = sys.argv[3]
	except:
		print("Usage: ")
		print(sys.argv[ 0 ] + " <from_n> <to_n> <outfolder>")
		print(sys.argv[ 0 ] + " 2 6 tmp/")
		sys.exit(-1)
		
	# DOMAIN
	str_domain = "CORRIDOR-STRIPS\n\n"
	str_domain += "STATE DESCRIPTOR:\n"
	
	str_domain += "location:var_type\n"
	str_domain += "at(location):pred_type\n"
	str_domain += "goal-at(location):pred_type\n"
	str_domain += "adjacent(location,location):pred_type\n"
	str_domain += "ptr-loc1:location\n"
	str_domain += "ptr-loc2:location\n"
		
	str_domain += "\nACTION: move(from:location,to:location)\n"
	str_domain += "TYPE: memory\n"
	str_domain += "PRECONDITIONS:\n"
	str_domain += "( at(from) = 1 )\n"
	str_domain += "( at(to) = 0 )\n"
	str_domain += "( adjacent(from,to) = 1 )\n"
	str_domain += "EFFECTS:\n"
	str_domain += "( at(from) = 0 )\n"
	str_domain += "( at(to) = 1 )\n"

	f_domain=open( out_folder + "domain.txt", "w" )
	f_domain.write( str_domain )
	f_domain.close
		

	random.seed(1007)

	# INSTANCES
	for i in range(from_nth,to_nth+1):
		# Problem name
		str_problem = "CORRIDOR-" + str(i) + "\n"
		
		# Compute (pre)
		vi = random.randint(1,i)
		vgi = random.randint(1,i)
		while vgi == vi:
			vgi = random.randint(1, i)
			
		# Objects
		str_problem += "\nOBJECTS:\n"
		for j in range(1,i+1):
			str_problem += "l"+str(j)+":location\n"
		
		# Initial state
		str_problem += "\nINIT:\n"
		str_problem += "( at(l"+str(vi)+") = 1 )\n"
		str_problem += "( goal-at(l"+str(vgi)+") = 1 )\n"
		for j in range(1,i):
			str_problem += "( adjacent(l"+str(j)+",l"+str(j+1)+") = 1 )\n"
			str_problem += "( adjacent(l"+str(j+1)+",l"+str(j)+") = 1 )\n"
		
		# Compute		
		
		# Goal condition
		str_problem += "\nGOAL:\n"
		str_problem += "( at(l"+str(vgi)+") = 1 )\n"
		
		#print( str_problem )
		f_problem=open( out_folder + str( i+1-from_nth ) + ".txt","w")
		f_problem.write( str_problem )
		f_problem.close()
	
	sys.exit( 0 )
	
if __name__ == "__main__":
	main()
