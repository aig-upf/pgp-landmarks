#!/usr/bin/env python
# coding: utf-8

import sys

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
	str_domain = "TRIANGULAR-SUM\n\n"
	str_domain += "STATE DESCRIPTOR:\n"
	str_domain += "object:var_type\n"
	str_domain += "vector(object):pred_type\n"
	str_domain += "a:object\n"
	str_domain += "b:object\n"
		
	str_domain += "\nACTION: vector-add(x:object,y:object)\n"
	str_domain += "TYPE: memory\n"
	str_domain += "PRECONDITIONS:\n"
	str_domain += "( vector(x) + vector(y) )\n"
	str_domain += "EFFECTS:\n"
	str_domain += "( vector(x) += vector(y) )\n"

	str_domain += "\nACTION: vector-inc(x:object)\n"
	str_domain += "TYPE: memory\n"
	str_domain += "PRECONDITIONS:\n"
	str_domain += "( vector(x) + 1 )\n"
	str_domain += "EFFECTS:\n"
	str_domain += "( vector(x) += 1 )\n"
	
	str_domain += "\nACTION: vector-dec(x:object)\n"
	str_domain += "TYPE: memory\n"
	str_domain += "PRECONDITIONS:\n"
	str_domain += "( vector(x) - 1 )\n"
	str_domain += "EFFECTS:\n"
	str_domain += "( vector(x) -= 1 )\n"

	f_domain=open( out_folder + "domain.txt", "w" )
	f_domain.write( str_domain )
	f_domain.close
		
	# INSTANCES
	for i in range(from_nth,to_nth+1):
		# Problem name
		str_problem = "TRIANGULAR-SUM-" + str(i) + "\n"
		
		# Objects
		str_problem += "\nOBJECTS:\n"
		str_problem += "p0:object\n"
		str_problem += "p1:object\n"
		
		# Initial state
		str_problem += "\nINIT:\n"
		str_problem += "( vector(p0) = 0 )\n"
		str_problem += "( vector(p1) = " + str(i) + " )\n"
		
		# Compute
		sol = i*(i+1)//2
		
		# Goal condition
		str_problem += "\nGOAL:\n"
		str_problem += "( vector(p0) = " + str(sol) + " )\n"
		str_problem += "( vector(p1) = 0 )\n"
		
		#print( str_problem )
		f_problem=open( out_folder + str( i+1-from_nth ) + ".txt","w")
		f_problem.write( str_problem )
		f_problem.close()
	
	sys.exit( 0 )
	
if __name__ == "__main__":
	main()
