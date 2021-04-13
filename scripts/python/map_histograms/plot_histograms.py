#!/bin/python3
import os, sys, regex
import matplotlib.pyplot as plt

# User defined modules
from modules import Histomap

# Simulation name string format
SIM_FORMAT = 's(\d+.\d+)_h(\d+.\d+)_r(\d+.\d+)'

if __name__ == '__main__':

    # Name of the current simulation
    sim_name = sys.argv[1]
    
    # Get the path to the protein
    path_to_protein = sys.argv[2]

    # Out folder where the plot will be stored
    out_path = sys.argv[3]

    # Path to maps
    path_to_maps = os.path.join(path_to_protein, 'maps', sim_name)

    # Assert some conditions on the paths
    assert os.path.exists(path_to_protein)
    assert os.path.exists(path_to_maps)
    
    # Get the correct builds
    map_paths = [
        os.path.join(path_to_protein, 'refmac.map'), 
        os.path.join(path_to_maps, 'noisy.map'), 
        os.path.join(path_to_maps, 'denoised.map')
    ]
    
    # Figure that will contain all the plots
    fig = plt.figure(figsize = (14, 10))

    # Match the simulation data from the simulation name
    matches = regex.match(SIM_FORMAT, sim_name)
    
    # Add the simulation name as supertitle
    fig.suptitle(
        's = {}, h = {}, r = {} (A)'.format(
            matches.group(1), matches.group(2), matches.group(3)
        ), y = 0.95
    )

    # Iterate over all possible map files
    for m, map_path in enumerate(map_paths, 1):
    
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
    plt.subplots_adjust(
        left = 0.1, right = 0.9,
        wspace = 0.25, hspace = 0.2,
    )

    plt.savefig(os.path.join(out_path, 'histogram.eps'))
