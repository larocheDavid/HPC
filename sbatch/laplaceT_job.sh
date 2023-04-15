#!/bin/bash

#SBATCH --cpus-per-task=1
#SBATCH --array=1-5
#SBATCH -p shared-cpu
#SBATCH -t 00:60:00

module load foss

srun ./laplace.out 1000 1000 100000
