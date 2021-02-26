#!/bin/bash
# -- Create a .map file from a .mtz file

# -- Source the needed setup
source ${CCP4PATH}/bin/ccp4.setup-sh

# -- Source some checks functions
source ./scripts/shell/checks.sh

# -- Retrieve some command line information
PROT_PATH=${1}; PROTEIN=$(basename ${PROT_PATH})

# -- Assert the file extension is correct
assert_dir_exists ${PROT_PATH}
assert_dir_exists ${PROT_PATH}/maps

# -- Name of the mtz file and corresponding map file
MTZ_FILE="refmac.mtz"; MAP_FILE="refmac.map"

# -- Path of input file and output files
INP_PATH=${PROT_PATH}/${MTZ_FILE}
OUT_PATH=${PROT_PATH}/maps/${MTZ_FILE}

# -- Fourier transform .mtz to obtain the .map file
cfft -mtzin ${INP_PATH} -mapout ${OUT_PATH} -colin-fc "/*/*/[FWT,PHWT]" > /dev/null
