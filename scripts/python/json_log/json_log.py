#!/bin/python3
import sys, os

# Import some user defined modules
from modules import JSON_pipeline

if __name__ == '__main__':

    # Obtain the protein name
    protein, sim_name = sys.argv[1:]

    # Generate the path to the log
    path_to_out_data = './out/data/{}/{}'.format(protein, sim_name)
    path_to_out_log  = './out/log/{}/{}'.format(protein, sim_name)
    
    # Some sanity checks
    assert os.path.exists(path_to_out_data)
    assert os.path.exists(path_to_out_log)

    # Create the folder to the log file if it does not exist
    if not os.path.exists(path_to_out_log): 
        os.makedirs(path_to_out_log)

    # Generate the json log from the protein provided
    JSON_pipeline(path_to_out_data, path_to_out_log)
