function copy_files()
{
    # -- Retrieve the needed input and output paths
    PROT_PATH=${1}; OUT_PATH=${2}

    # -- Copy the files into the location
    cp ${PROT_PATH}/dataset.mtz  ${OUT_PATH}
    cp ${PROT_PATH}/refmac.mtz   ${OUT_PATH}
    cp ${PROT_PATH}/refmac.pdb   ${OUT_PATH}
    cp ${PROT_PATH}/sequence.seq ${OUT_PATH}
}

function print_help()
{
    printf " -- pipeline \n\n"
    printf "  * Usage:\n"
    printf "                ./pipeline [protein] [sigma] [perc_h] [r_env] (gpu_id) \n\n"
    printf "  * Parameters:\n"
    printf "     * protein: Name of the protein. The program needs all proteins\n"
    printf "       to be processed located inside a data/ folder. The data folder\n"
    printf "       must be present in the same directory where pipeline is called.\n"
    printf "     * sigma (s): Standard deviation of the additive gaussian noise added\n"
    printf "       to the map. sigma = 0.0 will not add any noise.\n"
    printf "     * perc_h (p): Percentage of the total spread of the map that will be\n"
    printf "       used to generate the denoising parameter.\n"
    printf "     * r_env  (r): Radious that defines the environment volume. All points\n"
    printf "       whose distance is less or equal than r_env from a reference point\n"
    printf "       will be considered in the environment calculation.\n"
    printf "     * gpu_id: Optional. Index of the gpu device used in the calculation.\n"
    printf "       Default is 0\n\n"
    printf "  * Example: pipeline rnase 0.0 0.05 2.0\n"
    printf "     This run will create two folders inside a folder named out. One will\n"
    printf "     be stored inside data/rnase and the other inside out/log/rnase. The\n"
    printf "     particular locator for the current parameters is:\n\n"
    printf "                     s0.0000_h0.0266_r2.0000_p0.0500\n\n"
    printf "  * Important:\n"
    printf "     - pipeline needs a environmental variable called \$CCP4_PATH\$\n"
    printf "    containing the path to the CCP4 software. For example, it can\n"
    printf "    be CCP4_PATH=~/software/ccp4-7.1.\n"
    printf " --\n"
}
