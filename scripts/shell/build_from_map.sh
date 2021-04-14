#!/bin/bash
# -- Build a model from a .map file

# -- Source the configuration of CCP4
source ${CCP4_PATH}/bin/ccp4.setup-sh

# -- Source some check functions
source ${BASE_PATH}/scripts/shell/checks.sh

# -- Retrieve some command line variables
MAP_FILE=${1}; PROT_PATH=${2}; OUT_FOLDER=${3}; 
OVERWRITE=false; RUN_PATH=$(pwd)

# -- Boolean flag to not overwrite data
[[ ! -z ${4} ]] && OVERWRITE=true

# -- Generate some variables
PROTEIN=$(basename ${PROT_PATH});
OUT_PATH=${RUN_PATH}/out/${PROTEIN}/${OUT_FOLDER}
BASE_MAP=$(basename ${MAP_FILE})

# -- If the output folder exists, then decide to continue
if [[ (-d ${OUT_PATH}) && (${OVERWRITE} == "false") ]]; then
    info_message "${OUT_PATH} already exists and not overwriting."
    exit 0
fi

# -- Generate some folders
mkdir -p ${RUN_PATH}/out/${PROTEIN}/${OUT_FOLDER}

# -- Check if directories exist
assert_dir_exists ${PROT_PATH}
assert_dir_exists ${PROT_PATH}/maps
assert_dir_exists ${OUT_PATH}

# -- Log of the cinvfft files
INV_LOG=${OUT_PATH}/log/invfft_${BASE_MAP/.map/}.log
SIG_LOG=${OUT_PATH}/log/sigma.log
BUC_LOG=${OUT_PATH}/log/buccaneer.log

# -- Move to directory and save new directory
cd ${OUT_PATH}; END_PATH=$(pwd)
 
# -- Assert we are in the correct directory
assert_location ${RUN_PATH} out/${PROTEIN}/${OUT_FOLDER} ${END_PATH}

# -- Remove all files in the current directory
rm -r ./* > /dev/null 2>&1

# -- Create the log directory
mkdir -p ${OUT_PATH}/log

# -- Create an .mtz file from the .map file
cinvfft -mapin ${PROT_PATH}/${MAP_FILE} -mtzin ${PROT_PATH}/dataset.mtz -mtzout __sf.mtz -colout "[FC,PHIC]" > ${INV_LOG}

# -- Prepare input .mtz file for bucaneer
sigmaa HKLIN __sf.mtz HKLOUT buccaneer-input.mtz  >> ${SIG_LOG} <<eof
PARTIAL
LABIN FP=F SIGFP=SIGF FC=FC PHIC=PHIC
END
eof

# -- Start buccaneer to process the data
ccp4-python -u ${CCP4_PATH}/bin/buccaneer_pipeline -stdin > ${BUC_LOG} <<eof
title ${MAP_FILE} build
pdbin-ref ${CCP4_PATH}/lib/data/reference_structures/reference-1tqw.pdb
mtzin-ref ${CCP4_PATH}/lib/data/reference_structures/reference-1tqw.mtz
colin-ref-fo [/*/*/FP.F_sigF.F,/*/*/FP.F_sigF.sigF]
colin-ref-hl [/*/*/FC.ABCD.A,/*/*/FC.ABCD.B,/*/*/FC.ABCD.C,/*/*/FC.ABCD.D]
seqin ${PROT_PATH}/sequence.seq
mtzin buccaneer-input.mtz
colin-fo [/*/*/F,/*/*/SIGF]
colin-free [/*/*/FreeR_flag]
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
