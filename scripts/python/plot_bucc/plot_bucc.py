#!/bin/python3
import os, sys

# Load the JSON parser module
from modules import plot_parameters
from modules import plot_residues

if __name__ == '__main__':

    # Select the log path to use
    protein, sim_name = sys.argv[1:]

    # Generate the path to the log file
    path_to_log = './out/log/{}/{}'.format(protein, sim_name)

    # Plot the parameters form the log file
    plot_parameters(path_to_log)

    # Plot the residues form the log file
    plot_residues(path_to_log)
