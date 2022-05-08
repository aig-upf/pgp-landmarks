[ ! -d "domain/" ] && mkdir -p "domain"
[ ! -d "domain/validation/" ] && mkdir -p "domain/validation"
[ ! -d "experiments/" ] && mkdir -p "experiments/"
[ ! -d "experiments/validation/" ] && mkdir -p "experiments/validation"

echo 'Generating Baking validation instances...'
if [ -d "domain/validation/baking/" ]; then
	rm -rf domain/validation/baking/*
else
	mkdir -p "domain/validation/baking"
fi 
PYTHONHASHSEED=1 python domain/generators/baking.py 11 60 0 domain/validation/baking/

# AIJ domain
echo 'Generating Blocks Ontable validation instances...'
if [ -d "domain/validation/ontable/" ]; then
	rm -rf domain/validation/ontable/*
else
	mkdir -p "domain/validation/ontable"
fi 
PYTHONHASHSEED=1 python domain/generators/ontable.py 16 65 1 domain/validation/ontable/

echo 'Generating Corridor-STRIPS validation instances...'
if [ -d "domain/validation/corridor-strips/" ]; then
	rm -rf domain/validation/corridor-strips/*
else
	mkdir -p "domain/validation/corridor-strips"
fi 
PYTHONHASHSEED=1 python domain/generators/corridor-strips.py 12 61 domain/validation/corridor-strips/

echo 'Generating Gripper validation instances...'
if [ -d "domain/validation/gripper/" ]; then
	rm -rf domain/validation/gripper/*
else
	mkdir -p "domain/validation/gripper"
fi 
PYTHONHASHSEED=1 python domain/generators/gripper.py 12 61 domain/validation/gripper/

echo 'Generating Intrusion validation instances...'
if [ -d "domain/validation/intrusion/" ]; then
	rm -rf domain/validation/intrusion/*
else
	mkdir -p "domain/validation/intrusion"
fi 
PYTHONHASHSEED=1 python domain/generators/intrusion.py 11 60 0 domain/validation/intrusion/

echo 'Generating Lock validation instances...'
if [ -d "domain/validation/lock/" ]; then
	rm -rf domain/validation/lock/*
else
	mkdir -p "domain/validation/lock"
fi 
PYTHONHASHSEED=1 python domain/generators/lock.py 12 61 0 domain/validation/lock/

echo 'Generating Spanner validation instances...'
if [ -d "domain/validation/spanner/" ]; then
	rm -rf domain/validation/spanner/*
else
	mkdir -p "domain/validation/spanner"
fi 
PYTHONHASHSEED=1 python domain/generators/spanner.py 12 61 0 domain/validation/spanner/

echo 'Generating Visitall validation instances...'
if [ -d "domain/validation/visitall/" ]; then
	rm -rf domain/validation/visitall/*
else
	mkdir -p "domain/validation/visitall"
fi 
PYTHONHASHSEED=1 python domain/generators/visitall.py 12 61 domain/validation/visitall/


