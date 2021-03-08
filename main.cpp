#include <iostream>
#include <sys/stat.h> 
#include <sys/types.h> 


// User defined modules
#include <Map.hpp>
#include <format.hpp>
#include <quadrant.hpp>

int main(const int argc, const char** argv)
{
    // Load a Map file from memory
    Map map(argv[1]);

    // Added sigma noise to the map
    const float sigma = std::stof(argv[2]);

    // Denoising h squared parameter
    const float h2 = std::stof(argv[3]);

    // Radious of comparison search in the denoiser
    const float r_search = std::stof(argv[4]);

    // Radious of comparison in the denoiser
    const float r_comp = std::stof(argv[5]);

    // Generate the folder to save the data to
    auto out_path = Format::format_string(
        "./data/rnase/maps/s%.4f_h%.4f_rs%.4f_rc%.4f",
        sigma, h2, r_search, r_comp
    );

    // Create the output path if needed
    mkdir(out_path.c_str(), 0777);

    // Add some noise to the map
    map.add_noise(sigma);

    // Denoise the map using non-local means
    Map denoised = map.nlmeans_denoise(h2, r_search, r_comp);

    // Save the noisy and denoised maps in memory
    map.save_map(out_path + "/noisy.map");
    denoised.save_map(out_path + "/denoised.map");

return 0;
}
