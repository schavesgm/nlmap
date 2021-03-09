#include <iostream>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <string>


// User defined modules
#include <Map.hpp>
#include <path.hpp>
#include <quadrant.hpp>

int main(const int argc, const char** argv)
{
    // Path to the protein
    const char* protein_path = argv[1];
    const char* map_path     = argv[2];

    // Added sigma noise to the map
    const float sigma    = std::stof(argv[3]);
    const float h2       = std::stof(argv[4]);
    const float r_search = std::stof(argv[5]);
    const float r_comp   = std::stof(argv[6]);

    // Generate the path to the protein location
    const auto map_location = Path::join_path(
        protein_path, map_path
    );

    // Load a Map file from memory
    Map map(map_location);

    // Generate the folder to save the data to
    const auto out_path = Path::format_str(
        "%s/maps/s%.4f_h%.4f_rs%.4f_rc%.4f",
        protein_path, sigma, h2, r_search, r_comp
    );

    // Create the output path if needed
    mkdir(out_path.c_str(), 0777);

    // Add some noise to the map
    map.add_noise(sigma);

    // Denoise the map using non-local means
    Map denoised = map.nlmeans_denoise(h2, r_search, r_comp);

    // Save the noisy map in memory
    map.save_map(
        Path::join_path(out_path, "noisy.map")
    );

    // Save the denoised map in memory
    denoised.save_map(
        Path::join_path(out_path, "denoised.map")
    );

return 0;
}
