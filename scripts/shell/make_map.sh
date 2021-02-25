#!/bin/bash
# -- Create a .map file from a .mtz file

# -- Source the needed setup
source ${CCP4PATH}/bin/ccp4.setup-sh

# -- Source some checks functions
source ./scripts/shell/checks.sh

# -- Assert the file extension is correct
assert_file_extension ${1} "mtz"

# -- Important variables needed
MTZ_FILE=${1}; MAP_FILE=${1/.mtz/.map}; PROT_PATH=${2} 

# -- Fourier transform .mtz to obtain the .map file
if [ -z "${3}" ]; then
    res=$(cfft -mtzin ${PROT_PATH}/${MTZ_FILE} -mapout ${PROT_PATH}/${MAP_FILE} -colin-fc "/*/*/[FWT,PHWT]")
else 
    cfft -mtzin ${PROT_PATH}/${MTZ_FILE} -mapout ${PROT_PATH}/${MAP_FILE} -colin-fc "/*/*/[FWT,PHWT]"
fi
