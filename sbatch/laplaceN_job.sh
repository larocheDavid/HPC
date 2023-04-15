#!/bin/bash

#SBATCH --cpus-per-task=1
#SBATCH --array=1-5
#SBATCH -p shared-cpu
#SBATCH -t 00:60:00

srun ./laplace.out 10000 1000 10000
