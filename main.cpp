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
    const float sigma  = std::stof(argv[3]);
    const float perc_t = std::stof(argv[4]);
    const float r_comp = std::stof(argv[5]);

    // Generate the path to the protein location
    const auto map_location = Path::join_path(protein_path, map_path);

    // Load a Map file from memory
    Map original(map_location);

    // Add some noise to the map
    original.add_noise(sigma);

    // Denoise the map using non-local means
    Map denoised = original.nlmeans_denoise(perc_t, r_comp);

    // Retrieve the denoise parameter
    const float h = denoised.hd;

    // Generate the folder to save the data to
    const auto out_path = Path::format_str(
        "%s/maps/s%.4f_h%.4f_r%.4f_p%.4f", 
        protein_path, sigma, h, r_comp, perc_t
    );

    // Create the output path if needed
    mkdir(out_path.c_str(), 0777);

    // Save the noisy map in memory
    original.save_map(
        Path::join_path(out_path, "noisy.map")
    );

    // Save the denoised map in memory
    denoised.save_map(
        Path::join_path(out_path, "denoised.map")
    );
    
    // Save the average for each environment in the noisy map
    original.save_table_of_avg(
        Path::join_path(out_path, "table_avg_noisy.dat"), r_comp
    );

    // Save the average for each environment in the denoised map
    denoised.save_table_of_avg(
        Path::join_path(out_path, "table_avg_denoised.dat"), r_comp
    );

    // Output the value of h to capture it in the pipeline
    std::cout << h << std::endl;

return 0;
}
