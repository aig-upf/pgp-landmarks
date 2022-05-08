#!/usr/bin/env python
import sys, random
from tarski.io import PDDLReader
import PDDLtranslator


#*****************#
# Functions declarations
#*****************#
def get_objects(num_dimension):
   str_objects = ""
   for i in range(1, num_dimension+1):
      str_objects = str_objects + " c" + str(i) 

   return(str_objects)
   

#*****************#
def get_init(num_dimension):

   str_init="\n"
   str_init = str_init + "     (at-key c"+str(num_dimension)+") (at-lock c1)\n"   
   str_init = str_init + "     " 
   str_init=str_init+ " (at-agent c" + str(random.randint(1, num_dimension))+")"
   for i in range(1, num_dimension):
      str_init=str_init+ " (adjacent c" + str(i) +  " c" + str(i+1)+")"
      str_init=str_init+ " (adjacent c" + str(i+1) +  " c" + str(i)+")"
   str_init = str_init + "\n"

   return(str_init)


#*****************#
def get_goals(num_dimension):

   str_goal=""
   str_goal=str_goal+"\n  (and"

   str_goal=str_goal+ " (unlocked)"

   str_goal=str_goal+")"
   return(str_goal)
#*****************#


def generate_problem(name,ndimension):
   str_out = ""
   str_out += "(define (problem "+name+")\n"
   str_out += " (:domain LOCK)\n"
   str_out += " (:objects "+ get_objects(ndimension)+")\n"
   str_out += " (:init " + get_init(ndimension)+")\n"
   str_out += " (:goal"+ get_goals(ndimension)+"))\n"
   return str_out
#*****************#


def generate_domain():
   str_out = ""
   str_out += "(define (domain LOCK)\n"
   str_out += "  (:requirements :strips)\n"
   str_out += "  (:predicates (at-agent ?x) (at-key ?x) (at-lock ?x) (has-agent-key) (adjacent ?x ?y) (unlocked))\n"
   str_out += "\n"
   str_out += " (:action move\n"
   str_out += "   :parameters  (?x ?y)\n"
   str_out += "   :precondition (and (adjacent ?x ?y) (at-agent ?x)(not (at-agent ?y)))\n"
   str_out += "   :effect (and (at-agent ?y) (not (at-agent ?x))))\n"
   str_out += "\n"
   str_out += " (:action pickup\n"
   str_out += "   :parameters  (?x)\n"
   str_out += "   :precondition (and (at-agent ?x) (at-key ?x))\n"
   str_out += "   :effect (and (has-agent-key) (not (at-key ?x))))\n"   
   str_out += "\n"
   str_out += " (:action drop\n"
   str_out += "   :parameters  (?x)\n"
   str_out += "   :precondition (and (at-agent ?x) (has-agent-key))\n"
   str_out += "   :effect (and  (at-key ?x)))\n"
   str_out += "\n"
   str_out += "  (:action open\n"
   str_out += "   :parameters  (?x)\n"
   str_out += "   :precondition (and (at-agent ?x) (at-lock ?x) (has-agent-key))\n"
   str_out += "   :effect (and  (unlocked)))\n"
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
      print(sys.argv[ 0 ] + " 5 10 0 tmp/")
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
      problem_name = "LOCK-" + str( i+1-from_nth )

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



