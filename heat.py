import numpy as np
from matplotlib.pyplot import plot, draw, show, imshow
from matplotlib import pyplot as plt

def iter(domain):
	N, M = np.shape(domain)
	south = np.roll(domain, -1, axis=0)
	north = np.roll(domain,  1, axis=0)
	east  = np.roll(domain, -1, axis=1)
	west  = np.roll(domain,  1, axis=1)
	next = 0.25*(south + north + east + west)
	next[0,:] = domain[0,:]
	next[:,0] = domain[:,0]
	max_row = N-1
	max_col = M-1
	next[max_row,:] = domain[max_row,:]
	next[:,max_col] = domain[:,max_col]
	return next
	
def init_1(N, M, rnd_val):
	max_row = N-1
	max_col = M-1
	if rnd_val:
		domain = np.random.rand(N,M)
	else:
		domain = np.zeros((N,M))
	domain[0,:] = 0.0
	domain[:,0] = 1.0
	domain[max_row,:] = 1.0
	domain[:,max_col] = 0.0
	return domain
	
def init_2(N, M, rnd_val):
	max_row = N-1
	max_col = M-1
	if rnd_val:
		domain = np.random.rand(N,M)
	else:
		domain = np.zeros((N,M))
	domain[0,:] = 1.0
	domain[max_row,:] = 1.0
	return domain
	
def init_3(N, M, rnd_val):
	max_row = N-1
	max_col = M-1
	if rnd_val:
		domain = np.random.rand(N,M)
		domain[0,:] = 0.0
		domain[:,0] = 0.0
		domain[max_row,:] = 0.0
		domain[:,max_col] = 0.0
	else:
		domain = np.zeros((N,M))
	domain[0,0:np.int(max_col/2)] = 1.0
	domain[max_row,int(max_col/2):max_col] = 1.0
	return domain


def plot_domain(domain, pause_time):
	plt.imshow(domain, cmap='hot', interpolation='nearest')
	plt.draw()
	plt.pause(pause_time)

def temp_plot(total_ts, save_file = None):
	n = len(total_ts)
	xs = list(range(n))
	fig, ax = plt.subplots()
	ax.plot(xs, total_ts)
	ax.set(xlabel='Iterations', ylabel='Temperature', title='Sum of domain temperatures')
	ax.grid()
	ax.legend(['T'])

	if save_file is not None:
		fig.savefig(save_file)
	plt.show(block=True)

def simulate(N, M, T, rnd_val = False, show_plot = False, type_src = 1):
	total_temp = []
	init_plot_wait = 10
	if type_src == 2:
		domain = init_2(N, M, rnd_val)
	elif type_src == 3:
		domain = init_3(N, M, rnd_val)
	else:
		domain = init_1(N, M, rnd_val)
	pause_time = 0.01
	print("Domain is", N, "x", M)
	if show_plot:
		plt.ion()
		plt.show()
		plot_domain(domain, init_plot_wait)
		
	for t in range(T):
		total_temp.append(np.sum(domain))
		if t % 100 == 0:
			print("Iteration", t, "over", T)
			print("Sum is", total_temp[-1])
			if show_plot:
				plot_domain(domain, pause_time)
		next = iter(domain)
		domain = next
	return (total_temp, domain)
        
if __name__ == '__main__':
	L = 40
	C = 70
	T = 10
	rnd_val = False
	show_plot = True
	type_src = 3
	total_ts, solution = simulate(L, C, T, rnd_val, show_plot, type_src)
	temp_plot(total_ts)
	

