#!/bin/bash

#PBS -l nodes=1:ppn=16
#PBS -l pmem=4gb
#PBS -l walltime=48:00:00
#PBS -q batch
#PBS -j oe

../../software/spaced-prdna/spaced -o primates.spaced.out -k 9 -l 21 -d EU ../../data/primates/primates.full.fas  > primates.spaced.out.log
