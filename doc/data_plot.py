import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

with open('data.txt', 'r') as f:
    data = [list(map(float, line.strip().split())) for line in f]

def plot_speedup(data, N, M, T):
	filtered_data = [row for row in data if row[1] == N and row[2] == M and row[3] == T]
	sequ_times = np.array([row for row in filtered_data if row[0] == 1])
	mean_sequ_time = np.mean(sequ_times[:,4])

	nCPUS = [1, 2, 4, 8, 16, 32]
	speedup = []
	for n in nCPUS:
		parallel_time = np.array([row for row in filtered_data if row[0] == n])
	
		mean_parallel_time = np.mean(parallel_time[:,4])
		speedup.append(mean_sequ_time/mean_parallel_time)

	plt.plot(nCPUS, nCPUS, '--', color='grey', label='ideal speedup')
	plt.plot(nCPUS, speedup, 'x-', color='red', label='speedup')

	plt.xlabel('Number of CPUs')
	plt.ylabel('Speedup')
	plt.title("Data (N={}, M={}, T={})".format(N, M, T))

	plt.legend()
	plt.show()


N = 1000
M = 1000
T = 10000

plot_speedup(data, N*10, M, T)
plot_speedup(data, N, M*10, T)
plot_speedup(data, N, M, T*10)
