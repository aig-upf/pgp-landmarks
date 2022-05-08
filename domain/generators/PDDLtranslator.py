import sys
from tarski.io import PDDLReader

def translate_domain(tproblem,extra_pointers,file_name):

	str_out=""
	str_out+=tproblem.domain_name + "\n"

	str_out+="\n"
	str_out+="STATE DESCRIPTOR:\n"
	for t in tproblem.language.sorts:
		if not str(t.name) in ["Real","Integer","Natural","number"]:
			if t.name!="object" or len(tproblem.language.sorts)==1:
				str_out+=str(t.name) + ":var_type\n"

	for p in tproblem.language.predicates:
		if not str(p.name) in ["=","!=","<","<=",">",">="]:
			str_out+=str(p.name)+"("

			str_aux=""
			for a in p.sort:
				str_aux+=(a.name)+","
			str_aux += "):pred_type"+"\n"
			str_out += str_aux.replace(",)",")") 
  
	for t in tproblem.language.sorts:
		if not str(t.name) in ["Real","Integer","Natural","number"]:
			nptrs=0
			for p in tproblem.language.predicates:
				counter=0
				if not str(p.name) in ["=","!=","<","<=",">",">="]:				
					for a in p.sort:
						if str(t.name) == str(a.name):
							counter = counter + 1
			nptrs=max(nptrs,counter)
			for aname in list(tproblem.actions):
				counter=0
				action = tproblem.get_action(aname)			
				for a in action.parameters:
					if str(t.name) == str(a.sort.name):
						counter = counter + 1
				nptrs=max(nptrs,counter)
			for i in range(nptrs+extra_pointers):
				str_out += "ptr-"+t.name+str(i+1)+":"+(t.name)+"\n"



	for aname in list(tproblem.actions):
		action = tproblem.get_action(aname)
		str_out+="\n"		
		str_out+="ACTION: " + aname + "("

		str_aux=""
		for p in action.parameters:
			str_aux+=str(p) + ":" + p.sort.name + ","

		str_aux+=")\n"
		str_out+=str_aux.replace("?","").replace(",)",")")

		str_out+="TYPE: memory\n"

		str_out+="PRECONDITIONS:\n"
		if type(action.precondition).__name__=="CompoundFormula":
			for p in action.precondition.subformulas:
				if type(p).__name__=="CompoundFormula": # NegatedAtom
					str_out+="( "+ str(p.subformulas[0]).replace("?","")+" = 0 )\n"					
				else:
					str_out+="( "+ str(p).replace("?","")+" = 1 )\n"
		if type(action.precondition).__name__=="Atom":
			str_out+="( "+ str(action.precondition).replace("?","")+" = 1 )\n"

		str_out+="EFFECTS:\n"
		for p in action.effects:
			if type(p).__name__=="DelEffect":
				str_out+="( "+ str(p.atom).replace("?","")+" = 0 )\n"
			if type(p).__name__=="AddEffect":
				str_out+="( "+ str(p.atom).replace("?","")+" = 1 )\n"

	f_domain = open(file_name,"w")
	f_domain.write(str_out)
	f_domain.close()
	return



def translate_problem(tproblem,file_name):
	str_out=""

	str_out+=tproblem.name + "\n"

	str_out+="\n"
	str_out+="OBJECTS:\n"
	for o in tproblem.language.constants():
		str_out+=str(o) + ":" + str(o.sort.name) + "\n"

	str_out+="\n"
	str_out+="INIT:\n"
	for a in tproblem.init.as_atoms():
		str_out+="( "+ str(a).replace("?","")+" = 1 )\n"

	str_out+="\n"
	str_out+="GOAL:\n"
	if type(tproblem.goal).__name__=="CompoundFormula":
		for p in tproblem.goal.subformulas:
			str_out+="( "+ str(p).replace("?","")+" = 1 )\n"
	if type(tproblem.goal).__name__=="Atom":
		str_out+="( "+ str(tproblem.goal).replace("?","")+" = 1 )\n"

	f_domain = open(file_name,"w")
	f_domain.write(str_out)
	f_domain.close()
	return	




def main():
	try:
		domain_file_name = sys.argv[1]
		problem_file_name = sys.argv[2]
	except:
		print(sys.argv[ 0 ] + " <pddl domain file> <pddl problem file>")
		sys.exit(-1)

	reader = PDDLReader(raise_on_error=True)
	reader.parse_domain(domain_file_name)
	problem = reader.parse_instance(problem_file_name)

	translate_domain(problem,"domain.txt")
	translate_problem(problem,"problem.txt")

	sys.exit(0)
	
if __name__ == "__main__":
	main()
