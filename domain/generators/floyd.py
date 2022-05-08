#!/usr/bin/env python
import sys, random
from tarski.io import PDDLReader
import PDDLtranslator


#*****************#
# Functions declarations
#*****************#
def get_objects(num_dimension):

   unconnected = [[1,2]]
   for i in range(1, num_dimension):
      aux = unconnected[-1]
      unconnected.append([aux[0]+1,(aux[1]+2)%num_dimension])
   if num_dimension <=3:
      unconnected=[]
      

   str_objects = ""
   for i in range(1, num_dimension+1):
         for j in range(1, num_dimension+1):
            if not [i,j] in unconnected:
               str_objects = str_objects + " v" + str(i) + "-" + str(j)
   str_objects = str_objects + " - object"

   return(str_objects)
   

#*****************#
def get_init(num_dimension):

   unconnected = [[1,2]]
   for i in range(1, num_dimension):
      aux = unconnected[-1]
      unconnected.append([aux[0]+1,(aux[1]+2)%num_dimension])
   if num_dimension <=3:
      unconnected=[]

   str_init="\n"
   for i in range(1, num_dimension+1):
         for j in range(1, num_dimension):
            if (not [i,j] in unconnected) and (not [i,j+1] in unconnected):
               str_init = str_init + "(connected v" + str(i) + "-" + str(j) + " v" + str(i) + "-" + str(j+1)+") "
               str_init = str_init + "(connected v" + str(i) + "-" + str(j+1) + " v" + str(i) + "-" + str(j)+") "
         str_init = str_init + "\n"

   str_init = str_init + "\n"

   for i in range(1, num_dimension+1):
         for j in range(1, num_dimension):
            if (not [j,i] in unconnected) and (not [j+1,i] in unconnected):            
               str_init = str_init + "(connected v" + str(j) + "-" + str(i) + " v" + str(j+1) + "-" + str(i)+") "
               str_init = str_init + "(connected v" + str(j+1) + "-" + str(i) + " v" + str(j) + "-" + str(i)+") "
         str_init = str_init + "\n"

   return(str_init)


#*****************#
def get_goals(num_dimension):
   str_goal=""
   str_goal=str_goal+"\n  (and\n"

   str_goal=str_goal+ "   (connected v1-1 v" + str(num_dimension) + "-" + str(num_dimension) +")\n"
            
   str_goal=str_goal+")"
   return(str_goal)
#*****************#

def generate_problem(name,ndimension):
   str_out = ""
   str_out += "(define (problem "+name+")\n"
   str_out += " (:domain FLOYD)\n"
   str_out += " (:objects "+ get_objects(ndimension)+")\n"
   str_out += " (:init " + get_init(ndimension)+")\n"
   str_out += " (:goal"+ get_goals(ndimension)+"))\n"
   return str_out
#*****************#

def generate_domain():
   str_out = ""
   str_out += "(define (domain FLOYD)\n"
   str_out += "  (:requirements :strips)\n"
   str_out += "  (:predicates (connected ?x ?y))\n"
   str_out += "\n"
   str_out += "  (:action connect\n"
   str_out += "     :parameters (?x ?y ?z)\n"
   str_out += "     :precondition (and (connected ?x ?y) (connected ?y ?z))\n"
   str_out += "     :effect (and (connected ?x ?z) (connected ?z ?x)))\n"
   str_out += ")"
   return str_out  


#*****************#
# MAIN
#*****************#
# Reading the command line arguments

def main():
   try:
      from_nth = int( sys.argv[1] )
      to_nth = int( sys.argv[2] )
      extra_pointers = int( sys.argv[3] )
      out_folder = sys.argv[4]
   except:
      print("Usage: ")
      print(sys.argv[ 0 ] + " <from_n> <to_n> <extra_pointers> <outfolder>")
      print(sys.argv[ 0 ] + " 6 12 0 tmp/")
      sys.exit(-1)

   # GENERATION
   vdimension = range(from_nth,to_nth+1)

   # DOMAIN
   str_domain = generate_domain()
   f_domain=open( out_folder + "domain.pddl", "w" )
   f_domain.write( str_domain )
   f_domain.close()


   # INSTANCES
   random.seed(1007)
   
   for i in range(from_nth,to_nth+1):
      # Problem name
      problem_name = "FLOYD-" + str( i+1-from_nth )

      str_problem = generate_problem(problem_name,vdimension[i-from_nth])
         
      f_problem = open( out_folder + str( i+1-from_nth ) + ".pddl","w")
      f_problem.write( str_problem )
      f_problem.close()


   # TRANSLATION
   for i in range(from_nth,to_nth+1):
      reader = PDDLReader(raise_on_error=True)
      reader.parse_domain(out_folder +"domain.pddl")

      problem = reader.parse_instance(out_folder + str( i+1-from_nth ) + ".pddl")
      PDDLtranslator.translate_domain(problem,extra_pointers,out_folder +"domain.txt")
      PDDLtranslator.translate_problem(problem,out_folder + str( i+1-from_nth ) + ".txt")   

   sys.exit(0)

if __name__ == "__main__":
   main()



