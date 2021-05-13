#!/bin/python3
import matplotlib
matplotlib.use('Agg')

import os, sys, regex
import matplotlib.pyplot as plt

# User defined modules
from modules import Histomap

# Simulation name string format
SIMULATION_FORMAT = 's(\d+.\d+)_h(\d+.\d+)_r(\d+.\d+)'

if __name__ == '__main__':

    # Name of the current simulation
    protein, sim_name = sys.argv[1:]

    # Path to log output and data
    path_to_data = f'./out/data/{protein}/{sim_name}'
    path_to_log  = f'./out/log/{protein}/{sim_name}'

    # Assert the existence of the paths
    assert os.path.exists(path_to_data)
    assert os.path.exists(path_to_log)

    # Get the path to all needed maps
    path_to_maps = [
        os.path.join(path_to_data, 'refmap/files/refmac.map'),
        os.path.join(path_to_data, 'noisy/files/noisy.map'),
        os.path.join(path_to_data, 'denoised/files/denoised.map')
    ]

    # Assert the existence of the wanted files
    assert all(os.path.exists(p) for p in path_to_maps)

    # Figure that will contain all the plots
    fig = plt.figure(figsize = (14, 10))

    # Match the simulation data from the simulation name
    matches = regex.match(SIMULATION_FORMAT, sim_name)
    
    # Add the simulation name as supertitle
    fig.suptitle(
        's = {}, h = {}, r = {} (A)'.format(
            matches.group(1), matches.group(2), matches.group(3)
        ), y = 0.95
    )

    # Iterate over all possible map files
    for m, map_path in enumerate(path_to_maps, 1):
    
        # Generate a Histomap object with the map file
        histomap = Histomap(map_path)
    
        # Add an axis to the figure
        axis = fig.add_subplot(1, 3, m)
    
        # Add some properties to the axis
        axis.set_xlabel('Map value')
        axis.set_ylabel('Density')
        axis.grid(True, color = '#cccccc')
        axis.set_facecolor('#f9f9fe')
    
        # Set the title of the axis
        axis.set_title(os.path.basename(histomap.name).replace('.map', ''))
    
        # Histogram the data into the axis
        histomap.histogram(
            axis, 100, color = '#407294', alpha = 0.8
        )

    # Increase distance between subplots
    fig.subplots_adjust(
        left = 0.1, right = 0.9,
        wspace = 0.25, hspace = 0.2,
    )

    # Save the plot into the correct file
    fig.savefig(os.path.join(path_to_log, 'histogram'))
