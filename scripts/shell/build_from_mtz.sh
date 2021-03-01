#!/bin/bash
# -- Build a model from a .mtz file

# -- Source the configuration of CCP4
source ${CCP4PATH}/bin/ccp4.setup-sh

# -- Source some check functions
source ./scripts/shell/checks.sh

# -- Retrieve some command line variables
PROT_PATH=${1}; BASE_PATH=$(pwd) 

# -- Generate some variables
PROTEIN=$(basename ${PROT_PATH}); 
OUT_PATH=${BASE_PATH}/out/${PROTEIN}/refmtz

# -- Check if directories exist
assert_dir_exists ${PROT_PATH}

# -- Create the output directory if not present
mkdir -p ${OUT_PATH} 

# -- Check if some files exist
assert_file_exists ${PROT_PATH}/dataset.mtz
assert_file_exists ${PROT_PATH}/refmac.mtz

# -- Names of the files used to output the data
CAD_LOG=${OUT_PATH}/log/cad_input.log
BUC_LOG=${OUT_PATH}/log/buccaneer.log

# -- Move to directory and save new directory
cd ${OUT_PATH}; END_PATH=$(pwd)

# -- Assert we are in the correct directory
assert_location ${BASE_PATH} out/${PROTEIN}/refmtz ${END_PATH}

# -- Remove all files in the current directory
rm -r ./*

# -- Create the log directory
mkdir -p ${OUT_PATH}/log

# -- Generate the output for buccaneer
cad HKLIN1 ${PROT_PATH}/dataset.mtz HKLIN2 ${PROT_PATH}/refmac.mtz HKLOUT buccaneer-input.mtz > ${CAD_LOG} <<eof
LABIN  FILE 1 E1=F E2=SIGF E3=FreeR_flag
LABIN  FILE 2 E1=PHIC_ALL_LS E2=FOM
eof

# -- Build the model
ccp4-python -u $CCP4/bin/buccaneer_pipeline -stdin > ${BUC_LOG} <<eof
title Reference build
pdbin-ref ${CCP4PATH}/lib/data/reference_structures/reference-1tqw.pdb
mtzin-ref ${CCP4PATH}/lib/data/reference_structures/reference-1tqw.mtz
colin-ref-fo [/*/*/FP.F_sigF.F,/*/*/FP.F_sigF.sigF]
colin-ref-hl [/*/*/FC.ABCD.A,/*/*/FC.ABCD.B,/*/*/FC.ABCD.C,/*/*/FC.ABCD.D]
seqin ${PROT_PATH}/sequence.seq
mtzin buccaneer-input.mtz
colin-fo [/*/*/F,/*/*/SIGF]
colin-free [/*/*/FreeR_flag]
colin-phifom [/*/*/PHIC_ALL_LS,/*/*/FOM]
pdbout buccaneer.pdb
cycles 5
buccaneer-anisotropy-correction
buccaneer-fast
buccaneer-1st-cycles 3
buccaneer-1st-sequence-reliability 0.95
buccaneer-nth-cycles 2
buccaneer-nth-sequence-reliability 0.95
buccaneer-nth-correlation-mode
buccaneer-resolution 2
buccaneer-new-residue-name UNK
buccaneer-keyword mr-model-filter-sigma 3
jobs 2
pdbin-mr ${PROT_PATH}/refmac.pdb
prefix ./buccaneer_pipeline/
eof
