#!/bin/bash

# -- Export the location of the CCP4 software
export CCP4PATH=~/PlacementSTFC/software/ccp4-7.1

# -- Source some needed scripts
source ./scripts/shell/checks.sh

# -- Name of the protein to process
PROTEIN="rnase"; PROT_PATH="data/${PROTEIN}";

# -- Check if data exists and check if protein exists
assert_dir_exists "data" 
assert_dir_exists "${PROT_PATH}"

# -- Create a file from the .mtz file
echo " ** [Creating .map file from .mtz (${PROT_PATH}/refmac.mtz)]"
./scripts/shell/make_map.sh refmac.mtz ${PROT_PATH}
