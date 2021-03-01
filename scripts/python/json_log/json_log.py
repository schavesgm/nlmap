#!/bin/python3

import sys, os
from modules import pipeline_JSON

if __name__ == '__main__':

    # Obtain the path to the protein
    path_to_protein = sys.argv[1]

    print(path_to_protein)

    # Assert the existence of the protein
    assert os.path.exists(path_to_protein)

    # Print out some information
    print('Generating {} protein'.format(path_to_protein))

    # Generate the json log from the protein provided
    pipeline_JSON(path_to_protein, './log')
