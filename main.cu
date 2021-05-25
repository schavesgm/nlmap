#include <iostream>
#include <tuple>

// User defined modules
#include <Map.hpp>
#include <Argparser.hpp>
#include <path.hpp>
#include <denoiser.hpp>
#include <utils.hpp>
#include <stats.hpp>

int main(const int argc, char** argv)
{
    // Generate an argparser object to deal with command line input
    const Argparser command_args(argc, argv);

    // Check if the argparser contains the --help flag
    const bool is_help = command_args.check_flag("--help");

    if (is_help) {
        std::cout << 
        "  -- denoise_map\n"
        "  Usage:\n"
        "  denoise map --path [str] --name [str] --s [float] --p [float] --r [float]\n\n"
        "  Arguments:\n"
        "   --path: Path where the data is stored.   Example: ./data/rnase\n"
        "   --name: Name of the map file to process. Example: ./refmac.map\n"
        "   --s:    Standard deviation of the noise add. If zero, no noise added\n"
        "   --p:    Percentage of the total spread of the map used to create the\n"
        "           denoiser parameter.\n"
        "   --r:    Radious of search used to create an environment.\n"
        "  Example:\n"
        "  denoise_map --path data/rnase --name refmac.map --s 0.0 --p 0.05 --r 2.0\n\n";
        return 0;
    }

    // Check the existence of several flags in the system
    const bool is_path  = command_args.check_flag("--path");
    const bool is_name  = command_args.check_flag("--name");
    const bool is_s     = command_args.check_flag("--s");
    const bool is_p     = command_args.check_flag("--p");
    const bool is_r     = command_args.check_flag("--r");

    if (!is_path || !is_name || !is_s || !is_p || !is_r) {
        std::cout << " ERROR: Command line arguments are incorrect\n";
        return 1;
    }

    // Get the correct data from the argument parser
    const std::string protein_path = command_args.get_flag("--path");
    const std::string map_name     = command_args.get_flag("--name");
    const float sigma              = command_args.get_flag<float>("--s");
    const float perc_t             = command_args.get_flag<float>("--p");
    const float r_env              = command_args.get_flag<float>("--r");

    // Obtain the name of the protein from the protein path
    const auto protein = Path::get_basename(protein_path);

    // Load a Map file from memory
    Map original_map(Path::join_path(protein_path, map_name));

    // Add some noise to the map according to sigma
    original_map.add_noise(sigma);

    // Denoise the map using the map denoiser
    auto denoiser_output = Denoiser::nlmeans_denoiser(original_map, perc_t, r_env);

    // References to the objects encoded in the denoiser output
    auto& denoised_map       = std::get<0>(denoiser_output);
    auto& denoise_param      = std::get<1>(denoiser_output);

    // Calculate the environment statistics of the noisy and denoised maps
    auto noisy_env_stats    = Denoiser::table_of_stats(original_map, r_env);
    auto denoised_env_stats = Denoiser::table_of_stats(denoised_map, r_env);

    // Generate the path where the maps will be stored
    const auto maps_path = Path::format_str(
        "out/data/%s/s%.4f_h%.4f_r%.4f_p%.4f",
        protein.c_str(), sigma, denoise_param, r_env, perc_t
    );

    // Generate the path where the log will be output
    const auto logs_path = Path::format_str(
       "out/log/%s/s%.4f_h%.4f_r%.4f_p%.4f",
       protein.c_str(), sigma, denoise_param, r_env, perc_t
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

    // Output the value of h to capture it in the pipeline
    std::cout << denoise_param << std::endl;

return 0;
}
