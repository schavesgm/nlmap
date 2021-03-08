#!/bin/python3

import sys, os
from modules import pipeline_JSON

if __name__ == '__main__':

    # Obtain the protein name
    path_to_out_protein = sys.argv[1]

    # Obtain the name of the simulation
    simulation_name = sys.argv[2]

    # Create the log file 
    path_to_log = os.path.join(
        os.path.dirname(path_to_out_protein), 'log',
        os.path.basename(path_to_out_protein), simulation_name
    )

    # Create the folder to the log file if it does not exist
    if not os.path.exists(path_to_log): os.makedirs(path_to_log)

    # Generate the json log from the protein provided
    pipeline_JSON(path_to_out_protein, simulation_name, path_to_log)
