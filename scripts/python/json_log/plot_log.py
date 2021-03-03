import os, sys

# Load the JSON parser module
from modules import plot_parameters
from modules import plot_residues

if __name__ == '__main__':

    # Select the log path to use
    log_path = sys.argv[1]

    # Plot the parameters form the log file
    plot_parameters(log_path)

    # Plot the residues form the log file
    plot_residues(log_path)
