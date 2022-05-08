# Progressive Generalized Planning
This code computes planning programs that progressively solve planning instances, which builds on top of the [Best First Generalized Planning](https://github.com/rleap-project/best-first-generalized-planning) engine.
It starts with a short tutorial, and later explains every step in more detail.

## Short Tutorial

1. Generate folders and compile the project.
```shell
./scripts/compile_all.sh
```
2. Generate synthesis and validation instances of Visitall domain.
```shell
./domain/generators/visitall.py 2 11 domain/synthesis/visitall/
./domain/generators/visitall.py 12 61 domain/validation/visitall/
```
3. Search a program that solves Visitall
```shell
./main.bin PGP 7 domain/synthesis/visitall/
```
The resulting program should looks like:
```shell
0. visit(i,j)
1. inc(i)
2. goto(0,!(( zf = 1 )))
3. clear(i)
4. inc(j)
5. goto(0,!(( zf = 1 )))
6. end
```
4. Validate the output program, with and without infinite detection.
```shell
./validator.bin experiments/synthesis/visitall_PGP_7_landmarks.prog domain/validation/visitall/ INF
./validator.bin experiments/synthesis/visitall_PGP_7_landmarks.prog domain/validation/visitall/ NOINF
```
Both validations should finish with an `[INFO] GOAL ACHIEVED!`
Great! You have computed your first planning program :-).

## Long Tutorial
### Required folders and project compilation
The next command is used to generate required folders and compile the whole project:

```shell
./scripts/compile_all.sh
```
The compilation will generate a ```main.bin``` and ```validator.bin``` executables file in the root folder.

### Generating random inputs

The inputs are randomly generated from a high-level specification, and some require to install the Tarski module, i.e. ```pip install tarski```.
The next are the examples used in the project to generate the original domains and instances:

```shell
./domain/generators/baking.py 1 10 0 domain/synthesis/baking/
./domain/generators/ontable.py 10 15 1 domain/synthesis/ontable/
./domain/generators/corridor-strips.py 5 14 domain/synthesis/corridor-strips/
./domain/generators/gripper.py 2 11 domain/synthesis/gripper/
./domain/generators/intrusion.py 1 10 0 domain/synthesis/intrusion/
./domain/generators/lock.py 5 14 0 domain/synthesis/lock/
./domain/generators/spanner.py 1 10 0 domain/synthesis/spanner/
./domain/generators/visitall.py 2 11 domain/synthesis/visitall/
```

### Searching a planning program
Here we show few examples which loads the landmarks heuristic by default:

```shell
./main.bin PGP 7 domain/synthesis/visitall/ 
./main.bin PGP 8 domain/synthesis/gripper/ 
./main.bin PGP 9 domain/synthesis/intrusion/ 
./main.bin PGP 11 domain/synthesis/corridor-strips/ 
./main.bin PGP 13 domain/synthesis/baking/ 
./main.bin PGP 12 domain/synthesis/lock/ 
./main.bin PGP 12 domain/synthesis/spanner/ 
./main.bin PGP 11 domain/synthesis/ontable/ 
```

### Validating a program

Generating the original domains and validation instances:
```shell
./domain/generators/baking.py 11 60 0 domain/validation/baking/
./domain/generators/ontable.py 16 65 1 domain/validation/ontable/
./domain/generators/corridor-strips.py 12 61 domain/validation/corridor-strips/
./domain/generators/gripper.py 12 61 domain/validation/gripper/
./domain/generators/intrusion.py 11 60 0 domain/validation/intrusion/
./domain/generators/lock.py 12 61 0 domain/validation/lock/
./domain/generators/spanner.py 12 61 0 domain/validation/spanner/
./domain/generators/visitall.py 12 61 domain/validation/visitall/
```

Validating a specific program with infinite program detection by default:
```shell
./validator.bin experiments/synthesis/visitall_PGP_7_landmarks.prog domain/validation/visitall/
```

Explicit validation with/out infinite program detection (after computing Visitall with landmarks heuristic):
```shell
./validator.bin experiments/synthesis/visitall_PGP_7_landmarks.prog domain/validation/visitall/ INF
./validator.bin experiments/synthesis/visitall_PGP_7_landmarks.prog domain/validation/visitall/ NOINF
```

## Reproducing SoCS 2022 Experiments

> This is a remainder that some experiments, like validation with infinite detection, may exhaust your computer resources.

In order to reproduce SoCS 2022 experiments simply run:
```shell
./scripts/socs22_experiments.sh
```

This will generate the results for PGP using the goal counting heuristic (a.k.a. h5) and the landmarks heuristic in the ```./experiments/synthesis/``` folder.
Every experiment has a ".out" file, if it finds a solution also generates a ".prog" file with the planning program, and landmarks heuristic also generates a ".lg" file for each input planning instance with its landmark graph.
In case the user wants to compute BFS algorithm instead of PGP, set 
the first synthesis argument to BFS, i.e. ```./main.bin BFS 7 domain/synthesis/visitall/```.

This script consists of the following commands which should succeed (given the paper settings):
```shell
./scripts/compile_all.sh
./scripts/synthesis_generators.sh
./scripts/synthesis_experiments.sh
./scripts/synthesis_combined_pgp_landmarks_h1.sh
./scripts/synthesis_combined_bfs_h5_h1.sh
./scripts/validation_generators.sh
./scripts/validation_experiments.sh
```
