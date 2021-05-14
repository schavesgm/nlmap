#include <iostream>
#include <tuple>

// User defined modules
#include <Map.hpp>
#include <path.hpp>
#include <denoiser.hpp>
#include <utils.hpp>
#include <stats.hpp>

int main(const int argc, const char** argv)
{
    // Path to the protein
    const char* protein_path = argv[1];
    const char* map_name     = argv[2];

    // Added sigma noise to the map
    const float sigma  = std::stof(argv[3]);
    const float perc_t = std::stof(argv[4]);
    const float r_env  = std::stof(argv[5]);
    const float feps   = std::stof(argv[6]);

    // Obtain the name of the protein from the protein path
    const auto protein = Path::get_basename(protein_path);

    // Load a Map file from memory
    Map original_map(Path::join_path(protein_path, map_name));

    // Add some noise to the map according to sigma
    original_map.add_noise(sigma);

    // Denoise the map using the map denoiser
    auto denoiser_output = Denoiser::nlmeans_denoiser(original_map, perc_t, r_env, feps);

    // References to the objects encoded in the denoiser output
    auto& denoised_map       = std::get<0>(denoiser_output);
    auto& denoise_param      = std::get<1>(denoiser_output);
    auto& monitor_data       = std::get<2>(denoiser_output);

    // Calculate the environment statistics of the noisy and denoised maps
    auto noisy_env_stats    = Denoiser::table_of_stats(original_map, r_env);
    auto denoised_env_stats = Denoiser::table_of_stats(denoised_map, r_env);

    // Generate the path where the maps will be stored
    const auto maps_path = Path::format_str(
        "out/data/%s/s%.4f_h%.4f_r%.4f_p%.4f_e%.4f", 
        protein.c_str(), sigma, denoise_param, r_env, perc_t, feps
    );

    // Generate the path where the log will be output
    const auto logs_path = Path::format_str(
       "out/log/%s/s%.4f_h%.4f_r%.4f_p%.4f_e%.4f",
       protein.c_str(), sigma, denoise_param, r_env, perc_t, feps
    );

    // Create the basic directories if needed
    Path::make_path(maps_path); 
    Path::make_path(logs_path);

    // Paths to the noisy and denoised data
    const auto n_files_path = Path::join_path(maps_path, "noisy/files");
    const auto d_files_path = Path::join_path(maps_path, "denoised/files");

    // Paths to the noisy and denoised logs
    const auto n_log_path = Path::join_path(maps_path, "noisy/log");
    const auto d_log_path = Path::join_path(maps_path, "denoised/log");

    // Create the needed directories
    Path::make_path(n_files_path); Path::make_path(n_log_path);
    Path::make_path(d_files_path); Path::make_path(d_log_path);

    // Save the noisy and denoised maps in memory
    original_map.save_map(Path::join_path(n_files_path, "noisy.map"));
    denoised_map.save_map(Path::join_path(d_files_path, "denoised.map"));

    // Save the statistics of the environment in memory
    Utils::save_envstats(
        Path::join_path(n_log_path, "envstats.dat"), 
        noisy_env_stats, original_map
    );

    // Save the average for each environment in the denoised map
    Utils::save_envstats(
        Path::join_path(d_log_path, "envstats.dat"), 
        denoised_env_stats, denoised_map
    );

    // Save the monitor data into the log path
    Utils::save_stats(Path::join_path(logs_path, "monitor.dat"), monitor_data);

    // Output the value of h to capture it in the pipeline
    std::cout << denoise_param << std::endl;

return 0;
}
