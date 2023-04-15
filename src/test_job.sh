#!/bin/bash

#SBATCH -J laplace
#SBATCH -N 1
#SBATCH -c 1
#SBATCH -p public-cpu
#SBATCH -t 00:10:00

module load foss

srun ./laplace.out 100 100 100000


