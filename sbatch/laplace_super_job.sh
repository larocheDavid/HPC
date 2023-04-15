#!/bin/sh

module purge
module load foss

tasks=(1 2 4 8 16 32)
for c in "${tasks[@]}"; do
    sbatch --ntasks=$c laplaceN_job.sh
    sbatch --ntasks=$c laplaceM_job.sh
    sbatch --ntasks=$c laplaceT_job.sh
done
