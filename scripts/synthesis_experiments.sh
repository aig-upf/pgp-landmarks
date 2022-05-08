# PGP with landmarks
./scripts/time_memory.sh ./main.bin PGP 7 domain/synthesis/visitall/ landmarks
./scripts/time_memory.sh ./main.bin PGP 8 domain/synthesis/gripper/ landmarks
./scripts/time_memory.sh ./main.bin PGP 9 domain/synthesis/intrusion/ landmarks
./scripts/time_memory.sh ./main.bin PGP 11 domain/synthesis/corridor-strips/ landmarks
./scripts/time_memory.sh ./main.bin PGP 13 domain/synthesis/baking/ landmarks
./scripts/time_memory.sh ./main.bin PGP 12 domain/synthesis/lock/ landmarks
./scripts/time_memory.sh ./main.bin PGP 12 domain/synthesis/spanner/ landmarks
./scripts/time_memory.sh ./main.bin PGP 11 domain/synthesis/ontable/ landmarks

# PGP with euclidean distance (goal counting in propositional domains)
./scripts/time_memory.sh ./main.bin PGP 7 domain/synthesis/visitall/ h5
./scripts/time_memory.sh ./main.bin PGP 8 domain/synthesis/gripper/ h5
./scripts/time_memory.sh ./main.bin PGP 9 domain/synthesis/intrusion/ h5
./scripts/time_memory.sh ./main.bin PGP 11 domain/synthesis/corridor-strips/ h5
#./scripts/time_memory.sh ./main.bin PGP 13 domain/synthesis/baking/ h5  # Memory-exceeded
#./scripts/time_memory.sh ./main.bin PGP 12 domain/synthesis/lock/ h5  # Memory-exceeded
#./scripts/time_memory.sh ./main.bin PGP 12 domain/synthesis/spanner/ h5  # Time-exceeded
./scripts/time_memory.sh ./main.bin PGP 11 domain/synthesis/ontable/ h5

# BFS with landmarks
./scripts/time_memory.sh ./main.bin BFS 7 domain/synthesis/visitall/ landmarks
./scripts/time_memory.sh ./main.bin BFS 8 domain/synthesis/gripper/ landmarks
./scripts/time_memory.sh ./main.bin BFS 9 domain/synthesis/intrusion/ landmarks
./scripts/time_memory.sh ./main.bin BFS 11 domain/synthesis/corridor-strips/ landmarks
#./scripts/time_memory.sh ./main.bin BFS 13 domain/synthesis/baking/ landmarks  # Time-exceeded
#./scripts/time_memory.sh ./main.bin BFS 12 domain/synthesis/lock/ landmarks  # Time-exceeded
#./scripts/time_memory.sh ./main.bin BFS 12 domain/synthesis/spanner/ landmarks  # Time-exceeded
#./scripts/time_memory.sh ./main.bin BFS 11 domain/synthesis/ontable/ landmarks  # Time-exceeded

# BFS with euclidean distance (goal counting in propositional domains)
./scripts/time_memory.sh ./main.bin BFS 7 domain/synthesis/visitall/ h5
./scripts/time_memory.sh ./main.bin BFS 8 domain/synthesis/gripper/ h5
./scripts/time_memory.sh ./main.bin BFS 9 domain/synthesis/intrusion/ h5
./scripts/time_memory.sh ./main.bin BFS 11 domain/synthesis/corridor-strips/ h5
#./scripts/time_memory.sh ./main.bin BFS 13 domain/synthesis/baking/ h5  # Time-exceeded
#./scripts/time_memory.sh ./main.bin BFS 12 domain/synthesis/lock/ h5  # Time-exceeded
#./scripts/time_memory.sh ./main.bin BFS 12 domain/synthesis/spanner/ h5  # Time-exceeded
#./scripts/time_memory.sh ./main.bin BFS 11 domain/synthesis/ontable/ h5  # Time-exceeded


