#!/bin/bash
# -- Build a model from a .mtz file

# -- Source the configuration of CCP4
source ${CCP4_PATH}/bin/ccp4.setup-sh

# -- Source some check functions
source ${BASE_PATH}/scripts/shell/checks.sh

# -- Retrieve some command line variables
PROTEIN=${1}; SIM_NAME=${2}; RUN_PATH=$(pwd);

# -- Generate some variables
OUT_PATH=${RUN_PATH}/out/data/${PROTEIN}/${SIM_NAME}/refmtz

# -- Create the output directory if not present
mkdir -p ${OUT_PATH} ${OUT_PATH}/files ${OUT_PATH}/log

# -- Path to the refmtz files and the log results
FILES_PATH=${OUT_PATH}/files; LOG_PATH=${OUT_PATH}/log

# -- Check if some files exist
assert_file_exists ${FILES_PATH}/dataset.mtz
assert_file_exists ${FILES_PATH}/refmac.mtz
assert_file_exists ${FILES_PATH}/refmac.pdb
assert_file_exists ${FILES_PATH}/sequence.seq

# -- Names of the files used to output the data
CAD_LOG=${LOG_PATH}/cad_input.log
BUC_LOG=${LOG_PATH}/buccaneer.log
 
# -- Move to directory and save new directory
cd ${FILES_PATH}; END_PATH=$(pwd)

# -- Generate the output for buccaneer
cad HKLIN1 dataset.mtz HKLIN2 refmac.mtz HKLOUT buccaneer-input.mtz > ${CAD_LOG} <<eof
LABIN  FILE 1 E1=F E2=SIGF E3=FreeR_flag
LABIN  FILE 2 E1=PHIC_ALL_LS E2=FOM
eof

# -- Build the model
ccp4-python -u ${CCP4_PATH}/bin/buccaneer_pipeline -stdin > ${BUC_LOG} <<eof
title Reference build
pdbin-ref ${CCP4_PATH}/lib/data/reference_structures/reference-1tqw.pdb
mtzin-ref ${CCP4_PATH}/lib/data/reference_structures/reference-1tqw.mtz
colin-ref-fo [/*/*/FP.F_sigF.F,/*/*/FP.F_sigF.sigF]
colin-ref-hl [/*/*/FC.ABCD.A,/*/*/FC.ABCD.B,/*/*/FC.ABCD.C,/*/*/FC.ABCD.D]
seqin sequence.seq
mtzin buccaneer-input.mtz
colin-fo [/*/*/F,/*/*/SIGF]
colin-free [/*/*/FreeR_flag]
colin-phifom [/*/*/PHIC_ALL_LS,/*/*/FOM]
pdbout buccaneer.pdb
cycles 10
buccaneer-anisotropy-correction
buccaneer-fast
buccaneer-1st-cycles 3
buccaneer-1st-sequence-reliability 0.95
buccaneer-nth-cycles 7
buccaneer-nth-sequence-reliability 0.95
buccaneer-nth-correlation-mode
buccaneer-resolution 2
buccaneer-new-residue-name UNK
buccaneer-keyword mr-model-filter-sigma 3
jobs 2
pdbin-mr refmac.pdb
prefix ./buccaneer_pipeline/
eof
