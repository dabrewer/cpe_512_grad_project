import os
import re
import glob
import numpy as np
import matplotlib.pyplot as plt

#PLOT_TITLE = "Mesh Potentials (V): Threads = {}, Size = {} cm"
PLOT_TITLE = "Mesh Potentials (V): Size = {} cm"

def plot(x, y, v, threads, size):
    plt. clf()
    
    #plt.title(PLOT_TITLE.format(threads, size))

    plt.title(PLOT_TITLE.format(size))

    x=np.unique(x)
    y=np.unique(y)
    X,Y = np.meshgrid(x,y)
    V=v.reshape(len(y),len(x))

    mesh = plt.pcolormesh(X,Y,V, cmap='jet')
    plt.colorbar(mesh)

    # [plt.hlines(y=yVal, xmin=0.0, xmax=65.0, linewidth=0.5) for yVal in np.arange(0, 50, float(size))]
    # [plt.vlines(x=xVal, ymin=0.0, ymax=50.0, linewidth=0.5) for xVal in np.arange(0, 65, float(size))]

    plt.savefig("output/heatmaps/mesh_{}_{}.png".format(threads, size))


if not os.path.exists('output/heatmaps'):
    os.makedirs('output/heatmaps')

for fname in glob.glob("output/mesh_*.out"):
    # Extract relevant file info from filename
    results = re.search("output/mesh_(\w*)_(\d*\.\d*).out", fname)
    threads = results.group(1)
    size = results.group(2)
    # Import file contents
    
    data = np.genfromtxt(fname,delimiter='\t')
    x=data[:,0]
    y=data[:,1]
    v=data[:,2]

    # Export the plot to file
    plot(x, y, v, threads, size)