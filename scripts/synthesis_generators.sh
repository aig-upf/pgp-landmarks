[ ! -d "domain/" ] && mkdir -p "domain"
[ ! -d "domain/synthesis/" ] && mkdir -p "domain/synthesis"
[ ! -d "experiments/" ] && mkdir -p "experiments"
[ ! -d "experiments/synthesis/" ] && mkdir -p "experiments/synthesis"

echo 'Generating Baking synthesis instances...'
if [ -d "domain/synthesis/baking/" ]; then
	rm -rf domain/synthesis/baking/*
else
	mkdir -p "domain/synthesis/baking"
fi 
PYTHONHASHSEED=1 python domain/generators/baking.py 1 10 0 domain/synthesis/baking/

echo 'Generating Blocks Ontable synthesis instances...'
if [ -d "domain/synthesis/ontable/" ]; then
	rm -rf domain/synthesis/ontable/*
else
	mkdir -p "domain/synthesis/ontable"
fi 
PYTHONHASHSEED=1 python domain/generators/ontable.py 10 15 1 domain/synthesis/ontable/

echo 'Generating Corridor-STRIPS synthesis instances...'
if [ -d "domain/synthesis/corridor-strips/" ]; then
	rm -rf domain/synthesis/corridor-strips/*
else
	mkdir -p "domain/synthesis/corridor-strips"
fi 
PYTHONHASHSEED=1 python domain/generators/corridor-strips.py 5 14 domain/synthesis/corridor-strips/

echo 'Generating Gripper synthesis instances...'
if [ -d "domain/synthesis/gripper/" ]; then
	rm -rf domain/synthesis/gripper/*
else
	mkdir -p "domain/synthesis/gripper"
fi 
PYTHONHASHSEED=1 python domain/generators/gripper.py 2 11 domain/synthesis/gripper/

echo 'Generating Intrusion synthesis instances...'
if [ -d "domain/synthesis/intrusion/" ]; then
	rm -rf domain/synthesis/intrusion/*
else
	mkdir -p "domain/synthesis/intrusion"
fi 
PYTHONHASHSEED=1 python domain/generators/intrusion.py 1 10 0 domain/synthesis/intrusion/

echo 'Generating Lock synthesis instances...'
if [ -d "domain/synthesis/lock/" ]; then
	rm -rf domain/synthesis/lock/*
else
	mkdir -p "domain/synthesis/lock"
fi 
PYTHONHASHSEED=1 python domain/generators/lock.py 5 14 0 domain/synthesis/lock/

echo 'Generating Spanner synthesis instances...'
if [ -d "domain/synthesis/spanner/" ]; then
	rm -rf domain/synthesis/spanner/*
else
	mkdir -p "domain/synthesis/spanner"
fi 
PYTHONHASHSEED=1 python domain/generators/spanner.py 1 10 0 domain/synthesis/spanner/

# AIJ new
echo 'Generating Visitall synthesis instances...'
if [ -d "domain/synthesis/visitall/" ]; then
	rm -rf domain/synthesis/visitall/*
else
	mkdir -p "domain/synthesis/visitall"
fi 
PYTHONHASHSEED=1 python domain/generators/visitall.py 2 11 domain/synthesis/visitall/
