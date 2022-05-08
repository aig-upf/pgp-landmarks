experiments_file="experiments/"$(echo $4 | cut -d'/' -f 2-3)
synthesis="./main.bin"
validator="./validator.bin"
argc=$#
argv=("$@")

# Add search mode for synthesis (BFS | PGP)
if [ $1 -eq $synthesis ]; then
	experiments_file=$experiments_file"_${argv[1]}_${argv[2]}"
fi

# Add heuristics
for (( j=4; j < argc; j++ )); do
    experiments_file=$experiments_file"_${argv[j]}"
done

if [ $validator != $1 ] && [ $# -eq 4 ]; then
	experiments_file=$experiments_file"_landmarks"
fi

experiments_file=$experiments_file".out"
echo $experiments_file
timeout 3600 /usr/bin/time -f "time result\ncmd:%C\nreal %es\nmemory:%MKB \n" $@ 2>> $experiments_file
#/usr/bin/time -f "time result\ncmd:%C\nreal %es\nuser %Us \nsys  %Ss \nmemory:%MKB \ncpu %P" $@
