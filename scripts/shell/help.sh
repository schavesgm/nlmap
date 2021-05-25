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
    printf " -- pipeline script\n"
    printf "    ./pipeline [protein] [sigma] [perc_h] [r_env] [pref_eps]\n"
    printf "  * Parameters:\n"
    printf "     * protein: Name of the protein. The program needs all proteins\n"
    printf "       to be processed located inside a data/ folder. The data folder\n"
    printf "       must be present in the same directory where pipeline is called.\n"
    printf "     * sigma: Standard deviation of the additive gaussian noise added\n"
    printf "       to the map. sigma = 0.0 will not add any noise.\n"
    printf "     * perc_h: Percentage of the total spread of the map that will be\n"
    printf "       used to generate the denoising parameter.\n"
    printf "     * r_env: Radious that defines the environment volume. All points\n"
    printf "       whose distance is less or equal than r_env from a reference point\n"
    printf "       will be considered in the environment calculation.\n\n"
    printf "  * Important:\n"
    printf "     - pipeline needs a environmental variable called \$CCP4_PATH\$\n"
    printf "    containing the path to the CCP4 software. For example, it can\n"
    printf "    be CCP4_PATH=~/software/ccp4-7.1.\n"
    printf " --\n"
}
