#!/bin/bash

# ** Usage of each script
# -- make_map.sh path_to_protein_data
# -- build_from_mtz.sh path_to_protein_data
# -- build_from_map.sh map_file path_to_protein_data out_name

# -- Export the location of the CCP4 software
export CCP4PATH=~/PlacementSTFC/software/ccp4-7.1

# -- Source some needed scripts
source ./scripts/shell/checks.sh

# -- Name of the protein to process
PROTEIN="rnase"; PROT_PATH="$(pwd)/data/${PROTEIN}";

# -- Check if data exists and check if protein exists
assert_dir_exists "data" 
assert_dir_exists "${PROT_PATH}"

# -- Create some directories if not present
mkdir -p data/${PROTEIN}/maps

# -- Create a file from the .mtz file
echo " ** [Creating .map file from .mtz (${PROT_PATH}/refmac.mtz)]"
./scripts/shell/make_map.sh ${PROT_PATH}

# -- Build the reference model from the .mtz file
echo " ** [Building reference model from reference refmac.mtz]"
./scripts/shell/build_from_mtz.sh ${PROT_PATH}

# -- Build the reference model from the .map file
echo " ** [Building reference model from reference refmac.map]"
./scripts/shell/build_from_map.sh refmac.map ${PROT_PATH} refmap

# -- Obtain a log file from the protein data
echo " ** [Creating a log file from the pipeline processing]"
./scripts/python/json_log/json_log.py ./out/${PROTEIN} 
