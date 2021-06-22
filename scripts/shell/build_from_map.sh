#!/bin/bash
# -- Build a model from a .map file

# -- Source the configuration of CCP4
source ${CCP4_PATH}/bin/ccp4.setup-sh

# -- Source some check functions
source ${BASE_PATH}/scripts/shell/checks.sh

# -- Retrieve some command line variables
MAP_PATH=${1}; MAP_NAME=${2}; LOG_PATH=${1/files/log}; RUN_PATH=$(pwd)

# -- Create the log output directory
mkdir -p ${RUN_PATH}/${LOG_PATH}

# -- Check if directories exist
assert_dir_exists ${MAP_PATH}
assert_dir_exists ${LOG_PATH}

# -- Log of the cinvfft files
FFT_LOG=${RUN_PATH}/${LOG_PATH}/invfft_${MAP_NAME/.map/}.log
SIG_LOG=${RUN_PATH}/${LOG_PATH}/sigma.log
BUC_LOG=${RUN_PATH}/${LOG_PATH}/buccaneer.log

# -- Move to directory and save new directory
cd ${MAP_PATH}; END_PATH=$(pwd)  

# -- Create an .mtz file from the .map file
cinvfft -mapin ${MAP_NAME} -mtzin dataset.mtz -mtzout __sf.mtz -colout "[FC,PHIC]" > ${FFT_LOG}

# -- Prepare input .mtz file for bucaneer
sigmaa HKLIN __sf.mtz HKLOUT buccaneer-input.mtz  >> ${SIG_LOG} <<eof
PARTIAL
LABIN FP=F SIGFP=SIGF FC=FC PHIC=PHIC
END
eof

# -- Get some information to populate E3
mtzdump HKLIN dataset.mtz > mtzdump.log << eof
END
eof

# -- Get some information from the newly created dumplog
FREE_FLAG=$(grep -Poi 'SIGF \K[\w_]+' mtzdump.log)

# -- Start buccaneer to process the data
ccp4-python -u ${CCP4_PATH}/bin/buccaneer_pipeline -stdin > ${BUC_LOG} <<eof
title ${MAP_NAME} build
pdbin-ref ${CCP4_PATH}/lib/data/reference_structures/reference-1tqw.pdb
mtzin-ref ${CCP4_PATH}/lib/data/reference_structures/reference-1tqw.mtz
colin-ref-fo [/*/*/FP.F_sigF.F,/*/*/FP.F_sigF.sigF]
colin-ref-hl [/*/*/FC.ABCD.A,/*/*/FC.ABCD.B,/*/*/FC.ABCD.C,/*/*/FC.ABCD.D]
seqin sequence.seq
mtzin buccaneer-input.mtz
colin-fo [/*/*/F,/*/*/SIGF]
colin-free [/*/*/${FREE_FLAG}]
colin-phifom [/*/*/PHIC,/*/*/WCMB]
pdbout buccaneer.pdb
cycles 10
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
prefix ./buccaneer_pipeline/
eof
