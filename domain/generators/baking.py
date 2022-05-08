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
      str_objects = str_objects + " e" + str(i) 
   str_objects = str_objects + " - egg"

   for i in range(1, num_dimension+1):
      str_objects = str_objects + " f" + str(i)
   str_objects = str_objects + " - flour"      

   for i in range(1, num_dimension+1):
      str_objects = str_objects + " c" + str(i)            
   str_objects = str_objects + " - cake"


   for i in range(1, num_dimension):
      str_objects = str_objects + " s" + str(i+1)  
   str_objects = str_objects + " s1 - soap"      

   str_objects = str_objects + " p1 - pan o1 - oven"      

   return(str_objects)
   

#*****************#
def get_init(num_dimension):

   str_init="\n"
   str_init = str_init + "     (panisclean p1)"
   str_init = str_init + "\n"

   return(str_init)


#*****************#
def get_goals(num_dimension):

   str_goal=""
   str_goal=str_goal+"\n  (and (panisclean p1)"
   #str_goal=str_goal+"\n  (and "

   for i in range(1, num_dimension+1):
      str_goal=str_goal+ " (baked c" + str(i) +")"

   str_goal=str_goal+")"
   return(str_goal)
#*****************#


def generate_problem(name,ndimension):
   str_out = ""
   str_out += "(define (problem "+name+")\n"
   str_out += " (:domain BAKING)\n"
   str_out += " (:objects "+ get_objects(ndimension)+")\n"
   str_out += " (:init " + get_init(ndimension)+")\n"
   str_out += " (:goal"+ get_goals(ndimension)+"))\n"
   return str_out
#*****************#


def generate_domain():
   str_out = ""
   str_out += "(define (domain BAKING)\n"
   str_out += " (:requirements :typing)\n"
   str_out += " (:types egg flour cake pan oven soap)\n"
   str_out += " (:predicates (panhasegg ?p - pan) (panhasflour ?p - pan) (panisclean ?p - pan) (paninoven ?p - pan) (inoven ?p - pan ?o - oven) (ovenisfull ?o - oven) (ismixed ?p - pan) (baked ?n - cake) (flourconsumed ?f - flour) (eggconsumed ?e - egg) (soapconsumed ?s - soap))\n"
   str_out += "\n"    
   str_out += " (:action putegginpan\n"
   str_out += "    :parameters (?e - egg ?p - pan)\n"
   str_out += "    :precondition (and (not(eggconsumed ?e)) (not (panhasegg ?p)) (not (ismixed ?p)) (panisclean ?p) (not (paninoven ?p)))\n"
   str_out += "    :effect (and (eggconsumed ?e) (panhasegg ?p)))\n"
   str_out += "\n" 
   str_out += " (:action putflourinpan\n"
   str_out += "    :parameters (?f - flour ?p - pan)\n"
   str_out += "    :precondition (and (not(flourconsumed ?f)) (not (panhasflour ?p)) (not (ismixed ?p)) (panisclean ?p) (not (paninoven ?p)))\n"
   str_out += "    :effect (and (flourconsumed ?f) (panhasflour ?p)))\n"
   str_out += "\n"
   str_out += " (:action mix\n"
   str_out += "    :parameters (?e - egg ?f - flour ?p - pan)\n"
   str_out += "    :precondition (and (panhasflour ?p) (panhasegg ?p) (not (paninoven ?p)))\n"
   str_out += "    :effect (and (ismixed ?p) (not (panhasflour ?p)) (not (panhasegg ?p))))\n"
   str_out += "\n"
   str_out += " (:action putpaninoven\n"
   str_out += "    :parameters (?p - pan ?o - oven)\n"
   str_out += "    :precondition (and (not (ovenisfull ?o)) (not (paninoven ?p)))\n"
   str_out += "    :effect (and (ovenisfull ?o) (inoven ?p ?o) (paninoven ?p)))\n"
   str_out += "\n"
   str_out += " (:action removepanfromoven\n"
   str_out += "    :parameters (?p - pan ?o - oven)\n"
   str_out += "    :precondition (and (inoven ?p ?o))\n"
   str_out += "    :effect (and (not (ovenisfull ?o)) (not (inoven ?p ?o)) (not (paninoven ?p))))\n"
   str_out += "\n"
   str_out += " (:action bakecake\n"
   str_out += "    :parameters (?o - oven ?p - pan ?c - cake)\n"
   str_out += "    :precondition (and (ismixed ?p) (inoven ?p ?o))\n"
   str_out += "    :effect (and (not (ismixed ?p)) (not (panisclean ?p)) (baked ?c)))\n"
   str_out += "\n"
   str_out += "  (:action cleanpan\n"
   str_out += "    :parameters (?p - pan ?s - soap)\n"
   str_out += "    :precondition (and (not (soapconsumed ?s))\n"
   str_out += "                  (not (paninoven ?p)))\n"
   str_out += "    :effect (and (panisclean ?p) (soapconsumed ?s)))\n"

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
      problem_name = "BAKING-" + str( i+1-from_nth )

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



