echo 'Generating folders...'
[ ! -d "domain/" ] && mkdir -p "domain"

[ ! -d "domain/synthesis/" ] && mkdir -p "domain/synthesis"

[ ! -d "domain/synthesis/baking/" ] && mkdir -p "domain/synthesis/baking"
[ ! -d "domain/synthesis/corridor-strips/" ] && mkdir -p "domain/synthesis/corridor-strips"
[ ! -d "domain/synthesis/gripper/" ] && mkdir -p "domain/synthesis/gripper"
[ ! -d "domain/synthesis/intrusion/" ] && mkdir -p "domain/synthesis/intrusion"
[ ! -d "domain/synthesis/lock/" ] && mkdir -p "domain/synthesis/lock"
[ ! -d "domain/synthesis/ontable/" ] && mkdir -p "domain/synthesis/ontable"
[ ! -d "domain/synthesis/spanner/" ] && mkdir -p "domain/synthesis/spanner"
[ ! -d "domain/synthesis/visitall/" ] && mkdir -p "domain/synthesis/visitall"

[ ! -d "domain/validation/" ] && mkdir -p "domain/validation"

[ ! -d "domain/validation/baking/" ] && mkdir -p "domain/validation/baking"
[ ! -d "domain/validation/corridor-strips/" ] && mkdir -p "domain/validation/corridor-strips"
[ ! -d "domain/validation/gripper/" ] && mkdir -p "domain/validation/gripper"
[ ! -d "domain/validation/intrusion/" ] && mkdir -p "domain/validation/intrusion"
[ ! -d "domain/validation/lock/" ] && mkdir -p "domain/validation/lock"
[ ! -d "domain/validation/ontable/" ] && mkdir -p "domain/validation/ontable"
[ ! -d "domain/validation/spanner/" ] && mkdir -p "domain/validation/spanner"
[ ! -d "domain/validation/visitall/" ] && mkdir -p "domain/validation/visitall"

[ ! -d "experiments/" ] && mkdir -p "experiments"
[ ! -d "experiments/synthesis/" ] && mkdir -p "experiments/synthesis"
[ ! -d "experiments/validation/" ] && mkdir -p "experiments/validation"

echo 'Compiling the main project...'
./scripts/compile_main.sh
echo 'Compiling the validator...'
./scripts/compile_validator.sh
