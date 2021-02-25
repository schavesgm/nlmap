#!/bin/bash

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
mkdir -p out/${PROTEIN}/refmtz

# -- Create a file from the .mtz file
echo " ** [Creating .map file from .mtz (${PROT_PATH}/refmac.mtz)]"
./scripts/shell/make_map.sh ${PROT_PATH}

# -- Build the reference model from the .mtz file
echo " ** [Building reference model from reference refmac.mtz]"
./scripts/shell/build_from_mtz.sh ${PROT_PATH} out/${PROTEIN}/refmtz
