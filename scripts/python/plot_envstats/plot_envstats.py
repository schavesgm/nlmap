#!/bin/python3
import matplotlib
matplotlib.use('Agg')

import numpy as np
import matplotlib.pyplot as plt
import regex, os, sys

# Simulation regex
SIMULATION_FORMAT = 's(\d+.\d+)_h(\d+.\d+)_r(\d+.\d+)_p(\d+.\d+)_e(\d+.\d+)'

def eliminate_mirror_axis(axis):
    ''' Eliminate mirror axis from an axis plot. '''

    from matplotlib.lines import Line2D

    # Eliminate the frame
    axis.set_frame_on(False)

    # Disable the drawing of ticks in the top of the plot
    axis.get_xaxis().tick_bottom()

    # Turn off the axis
    axis.axes.get_yaxis().set_visible(True)
    axis.axes.get_xaxis().set_visible(True)

    # Get the interval defining the axis
    xmin, xmax = axis.get_xaxis().get_view_interval()
    ymin, ymax = axis.get_yaxis().get_view_interval()

    # Add the lines for the axis
    axis.add_artist(Line2D((xmin, xmax), (ymin, ymin), color='black', linewidth=2))
    axis.add_artist(Line2D((xmin, xmin), (ymin, ymax), color='black', linewidth=1))

class AvgTable:

    def __init__(self, path_to_file):

        # Save the path into memory
        self.path = path_to_file

        # Get the type of file depending on content
        self.name = 'noisy' if 'noisy' in self.path else 'denoised'

        # Load the data from the file
        data = np.loadtxt(path_to_file)

        # Get some important properties of the files
        self.min_map = data[0,1]
        self.max_map = data[1,1]
        self.avg_map = data[2,1]
        
        # Get the averages from the files
        self.avg_table = np.sort(data[3:,1])[::-1]

    def plot_avg_env(self, axis):
        ''' Plot the data into two axis. '''

        # Array of ones to plot horizontal lines
        ones = np.ones(self.avg_table.shape[0])

        # In the first plot, get a line plot
        axis[0].plot(self.avg_table, color = '#003049', label = 'Environment avg')
        axis[0].plot(ones * self.min_map, color = '#bc4749', label = 'Min/Max map value')
        axis[0].plot(ones * self.max_map, color = '#bc4749')
        axis[0].plot(ones * self.avg_map, color = '#fb8500', label = 'Avg map value')

        # In the second plot, plot the histogram
        axis[1].hist(self.avg_table, bins = 100, color = '#003049')
        axis[1].axvline(self.min_map, color = '#bc4749')
        axis[1].axvline(self.max_map, color = '#bc4749')
        axis[1].axvline(self.avg_map, color = '#fb8500')

        # Set some properties in the axis
        axis[0].set_xlabel('Index');     axis[0].set_ylabel('Map value')
        axis[1].set_xlabel('Map value'); axis[0].set_ylabel('Counts')
        
        # Add some text in the data
        axis[0].text(
            0.5, 0.90, self.name, horizontalalignment = 'center', 
            verticalalignment = 'center', transform = axis[0].transAxes
        )
        axis[1].text(
            0.5, 0.90, self.name, horizontalalignment = 'center', 
            verticalalignment = 'center', transform = axis[1].transAxes
        )

        # Place a grid on the data and eliminate axis
        [a.grid(True, color = '#eeeeee') for a in axis]
        [eliminate_mirror_axis(a) for a in axis]

if __name__ == '__main__':

    # Get the path to the files and the simulation name
    protein, sim_name = sys.argv[1:]

    # Get the path to the data and log
    path_to_data = './out/data/{}/{}'.format(protein, sim_name)
    path_to_log  = './out/log/{}/{}'.format(protein, sim_name)

    # Get some properties from the path
    matches = regex.match(SIMULATION_FORMAT, sim_name)

    # Get the noisy and denoised statistics
    path_to_files = [
        os.path.join(path_to_data, 'noisy/log/envstats.dat'),
        os.path.join(path_to_data, 'denoised/log/envstats.dat')
    ]

    # Assert the existence of the files
    assert all(os.path.exists(f) for f in path_to_files)

    # Generate a figure to plot the data
    fig = plt.figure(figsize = (14, 10))

    # Add 4 axis to the figure
    axis = [
        [fig.add_subplot(2, 2, 1), fig.add_subplot(2, 2, 2)],
        [fig.add_subplot(2, 2, 3), fig.add_subplot(2, 2, 4)]
    ]
    
    for i, path_to_envstat in enumerate(path_to_files):

        # Generate an object to deal with the table data
        envstat = AvgTable(path_to_envstat)

        # Plot the data for each table
        envstat.plot_avg_env(axis[i])

    # Get the correct handles for the labels
    handles, labels = axis[0][0].get_legend_handles_labels()

    # Append the legend to the figure
    fig.legend(
        handles, labels, loc='upper center', 
        ncol = 3, frameon = False,
        bbox_to_anchor = (0, 0.95, 1, 0)
    )

    fig.subplots_adjust(left = 0.1, right = 0.9, wspace = 0.25, hspace = 0.2)

    # Add the title to the data
    fig.suptitle(
        's = {}, h = {}, p = {},  r = {} (A), e = {}'.format(
            matches.group(1), matches.group(2),
            matches.group(3), matches.group(4),
            matches.group(5)
        )
    )

    plt.close()
    fig.savefig(os.path.join(path_to_log, 'envavg'))
