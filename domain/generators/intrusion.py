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
      str_objects = str_objects + " h" + str(i) 
   str_objects = str_objects + " - host"

   return(str_objects)
   

#*****************#
def get_init(num_dimension):

   str_init="\n"
   str_init = str_init + "     (dummy)"
   str_init = str_init + "\n"

   return(str_init)


#*****************#
def get_goals(num_dimension):

   str_goal=""
   str_goal=str_goal+"\n  (and"

   for i in range(1, num_dimension+1):
      str_goal=str_goal+ " (data-stolen-from h" + str(i) +")"

   str_goal=str_goal+")"
   return(str_goal)
#*****************#


def generate_problem(name,ndimension):
   str_out = ""
   str_out += "(define (problem "+name+")\n"
   str_out += " (:domain INTRUSION)\n"
   str_out += " (:objects "+ get_objects(ndimension)+")\n"
   str_out += " (:init " + get_init(ndimension)+")\n"
   str_out += " (:goal"+ get_goals(ndimension)+"))\n"
   return str_out
#*****************#


def generate_domain():
   str_out = ""
   str_out += "(define (domain INTRUSION)\n"
   str_out += "   (:requirements :strips :typing)\n"
   str_out += "   (:types host)\n"
   str_out += "   (:predicates (recon-performed ?h - host) (broke-into ?h - host) (deleted-logs ?h - host) (modified-files ?h - host) (access-obtained ?h - host) (root-access-obtained ?h - host) (files-downloaded ?h - host) (information-gathered ?h - host) (vandalized ?h - host) (data-stolen-from ?h - host) (dummy))\n"
   str_out += "\n"
   str_out += " (:action recon\n"
   str_out += "     :parameters (?h - host)\n"
   str_out += "     :precondition (and (dummy))\n"
   str_out += "     :effect (and (recon-performed ?h)))\n"
   str_out += "\n"
   str_out += " (:action information-gathering\n"
   str_out += "     :parameters (?h - host)\n"
   str_out += "     :precondition (and (recon-performed ?h))\n"
   str_out += "     :effect (and (information-gathered ?h)))\n"
   str_out += "\n"   
   str_out += " (:action break-into\n"
   str_out += "     :parameters (?h - host)\n"
   str_out += "     :precondition (and (recon-performed ?h))\n"
   str_out += "     :effect (and (access-obtained ?h)))\n"
   str_out += "\n"   
   str_out += " (:action modify-files\n"
   str_out += "     :parameters (?h - host)\n"
   str_out += "     :precondition (and (access-obtained ?h))\n"
   str_out += "     :effect (and (modified-files ?h)))\n"
   str_out += "\n"   
   str_out += " (:action clean\n"
   str_out += "     :parameters (?h - host)\n"
   str_out += "     :precondition (and (access-obtained ?h))\n"
   str_out += "     :effect (and (deleted-logs ?h)))\n"
   str_out += "\n"
   str_out += " (:action vandalize\n"
   str_out += "     :parameters (?h - host)\n"
   str_out += "     :precondition (and (modified-files ?h) (deleted-logs ?h))\n"
   str_out += "     :effect (and (vandalized ?h)))\n"
   str_out += "\n"   
   str_out += " (:action gain-root\n"
   str_out += "     :parameters (?h - host)\n"
   str_out += "     :precondition (and (access-obtained ?h))\n"
   str_out += "     :effect (and (root-access-obtained ?h)))\n"
   str_out += "\n"
   str_out += " (:action download-files\n"
   str_out += "     :parameters (?h - host)\n"
   str_out += "     :precondition (and (root-access-obtained ?h))\n"
   str_out += "     :effect (and (files-downloaded ?h)))\n"
   str_out += "\n"   
   str_out += " (:action steal-data\n"
   str_out += "     :parameters (?h - host)\n"
   str_out += "     :precondition (and (files-downloaded ?h) (deleted-logs ?h))\n"
   str_out += "     :effect (and (data-stolen-from ?h)))\n"

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
      print(sys.argv[ 0 ] + " 1 10 0 tmp/")
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
      problem_name = "INTRUSION-" + str( i+1-from_nth )

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



