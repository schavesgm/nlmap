#!/bin/bash
# -- Create a .map file from a .mtz file

# -- Source the needed setup
source ${CCP4PATH}/bin/ccp4.setup-sh

# -- Source some checks functions
source ./scripts/shell/checks.sh

# -- Retrieve some command line information
PROT_PATH=${1}

# -- Assert the file extension is correct
assert_dir_exists ${PROT_PATH}

# -- Name of the mtz file and corresponding map file
MTZ_FILE="refmac.mtz"; MAP_FILE="refmac.map"

# -- Fourier transform .mtz to obtain the .map file
if [ -z "${3}" ]; then
    res=$(cfft -mtzin ${PROT_PATH}/${MTZ_FILE} -mapout ${PROT_PATH}/${MAP_FILE} -colin-fc "/*/*/[FWT,PHWT]")
else 
    cfft -mtzin ${PROT_PATH}/${MTZ_FILE} -mapout ${PROT_PATH}/${MAP_FILE} -colin-fc "/*/*/[FWT,PHWT]"
fi
