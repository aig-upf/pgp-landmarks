echo 'Validating PGP(f_LM) programs'
echo 'Validating BAKING...'
./validator.bin experiments/synthesis/baking_PGP_13_landmarks.prog domain/validation/baking/ NOINF

echo 'Validating CORRIDOR-STRIPS...'
./validator.bin experiments/synthesis/corridor-strips_PGP_11_landmarks.prog domain/validation/corridor-strips/ NOINF

echo 'Validating GRIPPER...'
./validator.bin experiments/synthesis/gripper_PGP_8_landmarks.prog domain/validation/gripper/ NOINF

echo 'Validating INTRUSION...'
./validator.bin experiments/synthesis/intrusion_PGP_9_landmarks.prog domain/validation/intrusion/ NOINF

echo 'Validating LOCK...'
./validator.bin experiments/synthesis/lock_PGP_12_landmarks.prog domain/validation/lock/ NOINF

echo 'Validating ONTABLE...'
./validator.bin experiments/synthesis/ontable_PGP_11_landmarks.prog domain/validation/ontable/ NOINF

echo 'Validating SPANNER...'
./validator.bin experiments/synthesis/spanner_PGP_12_landmarks.prog domain/validation/spanner/ NOINF

echo 'Validating VISITALL...'
./validator.bin experiments/synthesis/visitall_PGP_7_landmarks.prog domain/validation/visitall/ NOINF



echo 'Validating PGP(f_GC) programs'
#echo 'Validating BAKING...'  # Memory-exceeded
#./validator.bin experiments/synthesis/baking_PGP_13_h5.prog domain/validation/baking/ NOINF

echo 'Validating CORRIDOR-STRIPS...'
./validator.bin experiments/synthesis/corridor-strips_PGP_11_h5.prog domain/validation/corridor-strips/ NOINF

echo 'Validating GRIPPER...'
./validator.bin experiments/synthesis/gripper_PGP_8_h5.prog domain/validation/gripper/ NOINF

echo 'Validating INTRUSION...'
./validator.bin experiments/synthesis/intrusion_PGP_9_h5.prog domain/validation/intrusion/ NOINF

#echo 'Validating LOCK...'  # Memory-exceeded
#./validator.bin experiments/synthesis/lock_PGP_12_h5.prog domain/validation/lock/ NOINF

echo 'Validating ONTABLE...'
./validator.bin experiments/synthesis/ontable_PGP_11_h5.prog domain/validation/ontable/ NOINF

#echo 'Validating SPANNER...'  # Time-exceeded
#./validator.bin experiments/synthesis/spanner_PGP_12_h5.prog domain/validation/spanner/ NOINF

echo 'Validating VISITALL...'
./validator.bin experiments/synthesis/visitall_PGP_7_h5.prog domain/validation/visitall/ NOINF



echo 'Validating BFS(f_GC) programs'
#echo 'Validating BAKING...'  # Time-exceeded
#./validator.bin experiments/synthesis/baking_BFS_13_h5.prog domain/validation/baking/ NOINF

echo 'Validating CORRIDOR-STRIPS...'
./validator.bin experiments/synthesis/corridor-strips_BFS_11_h5.prog domain/validation/corridor-strips/ NOINF

echo 'Validating GRIPPER...'
./validator.bin experiments/synthesis/gripper_BFS_8_h5.prog domain/validation/gripper/ NOINF

echo 'Validating INTRUSION...'
./validator.bin experiments/synthesis/intrusion_BFS_9_h5.prog domain/validation/intrusion/ NOINF

#echo 'Validating LOCK...'  # Time-exceeded
#./validator.bin experiments/synthesis/lock_BFS_12_h5.prog domain/validation/lock/ NOINF

#echo 'Validating ONTABLE...'  # Time-exceeded
#./validator.bin experiments/synthesis/ontable_BFS_11_h5.prog domain/validation/ontable/ NOINF

#echo 'Validating SPANNER...'  # Time-exceeded
#./validator.bin experiments/synthesis/spanner_BFS_12_h5.prog domain/validation/spanner/ NOINF

echo 'Validating VISITALL...'
./validator.bin experiments/synthesis/visitall_BFS_7_h5.prog domain/validation/visitall/ NOINF



echo 'Validating BFS(f_LM) programs'
#echo 'Validating BAKING...'  # Time-exceeded
#./validator.bin experiments/synthesis/baking_BFS_13_landmarks.prog domain/validation/baking/ NOINF

echo 'Validating CORRIDOR-STRIPS...'
./validator.bin experiments/synthesis/corridor-strips_BFS_11_landmarks.prog domain/validation/corridor-strips/ NOINF

echo 'Validating GRIPPER...'
./validator.bin experiments/synthesis/gripper_BFS_8_landmarks.prog domain/validation/gripper/ NOINF

echo 'Validating INTRUSION...'
./validator.bin experiments/synthesis/intrusion_BFS_9_landmarks.prog domain/validation/intrusion/ NOINF

#echo 'Validating LOCK...'  # Time-exceeded
#./validator.bin experiments/synthesis/lock_BFS_12_landmarks.prog domain/validation/lock/ NOINF

#echo 'Validating ONTABLE...'  # Time-exceeded
#./validator.bin experiments/synthesis/ontable_BFS_11_landmarks.prog domain/validation/ontable/ NOINF

#echo 'Validating SPANNER...'  # Time-exceeded
#./validator.bin experiments/synthesis/spanner_BFS_12_landmarks.prog domain/validation/spanner/ NOINF

echo 'Validating VISITALL...'
./validator.bin experiments/synthesis/visitall_BFS_7_landmarks.prog domain/validation/visitall/ NOINF



echo 'Validating PGP(f_LM,f_1)'
echo 'Validating BAKING...'
./validator.bin experiments/synthesis/baking_PGP_13_landmarks_h1.prog domain/validation/baking/ NOINF

echo 'Validating CORRIDOR-STRIPS...'
./validator.bin experiments/synthesis/corridor-strips_PGP_11_landmarks_h1.prog domain/validation/corridor-strips/ NOINF

echo 'Validating GRIPPER...'
./validator.bin experiments/synthesis/gripper_PGP_8_landmarks_h1.prog domain/validation/gripper/ NOINF

echo 'Validating INTRUSION...'
./validator.bin experiments/synthesis/intrusion_PGP_9_landmarks_h1.prog domain/validation/intrusion/ NOINF

echo 'Validating LOCK...'
./validator.bin experiments/synthesis/lock_PGP_12_landmarks_h1.prog domain/validation/lock/ NOINF

echo 'Validating ONTABLE...'
./validator.bin experiments/synthesis/ontable_PGP_11_landmarks_h1.prog domain/validation/ontable/ NOINF

echo 'Validating SPANNER...'
./validator.bin experiments/synthesis/spanner_PGP_12_landmarks_h1.prog domain/validation/spanner/ NOINF

echo 'Validating VISITALL...'
./validator.bin experiments/synthesis/visitall_PGP_7_landmarks_h1.prog domain/validation/visitall/ NOINF


echo 'Validating BFS(f_GC,f_1)'
#echo 'Validating BAKING...'  # Time-exceeded
#./validator.bin experiments/synthesis/baking_BFS_13_h5_h1.prog domain/validation/baking/ NOINF

echo 'Validating CORRIDOR-STRIPS...'
./validator.bin experiments/synthesis/corridor-strips_BFS_11_h5_h1.prog domain/validation/corridor-strips/ NOINF

echo 'Validating GRIPPER...'
./validator.bin experiments/synthesis/gripper_BFS_8_h5_h1.prog domain/validation/gripper/ NOINF

echo 'Validating INTRUSION...'
./validator.bin experiments/synthesis/intrusion_BFS_9_h5_h1.prog domain/validation/intrusion/ NOINF

#echo 'Validating LOCK...'  # Time-exceeded
#./validator.bin experiments/synthesis/lock_BFS_12_h5_h1.prog domain/validation/lock/ NOINF

#echo 'Validating ONTABLE...'  # Time-exceeded
#./validator.bin experiments/synthesis/ontable_BFS_11_h5_h1.prog domain/validation/ontable/ NOINF

#echo 'Validating SPANNER...'  # Time-exceeded
#./validator.bin experiments/synthesis/spanner_BFS_12_h5_h1.prog domain/validation/spanner/ NOINF

echo 'Validating VISITALL...'
./validator.bin experiments/synthesis/visitall_BFS_7_h5_h1.prog domain/validation/visitall/ NOINF


