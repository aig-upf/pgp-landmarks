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
	str_domain = "GRIPPER\n\n"
	str_domain += "STATE DESCRIPTOR:\n"
	str_domain += "room:var_type\n"
	str_domain += "ball:var_type\n"
	str_domain += "gripper:var_type\n"
	str_domain += "at-robby(room):pred_type\n"
	str_domain += "at(ball,room):pred_type\n"
	str_domain += "free(gripper):pred_type\n"
	str_domain += "carry(ball,gripper):pred_type\n"
	str_domain += "r1:room\n"
	str_domain += "r2:room\n"
	str_domain += "b1:ball\n"
	str_domain += "g1:gripper\n"
	
	str_domain += "\nACTION: move(r1:room,r2:room)\n"
	str_domain += "TYPE: memory\n"
	str_domain += "PRECONDITIONS:\n"
	str_domain += "( at-robby(r1) = 1 )\n"
	str_domain += "EFFECTS:\n"
	str_domain += "( at-robby(r1) = 0 )\n"
	str_domain += "( at-robby(r2) = 1 )\n"

	str_domain += "\nACTION: pick(b:ball,r:room,g:gripper)\n"
	str_domain += "TYPE: memory\n"
	str_domain += "PRECONDITIONS:\n"
	str_domain += "( at(b,r) = 1 )\n"
	str_domain += "( at-robby(r) = 1 )\n"
	str_domain += "( free(g) = 1 )\n"
	str_domain += "EFFECTS:\n"
	str_domain += "( carry(b,g) = 1 )\n"
	str_domain += "( at(b,r) = 0 )\n"
	str_domain += "( free(g) = 0 )\n"
	
	str_domain += "ACTION: drop(b:ball,r:room,g:gripper)\n"
	str_domain += "TYPE: memory\n"
	str_domain += "PRECONDITIONS:\n"
	str_domain += "( carry(b,g) = 1 )\n"
	str_domain += "( at-robby(r) = 1 )\n"
	str_domain += "EFFECTS:\n"
	str_domain += "( carry(b,g) = 0 )\n"
	str_domain += "( at(b,r) = 1 )\n"
	str_domain += "( free(g) = 1 )\n"
		
	f_domain=open( out_folder + "domain.txt", "w" )
	f_domain.write( str_domain )
	f_domain.close
			
	# INSTANCES
	for i in range(from_nth,to_nth+1):
		# Problem name
		str_problem = "GRIPPER-" + str(i) + "\n"
		
		# Objects
		str_problem += "\nOBJECTS:\n"
		for j in range(i):
			str_problem += "ball"+str(j+1)+":ball\n"
		str_problem += "left:gripper\n"
		str_problem += "right:gripper\n"
		str_problem += "rooma:room\n"
		str_problem += "roomb:room\n"
				
		# Initial state
		str_problem += "\nINIT:\n"
		str_problem += "( at-robby(rooma) = 1 )\n"
		for j in range(i):
			str_problem += "( at(ball"+str(j+1)+",rooma) = 1 )\n"
		str_problem += "( free(left) = 1 )\n"
		str_problem += "( free(right) = 1 )\n"
		
		# Compute		
		
		# Goal condition
		str_problem += "\nGOAL:\n"
		for j in range(i):
			str_problem += "( at(ball"+str(j+1)+",roomb) = 1 )\n"
		
		#print( str_problem )
		f_problem=open( out_folder + str( i+1-from_nth ) + ".txt","w")
		f_problem.write( str_problem )
		f_problem.close()
	
	sys.exit( 0 )
	
if __name__ == "__main__":
	main()
