#!/bin/bash

#PBS -l nodes=1:ppn=16
#PBS -l pmem=4gb
#PBS -l walltime=48:00:00
#PBS -q batch
#PBS -j oe
cd /home/sriram/work/arakawa/runs/primates/

# ../../build/arakawa.x -i ../../data/primates/ -o primates.acs.k0.out -k 0
# ../../build/arakawa.x -i ../../data/primates/ -o primates.acs.k1.out -k 1
# ../../build/arakawa.x -i ../../data/primates/ -o primates.acs.k2.out -k 2
# ../../build/arakawa.x -i ../../data/primates/ -o primates.acs.k3.out -k 3
# ../../build/arakawa.x -i ../../data/primates/ -o primates.acs.k4.out -k 4
# ../../build/arakawa.x -i ../../data/primates/ -o primates.acs.k5.out -k 5

../../build/arakawa.x -f ../../data/primates/primates.full.fas -o primates.acs.k0.out -k 0
../../build/arakawa.x -f ../../data/primates/primates.full.fas -o primates.acs.k1.out -k 1
../../build/arakawa.x -f ../../data/primates/primates.full.fas -o primates.acs.k2.out -k 2
../../build/arakawa.x -f ../../data/primates/primates.full.fas -o primates.acs.k3.out -k 3
../../build/arakawa.x -f ../../data/primates/primates.full.fas -o primates.acs.k4.out -k 4
../../build/arakawa.x -f ../../data/primates/primates.full.fas -o primates.acs.k5.out -k 5

# ./build/arakawa.x -i ./data/primates/ -o primates.acs.nve.k0.out -k 0 -n
# ./build/arakawa.x -i ./data/primates/ -o primates.acs.nve.k1.out -k 1 -n
# ./build/arakawa.x -i ./data/primates/ -o primates.acs.nve.k2.out -k 2 -n
# ./build/arakawa.x -i ./data/primates/ -o primates.acs.nve.k3.out -k 3 -n
# ./build/arakawa.x -i ./data/primates/ -o primates.acs.nve.k4.out -k 4 -n
# ./build/arakawa.x -i ./data/primates/ -o primates.acs.nve.k5.out -k 5 -n

