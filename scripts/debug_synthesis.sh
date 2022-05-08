# Long experiment
#domains=( "visitall" "gripper" "intrusion" "corridor-strips" "baking" "lock" "spanner" "ontable" )
#bounds=(7 8 9 11 13 12 12 11)
heuristics=("landmarks" "h5")

# Short experiment
domains=( "visitall" )
bounds=(7)
#heuristics=("landmarks")

total=${#domains[@]}
for h in "${heuristics[@]}"
do
	for (( i=0; i<$(($total)); i++ ))
	do
		./scripts/time_memory.sh ./main.bin $1 ${bounds[i]} "domain/synthesis/${domains[i]}/" $h
	done
    # experiments_file=$experiments_file"_${argv[j]}"
done

