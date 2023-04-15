import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Read the data from file
with open('data.txt', 'r') as f:
    data = [list(map(float, line.strip().split())) for line in f]

N = 1000
M = 10000
T = 10000

filtered_data = [row for row in data if row[1] == N and row[2] == M and row[3] == T]

sequ_times = [row for row in filtered_data if row[0] == 1]
mean_sequ_time = np.mean(sequ_times)

nCPUS = [1, 2, 4, 8, 16, 32]
speedup = []
ideal_speedup = [] 
for n in nCPUS:
	parallel_time = np.array([row for row in filtered_data if row[0] == n])
	
	print(parallel_time[:,4])
	mean_parallel_time = np.mean(parallel_time[:,4])
	print(mean_parallel_time)
	speedup.append(mean_sequ_time/mean_parallel_time)
	ideal_speedup.append(mean_sequ_time/(mean_parallel_time/n));

#print(speedup);

plt.plot(nCPUS, speedup, 'o-', label='speedup')
plt.plot(nCPUS,ideal_speedup, 'x-', label='ideal speedup')

# add labels and title
plt.xlabel('Number of CPUs')
plt.ylabel('Speedup')
plt.title('Speedup vs. Number of CPUs')

# add legend
plt.legend()

# show the plot
plt.show()
'''
# Convert the filtered data to a pandas DataFrame
df = pd.DataFrame(data, columns=["CPU", "N", "M", "T", "Time"])

# Calculate the mean time for each CPU value
mean_times = df.groupby("CPU").mean()["Time"]
speedup = mean_sequ_time/mean_times.values

print(mean_sequ_time);
for i in mean_times.index:
	print(i);
	
for i in mean_times:
	print(i);
ideal_speedup = mean_sequ_time/mean_times.index
# Print the resulting mean time to CPU number dataset


#print(ideal_speedup)


# plot the speedup and ideal_speedup curves
plt.plot(speedup,mean_times.index, 'o-', label='Speedup')
plt.plot(ideal_speedup, mean_times.index, 'o-', label='Ideal speedup')

# add labels and title
plt.xlabel('Number of CPUs')
plt.ylabel('Speedup')
plt.title('Speedup vs. Number of CPUs')

# add legend
plt.legend()

# show the plot
plt.show()
'''


'''


ideal_speedup.plot(x="CPU", y="Mean Time", color="blue", marker='x')
plt.xlabel("CPU Number")
plt.ylabel("Mean Time (seconds)")
plt.title("Mean Execution Time for Different CPU Numbers")

#x_ideal = [1, 2, 3, 4, 5]
#y_ideal = [2, 4, 6, 8, 10]

plt.show()

'''
'''

# Filter the data based on the values in columns 2-4
filtered_data = [row for row in data if row[0] != 1 and row[1] == N and row[2] == M and row[3] == T]

sequ_times = [row for row in data if row[0] == 1]
mean_sequ_time = np.mean(sequ_times)

# Transpose the data to separate the x and y values
x = [mean_sequ_time/row[4] for row in filtered_data]
y = [row[0] for row in filtered_data]

# Plot the data
#plt.yscale('log', base=2)
plt.yticks([2, 4, 8, 16, 32])

#plt.plot(x, y, 'x')
plt.scatter(x, y)
plt.plot(x, y)
plt.title("N={}, M={}, T={}".format(N, M, T))
plt.xlabel('speedup')
plt.ylabel('Number of processes')
plt.show()
'''
