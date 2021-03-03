#!/bin/python3

import sys, os
from modules import pipeline_JSON

if __name__ == '__main__':

    # Obtain the path to the protein
    path_to_protein = sys.argv[1]

    # Obtain the name of the simulation
    simulation_name = sys.argv[2]

    # Generate the path to the log
    path_to_log = os.path.join(
        './log', os.path.join(
            os.path.basename(path_to_protein), 
            simulation_name
        )
    )

    # Create the folder to the log file if it does not exist
    os.makedirs(path_to_log) if not os.path.exists(path_to_log) else False

    # Assert the existence of the protein path and the sim path
    assert os.path.exists(path_to_protein)

    # Print out some information
    print('   -- Using {}, {}: Saving into {}'.format(
        path_to_protein, simulation_name, path_to_log)
    )

    # Generate the json log from the protein provided
    pipeline_JSON(path_to_protein, simulation_name, path_to_log)
