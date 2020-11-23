import csv
import numpy as np
import matplotlib.pyplot as plt

################################################################################
# GLOBALS
################################################################################
STAT_FNAME='output/stat.out'
NUM_THREADS = ['S', '1', '2', '4', '8', '16', '32']

TIME_TOTAL  = 0
TIME_POINT  = 1
SPEEDUP     = 2
EFFICIENCY  = 3
VARIANCE    = 4

################################################################################
# CLASS DEFINITIONS
################################################################################
class Stat:
    def __init__(self, threads, points, iterations, time):
        self.threads = threads
        self.points = int(points)
        self.iterations = int(iterations)
        self.time = float(time)
        self.point_time = self.time / (self.iterations * self.points)
    def serial_compare(self, serial_time):
        if serial_time == self.time:
            self.speedup = 1
            self.efficiency = 1
        else:
            self.speedup = serial_time / self.time
            self.efficiency = self.speedup / int(self.threads)
    def __repr__(self):
        return "<Threads:{}\tPoints:{}\tIterations:{}\tTime:{}\tPoint Time:{}\tSpeedup:{}\tEfficiency:{}>".format(self.threads, self.points, self.iterations, self.time, self.point_time, self.speedup, self.efficiency)
    def __eq__(self, other): 
        return self.threads == other.threads and self.points == other.points and self.iterations == other.iterations
    def __hash__(self):
        return hash(self.threads) ^ hash(self.points) ^ hash(self.iterations)

class LinePlot:
    def __init__(self, title, xlabel, ylabel, outfile):
        self.title = title
        self.xlabel = xlabel
        self.ylabel = ylabel
        self.outfile = outfile
    def generate(self, stats, type):
        fig, ax = plt.subplots()
        # Set Plot Attributes
        ax.set(title=self.title)
        ax.set(xlabel=self.xlabel)
        ax.set(ylabel=self.ylabel)
        ax.set(xscale="log")
        ax.grid()
        # Generat Plot Lines
        for threads in NUM_THREADS:
            x = [stat.points for stat in stats if stat.threads == threads]
            if(type == TIME_TOTAL):
                y = [stat.time for stat in stats if stat.threads == threads]
            elif(type == TIME_POINT):
                y = [stat.point_time for stat in stats if stat.threads == threads]
            elif(type == SPEEDUP):
                y = [stat.speedup for stat in stats if stat.threads == threads]
            elif(type == EFFICIENCY):
                y = [stat.efficiency for stat in stats if stat.threads == threads]
            if threads is 'S':
                ax.plot(x, y, '.-', label='Serial Ref')
            else:
                ax.plot(x, y, '.-', label='{} Threads'.format(threads))
        # Generate legend and export
        # ax.legend(loc="upper right")
        ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.125), ncol=3)
        fig.savefig(self.outfile, bbox_inches="tight")

class ScatterPlot:
    def __init__(self, title, xlabel, ylabel, outfile):
        self.title = title
        self.xlabel = xlabel
        self.ylabel = ylabel
        self.outfile = outfile
    def generate(self, stats, type):
        fig, ax = plt.subplots()
        # Set Plot Attributes
        ax.set(title=self.title)
        ax.set(xlabel=self.xlabel)
        ax.set(ylabel=self.ylabel)
        ax.set(xscale="log")
        ax.grid()
        # Generat Plot Lines
        for threads in NUM_THREADS:
            x = [stat.points for stat in stats if stat.threads == threads]
            if(type == TIME_TOTAL):
                y = [stat.time for stat in stats if stat.threads == threads]
            elif(type == TIME_POINT):
                y = [stat.point_time for stat in stats if stat.threads == threads]
            elif(type == VARIANCE):
                y = [stat.variance for stat in stats if stat.threads == threads]
            if threads is 'S':
                ax.scatter(x, y, label='Serial Ref', marker='x', s=50, alpha=0.5)
            else:
                ax.scatter(x, y, label='{} Threads'.format(threads), marker='x', s=50, alpha=0.5)
        # Generate legend and export
        ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.125), ncol=3)
        fig.savefig(self.outfile, bbox_inches="tight")

################################################################################
# UTILITY FUNCTIONS
################################################################################
def import_stats(fname):
    with open(fname) as f:
        reader = csv.reader(f, delimiter=',')
        # Init all stats with basic info from file
        stats = [Stat(row[0], row[1], row[2], row[3]) for row in reader]
        # Sorting is important so we don't need to find min of each case later, we can just make it a set
        return sorted(stats, key=lambda x: x.time)

################################################################################
# PLOT FUNCTIONS
################################################################################
# def plot_time_total(stats):
#     # Set Plot Attributes
#     plt.title('Iteration Execution Time, 2D Electrostatic Solver')
#     plt.xlabel('Number of Mesh Nodes')
#     plt.ylabel('Execution Time in Seconds')
#     # Generat Plot Lines
#     for threads in NUM_THREADS:
#         x = [stat.points for stat in stats if stat.threads == threads]
#         y = [stat.time for stat in stats if stat.threads == threads]
#         if threads is 'S':
#             plt.plot(x, y, label='Serial Ref')
#         else:
#             plt.plot(x, y, label='{} Threads'.format(threads))
#     # Generate legend and export
#     plt.legend()
#     plt.savefig('output/post/total_time.png')
#     plt.show()



# TODO: PLOT TOTAL TIME VS SIZE
# TODO: PLOT ITER TIME VS SIZE
# TODO: PLOT SPEED UP VS SIZE
# TODO: PLOT EFFICIENCY VS SIZE

################################################################################
# MAIN
################################################################################
stats = import_stats(STAT_FNAME)
stats_set = sorted(set(stats), key = lambda x: x.points)

# Compare all runs to serial reference and add to stat objects
for ser in [stat for stat in stats_set if stat.threads == 'S']:
    #ser.serial_compare(ser.time)
    for stat in [stat for stat in stats_set if stat.points == ser.points]:
        stat.serial_compare(ser.time)

# Generate scatter plots with data from all 10 runs
plot_time_total_scat = ScatterPlot('Total Execution Time vs. Number of Mesh Nodes', 'Number of Mesh Nodes', 'Execution Time (s)', 'output/post/time_total_scatter.png')
plot_time_total_scat.generate(stats, TIME_TOTAL)

plot_time_total_scat = ScatterPlot('Average Point Calculation Time vs. Number of Mesh Nodes', 'Number of Mesh Nodes', 'Execution Time (s)', 'output/post/time_point_scatter.png')
plot_time_total_scat.generate(stats, TIME_POINT)

# Generate all line plots of minimum value from 10 runs
plot_time_total = LinePlot('Total Execution Time vs. Number of Mesh Nodes', 'Number of Mesh Nodes', 'Execution Time (s)', 'output/post/time_total.png')
plot_time_total.generate(stats_set, TIME_TOTAL)

plot_time_point = LinePlot('Average Point Calculation Time vs. Number of Mesh Nodes', 'Number of Mesh Nodes', 'Execution Time (s)', 'output/post/time_point.png')
plot_time_point.generate(stats_set, TIME_POINT)

plot_speedup = LinePlot('Execution Speedup vs. Number of Mesh Nodes', 'Number of Mesh Nodes', 'Speedup', 'output/post/speedup.png')
plot_speedup.generate(stats_set, SPEEDUP)

plot_efficiency = LinePlot('Execution Efficiency vs. Number of Mesh Nodes', 'Number of Mesh Nodes', 'Efficiency', 'output/post/efficiency.png')
plot_efficiency.generate(stats_set, EFFICIENCY)

# Generate Variance Plot
for stat in stats_set:
    stat.variance = np.var([s.time for s in stats if s == stat])
plot_time_total_variance = ScatterPlot('Total Execution Time vs. Number of Mesh Nodes', 'Number of Mesh Nodes', 'Execution Time (s)', 'output/post/time_total_variance.png')
plot_time_total_variance.generate(stats_set, VARIANCE)


# min_stats = []
# for stat in set(stats):
#     min_stats.append( min([s for s in stats if s == stat], key = lambda x: x.time) )

# for i in range(0,len(set_stats)):
#     if(min_stats[i].time != set_stats[i].time):
#         print(min_stats[i])
#         print(set_stats[i])
#         print()



