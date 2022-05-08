#!/usr/bin/env python
import sys, random
from tarski.io import PDDLReader
import PDDLtranslator


#*****************#
# Functions declarations
#*****************#
def get_objects(num_dimension):
   ncars = num_dimension
   nlocs = int(num_dimension*1.5)

   str_objects = ""
   for i in range(1, ncars+1):
      str_objects = str_objects + " c" + str(i) 
   str_objects = str_objects + " - car"

   for i in range(1, nlocs+1):
      str_objects = str_objects + " l" + str(i) 
   str_objects = str_objects + " - location"

   return(str_objects)
   

#*****************#
def get_init(num_dimension):
   ncars = num_dimension
   nlocs = int(num_dimension*1.5)

   str_init="\n"
   str_init = str_init + "     (empty-ferry) (at-ferry l1)\n"   
   str_init = str_init + "     " 
   random.seed(1006)           
   for i in range(1, ncars+1):
      str_init=str_init+ " (at c" + str(i) +  " l" + str(random.randint(1, nlocs))+")"
   str_init = str_init + "\n"

   random.seed(1007)
   str_init = str_init + "     "   
   for i in range(1, ncars+1):
      str_init=str_init+ " (goal-at c" + str(i) +  " l" + str(random.randint(1, nlocs)) + ")"
   str_init = str_init + "\n"


   return(str_init)


#*****************#
def get_goals(num_dimension):
   ncars = num_dimension
   nlocs = int(num_dimension*1.5)

   str_goal=""
   str_goal=str_goal+"\n  (and"

   random.seed(1007)
   for i in range(1, ncars+1):
      str_goal=str_goal+ " (at c" + str(i) +  " l" + str(random.randint(1, nlocs)) + ")"

   str_goal=str_goal+")"
   return(str_goal)
#*****************#


def generate_problem(name,ndimension):
   str_out = ""
   str_out += "(define (problem "+name+")\n"
   str_out += " (:domain FERRY)\n"
   str_out += " (:objects "+ get_objects(ndimension)+")\n"
   str_out += " (:init " + get_init(ndimension)+")\n"
   str_out += " (:goal"+ get_goals(ndimension)+"))\n"
   return str_out
#*****************#


def generate_domain():
   str_out = ""
   str_out += "(define (domain FERRY)\n"
   str_out += "  (:requirements :strips)\n"
   str_out += "  (:types car location)\n"
   str_out += "  (:predicates (at-ferry ?l - location) (at ?c - car ?l - location) (empty-ferry) (on ?c - car) (goal-at ?c - car ?l - location))\n"
   str_out += "\n"
   str_out += " (:action sail\n"
   str_out += "   :parameters  (?x - location ?y - location)\n"
   str_out += "   :precondition (and (at-ferry ?x) (not (at-ferry ?y)))\n"
   str_out += "   :effect (and  (at-ferry ?y) (not (at-ferry ?x))))\n"
   str_out += "\n" 
   str_out += " (:action board\n"
   str_out += "    :parameters (?x - car ?y - location)\n"
   str_out += "    :precondition  (and  (at ?x ?y) (at-ferry ?y) (empty-ferry) (not (goal-at ?x ?y)))\n"
   str_out += "    :effect (and (on ?x) (not (at ?x ?y)) (not (empty-ferry))))\n"
   str_out += "\n"
   str_out += " (:action debark\n"
   str_out += "    :parameters  (?x - car ?y - location)\n"
   str_out += "    :precondition  (and  (on ?x) (at-ferry ?y) (goal-at ?x ?y))\n"
   str_out += "    :effect (and (at ?x ?y) (empty-ferry) (not (on ?x))))\n"
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
      print(sys.argv[ 0 ] + " 10 15 0 tmp/")
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
      problem_name = "FERRY-" + str( i+1-from_nth )

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



