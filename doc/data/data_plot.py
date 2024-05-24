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
	speedup_sd = []
	for n in nCPUS:
		parallel_time = np.array([row for row in filtered_data if row[0] == n])
		speedup_parallel = mean_sequ_time/parallel_time
		speedup.append(np.mean(speedup_parallel[:,4]))
		
		parallel_time_sd = np.std(speedup_parallel[:,4])
		speedup_sd.append(parallel_time_sd)
		
		
	
	
	plt.plot(np.array(filtered_data)[:,0], (mean_sequ_time/np.array(filtered_data)[:,4]), '.', color='red', label='speedup')
	#plt.plot(nCPUS, nCPUS, '--', color='grey', label='ideal speedup')
	plt.plot(nCPUS, speedup, '.-', color='blue', label='speedup')
	plt.errorbar(nCPUS, speedup, yerr=speedup_sd, color='blue', fmt='none', capsize=3, label='error')
	plt.bar(nCPUS, speedup, yerr=speedup_sd, color='green', fmt='none', capsize=3, label='error')
	plt.xticks(nCPUS, nCPUS)
	plt.xlabel('Number of CPUs')
	plt.ylabel('Speedup')
	plt.title("Parameters: N={}, M={}, T={}".format(N, M, T))
	plt.legend()
	#plt.savefig("../figures/speedup_{}_{}_{}.png".format(N, M, T))
	plt.show()

N = 1000
M = 1000
T = 10000

plot_speedup(data, N*10, M, T)
plot_speedup(data, N, M*10, T)
plot_speedup(data, N, M, T*10)
