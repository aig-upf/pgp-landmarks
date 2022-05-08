#!/usr/bin/env python
import sys, random
from tarski.io import PDDLReader
import PDDLtranslator


#*****************#
# Functions declarations
#*****************#
def get_objects(num_dimension):

   nlevels = num_dimension + 2
   nnodes = (2**(num_dimension + 1)-1)

   str_objects = ""
   for i in range(1, nlevels+1):
      str_objects = str_objects + " l" + str(i) 
   str_objects = str_objects + " - level"

   for i in range(1, nnodes+1):
      str_objects = str_objects + " n" + str(i) 
   str_objects = str_objects + " - node"

   return(str_objects)
   

#*****************#
def get_init(num_dimension):
   nlevels = num_dimension +1
   nnodes = (2**(num_dimension)-1)   

   str_init="\n"
   str_init = str_init + "     (current n1) (top l1)\n"   
   str_init = str_init + "     "      
   for i in range(1, nlevels+1):
      str_init = str_init + "(next l" + str(i) + " l" + str(i+1) + ") "

   str_init = str_init + "\n"
   str_init = str_init + "     "   
   for i in range(1, nnodes+1):
      str_init = str_init + "(connected n" + str(i) + " n" + str(2*i) + ") " 
      str_init = str_init + "(connected n" + str(i) + " n" + str(2*i+1) + ") "           
   str_init = str_init + "\n"

   return(str_init)


#*****************#
def get_goals(num_dimension):
   nnodes = (2**(num_dimension + 1)-1)
   str_goal=""
   str_goal=str_goal+"\n  (and"

   for i in range(1, nnodes+1):
      str_goal=str_goal+ " (visited n" + str(i) + ")"

   str_goal=str_goal+")"
   return(str_goal)
#*****************#


def generate_problem(name,ndimension):
   str_out = ""
   str_out += "(define (problem "+name+")\n"
   str_out += " (:domain DFS)\n"
   str_out += " (:objects "+ get_objects(ndimension)+")\n"
   str_out += " (:init " + get_init(ndimension)+")\n"
   str_out += " (:goal"+ get_goals(ndimension)+"))\n"
   return str_out
#*****************#


def generate_domain():
   str_out = ""
   str_out += "(define (domain DFS)\n"
   str_out += "  (:requirements :strips)\n"
   str_out += "  (:types node level)\n"
   str_out += "  (:predicates (stack ?n - node ?l - level) (top ?l - level) (next ?l1 ?l2 - level) (connected ?n1 ?n2 - node) (visited ?n - node) (current ?n - node))\n"
   str_out += "\n"
   str_out += "  (:action push\n"
   str_out += "     :parameters (?n1 ?n2 - node ?l1 ?l2 - level)\n"
   str_out += "     :precondition (and (top ?l1) (next ?l1 ?l2) (current ?n1) (visited ?n1) (not (visited ?n2)) (connected ?n1 ?n2))\n"
   str_out += "     :effect (and (not (top ?l1)) (top ?l2) (stack ?n2 ?l2)))\n"
   str_out += "\n"
   str_out += " (:action pop\n"
   str_out += "     :parameters (?n1 ?n2 - node ?l1 ?l2 - level)\n"
   str_out += "     :precondition (and (top ?l2) (next ?l1 ?l2) (current ?n1) (stack ?n2 ?l2))\n"
   str_out += "     :effect (and (not (top ?l2)) (top ?l1) (not (stack ?n2 ?l2)) (not (current ?n1)) (current ?n2)))\n"
   str_out += "\n"
   str_out += " (:action visit\n"
   str_out += "     :parameters (?n - node)\n"
   str_out += "     :precondition (and (current ?n) (not (visited ?n)))\n"
   str_out += "     :effect (and (current ?n) (visited ?n)))\n"
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
      print(sys.argv[ 0 ] + " 2 5 0 tmp/")
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
      problem_name = "DFS-" + str( i+1-from_nth )

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



