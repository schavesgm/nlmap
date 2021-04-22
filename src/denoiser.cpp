#include <denoiser.hpp>

// -- Compute a table containing all possible environments in the map {{{
float* Denoiser::table_of_envs(Map& map, const float& R_max)
{
    // First, obtain a table of near indices
    const auto indices = table_of_indices(map, R_max);

    // Allocate memory for all octancts in the grid
    float* envs = new float[Octanct::No * map.get_volume()];

    // Count the corresponding environment for each point
    int env_idx = 0;

    // iterate for each point in the grid to obtain its environment
    for (int w = 0; w < map.Nw; w++) {
        for (int v = 0; v < map.Nv; v++) {
            for (int u = 0; u < map.Nu; u++) {

                // Append the quadrant to the table
                const auto env = construct_environment(map, u, v, w, indices);

                // Copy the data from the environment to the pointed memory
                for (octanct o = 0; o < Octanct::No; o++) {

                    // Index in the contiguos memory
                    const int env_oct = env_idx * Octanct::No + o;

                    // Copy the data into the memory
                    envs[env_oct] = env[o];
                }

                // Move to the next environment in the grid
                env_idx++;
            }
        }
    }

    // Return the table of environments
    return envs;
}
// -- }}}

// -- Get all points whose distance is less or equal to a given distance {{{
vector<grid_point> Denoiser::table_of_indices(Map& map, const float& R_max) 
{
    // Vector that will conatin all nearest indices
    vector<grid_point> table;

    map.grid.use_points_around(Fractional(0, 0, 0), R_max,
        [&map, &table](const float& m, const double& d2, const Position& dp)
        {
            table.push_back(map.grid.get_nearest_point(dp));
        }
    );

    return table;
}
// -- }}}

// -- Construct the environment around a given grid point
vector<float> Denoiser::construct_environment(
    const Map& map, const int& u, const int& v, const int& w, const vector<grid_point>& indices
) {
    // Vector that will contain the environment values separated into octancts
    vector<float> raw_environment[Octanct::No];

    // Get the map value for (u, v, w)
    const float m_central = map.get_value(u, v, w);

    // Iterate for all points in the table
    for (auto& p : indices) {

        // Obtain the right quadrant for the current point
        const unsigned char bw = (p.w >= 0) ? 0b100 : 0b000;
        const unsigned char bv = (p.v >= 0) ? 0b010 : 0b000;
        const unsigned char bu = (p.u >= 0) ? 0b001 : 0b000;

        // Append the map value to the corresponding quadrant
        raw_environment[(bw | bv | bu)].push_back(
            map.get_value(p.u + u, p.v + v, p.w + w)
        );
    }

    return average_environment(raw_environment, m_central);
}
// -- }}}

// -- Compute the average of the raw environment for each octanct {{{
vector<float> Denoiser::average_environment(const vector<float>* raw_environment, const float& m0) 
{
    // Generate the vector containing the average in each quadrant
    vector<float> environment(Octanct::No);

    for (octanct o = 0; o < Octanct::No; o++) {
        // Calculate the average for each octanct adding the central
        if (o != Octanct::o7) {
            environment[o] = std::accumulate(
                raw_environment[o].begin(), raw_environment[o].end(), m0
            ) / (raw_environment[o].size() + 1);
        } else {
            environment[o] = std::accumulate(
                raw_environment[o].begin(), raw_environment[o].end(), 0.0f
            ) / (raw_environment[o].size());
        }
    }

    return environment;
}
// -- }}}

// -- Compute the table of averaged environments for the given map {{{
vector<float> Denoiser::table_of_envavg(Map& map, const float& R_max)
{
    // Get some needed aliases
    const int& Ne = map.get_volume();
    const int& No = Octanct::No;

    // Allocate memory for the table of averaged environments
    vector<float> avg_envs(Ne);

    // Construct the environments for the given map
    float* envs = table_of_envs(map, R_max);

    // Build the average for each environment
    for (int e = 0; e < Ne; e++) {

        // Temporary that holds the average value
        float env_sum = 0.0f;

        // Add all octanct values to the average
        for (octanct o = 0; o < No; o++) {
            env_sum += envs[e * No + o];
        }

        // Compute the averaged environment
        avg_envs[e] = env_sum / No;
    }

    // Delete the heap allocated memory
    delete[] envs;

    return avg_envs;
}

vector<float> Denoiser::table_of_envavg(Map& map, const float* envs)
{
    // Get some needed aliases
    const int& Ne = map.get_volume();
    const int& No = Octanct::No;

    // Allocate memory for the table of averaged environments
    vector<float> avg_envs(Ne);

    // Build the average for each environment
    for (int e = 0; e < Ne; e++) {

        // Temporary that holds the average value
        float env_sum = 0.0f;

        // Add all octanct values to the average
        for (octanct o = 0; o < No; o++) {
            env_sum += envs[e * No + o];
        }

        // Calculate the average
        avg_envs[e] = env_sum / No;
    }
    return avg_envs;
}
// -- }}}

// -- Compute the standard deviation for each environment {{{
vector<float> Denoiser::table_of_envstd(Map& map, const float* envs)
{
    // Get some needed aliases
    const int& Ne = map.get_volume();
    const int& No = Octanct::No;

    // Compute the average for each environment in the map
    vector<float> avg_envs = Denoiser::table_of_envavg(map, envs);

    // Allocate memory for the table of averaged environments
    vector<float> std_envs(Ne);

    // Build the average for each environment
    for (int e = 0; e < Ne; e++) {

        // Temporary that holds the average value
        float env_std = 0.0f;

        // Add all octanct values to the average
        for (octanct o = 0; o < No; o++) {
            env_std += std::pow(envs[e * No + o] - avg_envs[e], 2);
        }

        // Compute the averaged environment
        std_envs[e] = std::sqrt(env_std / (No - 1));
    }

    return std_envs;
}
// -- }}}

// -- Function to compare two environments included in a table {{{
void Denoiser::compute_kernels( 
    float* kernels, const float* envs, const octanct* rots, const int& er, const int& Ne, 
    const float& hd, const vector<float>& envavg, const vector<float>& envstd
) {
    // Get the total number of rotations and octancts
    const int& Nr = Octanct::Nr;
    const int& No = Octanct::No;

    // Precompute some values that will be used in the loops
    const float two_sigmasq = 2 * hd * hd;
    const int ref_row = er * No;

    // References to some repeated memory addressed
    const float& ref_avg = envavg[er];
    const float& ref_std = envstd[er];

    for (int e = 0; e < Ne; e++) {

        // Calculate the needed filter parameters
        const float dA = envavg[e] - ref_avg;
        const float dS = envstd[e] - ref_std;

        // Filter the data using the averaged environment
        if ((std::abs(dA + dS) < hd) && (std::abs(dA - dS) < hd)) {

            // Variable to hold the minimum value among all comparison
            float min_dsq = 1000000;

            // Compare the environments for each rotation
            for (int r = 0; r < Nr; r++) {

                // Variable to compute the distance squared for this iteration
                float dist_sq = 0.0f;

                // Precompute some values to avoid calculations
                const int rot_row = r * No;

                // Use all octancts in the comparison
                for (octanct o = Octanct::o0; o < No; o++) {

                    // Obtain the rotated reference environment and the comparison octanct
                    const float refr = envs[ref_row + rots[rot_row + o]];
                    const float comp = envs[e * No + o];

                    // Compute the distance squared
                    dist_sq += (refr - comp) * (refr - comp);
                }

                // Normalise the distance squared by the number of octancts
                dist_sq = dist_sq / No;

                // Update the minimum distance if possible
                min_dsq = (min_dsq > dist_sq) ? dist_sq / 8 : min_dsq;
            }

            // Append the minimum distance to the vector of minimum distances
            kernels[e] = exp(- min_dsq / two_sigmasq);
        }
    }
}
// -- }}}

// -- Function to compute the denoised version of the map value {{{
inline float Denoiser::compute_uhat(const float* kernel, const float* maps, const int& Ne)
{
    // Variable that will contain the result
    float uhat = 0.0f; float sumk = 0.0f;

    // Iterate for all comparision environments
    for (int e = 0; e < Ne; e++) {
        uhat += kernel[e] * maps[e]; sumk += kernel[e];
    }

    return uhat / sumk;
}
// -- }}}

// -- Main algorithm to denoise a map using non-local means {{{
std::tuple<Map, float, vector<float>> Denoiser::nlmeans_denoiser(
    Map& map, const float& p_thresh, const float& R_max
) {
    // Construct some needed aliases
    const int& Ne = map.get_volume();  // -- Number of environments in the map
    const int& No = Octanct::No;       // -- Number of octancts in an env (8)
    const int& Nr = Octanct::Nr;       // -- Number of rotations per comp (10)

    // Generate a copy of the map to denoise it
    Map denoised_map = map;

    // Generate a table containing all the environments
    const float* envs = table_of_envs(map, R_max);

    // Generate a table containing the average of each environment
    const vector<float> envavg = table_of_envavg(map, envs);

    // Generate a table containing the standard dev of each environment
    const vector<float> envstd = Denoiser::table_of_envstd(map, envs);

    // Generate a table containing the rotated indices for each rotation
    const octanct* rots = Octanct::table_of_rotations();

    // Pointer to the map data in memory
    const float* map_data = map.data();

    // Get the maximum and minimum environment average
    const auto min = std::min_element(envavg.begin(), envavg.end());
    const auto max = std::max_element(envavg.begin(), envavg.end());

    // Calculate the denoising parameter using the threshold provided
    const float hd = p_thresh * (*max - *min);

    // Buffer to store the minimum distance squared and the kernel values
    float* kernels = new float[Ne];

    // Iterate for each position in the grid
    for (int er = 0; er < Ne; er++) {

        // Calculate the kernel corresponding to each distance squared
        compute_kernels(kernels, envs, rots, er, Ne, hd, envavg, envstd);

        // Compute the denoised version of the map at the current location
        denoised_map.set_value(er, compute_uhat(kernels, map_data, Ne));

        // Set the kernels memory block to zero
        memset(kernels, 0.0f, Ne * sizeof(float));

        if (er == 1000) { break; }
    }

    // Delete the heap allocated data
    delete[] kernels;
    delete[] envs;
    delete[] rots;

    // Return a tuple containing the denoised map and the denoised parameter
    return std::make_tuple(denoised_map, hd, envavg);
}
// -- }}}
