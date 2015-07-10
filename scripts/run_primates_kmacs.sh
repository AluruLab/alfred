#!/bin/bash

#PBS -l nodes=1:ppn=16
#PBS -l pmem=4gb
#PBS -l walltime=48:00:00
#PBS -q batch
#PBS -j oe
cd /home/sriram/work/arakawa/runs/primates/

/home/sriram/work/arakawa/software/kmacs/kmacs ../../data/primates/primates.full.fas 0 primates.kmacs.k0.out >primates.kmacs.k0.out.log
#/home/sriram/work/arakawa/software/kmacs/kmacs ../../data/primates/primates.full.fas 30 primates.kmacs.k30.out >primates.kmacs.k30.out.log
#/home/sriram/work/arakawa/software/kmacs/kmacs ../../data/primates/primates.full.fas 40 primates.kmacs.k40.out >primates.kmacs.k40.out.log
/home/sriram/work/arakawa/software/kmacs/kmacs ../../data/primates/primates.full.fas 50 primates.kmacs.k50.out >primates.kmacs.k50.out.log
/home/sriram/work/arakawa/software/kmacs/kmacs ../../data/primates/primates.full.fas 60 primates.kmacs.k60.out >primates.kmacs.k60.out.log
/home/sriram/work/arakawa/software/kmacs/kmacs ../../data/primates/primates.full.fas 70 primates.kmacs.k70.out >primates.kmacs.k70.out.log
/home/sriram/work/arakawa/software/kmacs/kmacs ../../data/primates/primates.full.fas 80 primates.kmacs.k80.out >primates.kmacs.k80.out.log
/home/sriram/work/arakawa/software/kmacs/kmacs ../../data/primates/primates.full.fas 90 primates.kmacs.k90.out >primates.kmacs.k90.out.log

