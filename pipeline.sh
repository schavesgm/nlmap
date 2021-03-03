#!/bin/bash

# ** Usage of each script
# -- make_map.sh path_to_protein_data
# -- build_from_mtz.sh path_to_protein_data (overwrite_flag)
# -- build_from_map.sh map_file path_to_protein_data out_name

# -- Export the location of the CCP4 software
export CCP4PATH=~/PlacementSTFC/software/ccp4-7.1

# -- Source some needed scripts
source ./scripts/shell/checks.sh

# -- Set some needed variables for the simulation
# sigma=$(printf %.4f ${1}); assert_var_exists "noise_std"   ${1}
# hsqrt=$(printf %.4f ${2}); assert_var_exists "h_square"    ${2}
# bvole=$(printf %d ${3});   assert_var_exists "big_vol_e"   ${3}
# svole=$(printf %d ${4});   assert_var_exists "small_vol_e" ${4}

sigma=$(printf %.4f 0.1)
hsqrt=$(printf %.4f 0.05)
bvole=$(printf %d 5)
svole=$(printf %d 2)

# -- Name of the protein to process (THIS HAS TO BE COMMAND LINED)
PROTEIN="rnase"; PROT_PATH="$(pwd)/data/${PROTEIN}";

# -- Specific name of the simulation name
SIM_NAME="s${sigma}_h${hsqrt}_b${bvole}_v${svole}"

# -- Check if data exists and check if protein exists
assert_dir_exists "data" 
assert_dir_exists "${PROT_PATH}"
 
# -- Create some directories if not present
mkdir -p data/${PROTEIN}/maps

# -- Create a .map file from the .mtz file
echo " ** [Creating .map file from .mtz (${PROT_PATH}/refmac.mtz)]"
./scripts/shell/make_map.sh ${PROT_PATH}

# -- Build the reference model from the .mtz file
echo " ** [Building reference model from reference refmac.mtz]"
./scripts/shell/build_from_mtz.sh ${PROT_PATH}

# -- Build the reference model from the .map file
echo " ** [Building reference model from reference refmac.map]"
./scripts/shell/build_from_map.sh refmac.map ${PROT_PATH} refmap
 
# -- Build a model for each processed map file
for file in $(ls ${PROT_PATH}/maps/${SIM_NAME}/*); do

    # Basename of the file
    base=$(basename -- ${file})

    # Echo some data to the console
    echo " ** [Building model from map file ${base}]"

    # Location of the map file inside ${PROT_PATH}
    MAP_LOC=maps/${SIM_NAME}/${base}

    # Output folder inside ./out
    OUT_PATH=${SIM_NAME}/${base/.map/}

    # Process the map with the correct script
    ./scripts/shell/build_from_map.sh ${MAP_LOC} ${PROT_PATH} ${OUT_PATH} "overwrite"

done

# # -- Obtain a log file from the protein data
# echo " ** [Creating a log file from the pipeline processing]"
# ./scripts/python/json_log/json_log.py ./out/${PROTEIN} 
