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
	str_domain = "VISITALL\n\n"
	str_domain += "STATE DESCRIPTOR:\n"
	
	str_domain += "row:var_type\n"
	str_domain += "column:var_type\n"
	str_domain += "cell(row,column):pred_type\n"
	str_domain += "i:row\n"
	str_domain += "j:column\n"
		
	str_domain += "\nACTION: visit(x:row,y:column)\n"
	str_domain += "TYPE: memory\n"
	str_domain += "PRECONDITIONS:\n"
	str_domain += "( cell(x,y) = 0 )\n"
	str_domain += "EFFECTS:\n"
	str_domain += "( cell(x,y) = 1 )\n"

	f_domain=open( out_folder + "domain.txt", "w" )
	f_domain.write( str_domain )
	f_domain.close
		

	random.seed(1007)
	
	# INSTANCES
	for i in range(from_nth,to_nth+1):
		# Problem name
		str_problem = "VISITALL-" + str(i) + "\n"
			
		# Objects
		str_problem += "\nOBJECTS:\n"
		for p in range(i):
			str_problem += "r"+str(p)+":row\n"
			str_problem += "c"+str(p)+":column\n"		
		
		# Initial state
		str_problem += "\nINIT:\n"
		for r in range(i):
			for c in range(i):
				str_problem += "( cell(r"+str(r)+",c"+str(c)+") = 0 )\n"
		
		# Compute		
		
		# Goal condition
		str_problem += "\nGOAL:\n"
		for r in range(i):
			for c in range(i):
				str_problem += "( cell(r"+str(r)+",c"+str(c)+") = 1 )\n"
		
		#print( str_problem )
		f_problem=open( out_folder + str( i+1-from_nth ) + ".txt","w")
		f_problem.write( str_problem )
		f_problem.close()
	
	sys.exit( 0 )
	
if __name__ == "__main__":
	main()
