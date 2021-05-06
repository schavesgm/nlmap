#include <denoiser.hpp>

// -- Inline function to get all octancts in a vector
inline void get_octancts(vector<unsigned char>& vec, const unsigned char& pos_flag, const int& p)
{
    // Flag that implies that p < 0
    const unsigned char neg_flag = 0b000;

    // Calculate the corresponding octanct
    const unsigned char oct = (p > 0) ? pos_flag : neg_flag;

    // Append the value into the vector
    vec.push_back(oct);

    // If p is zero, then append the masked octanct as well
    if (p == 0) vec.push_back(oct ^ pos_flag);
}

// -- Indices whose distance to a central point is less than R_max {{{
vector<grid_point> Denoiser::table_of_indices(Map& map, const float& r_env) 
{
    // Vector that will conatin all nearest indices
    vector<grid_point> indices;

    map.grid.use_points_around(Fractional(0, 0, 0), r_env,
        [&map, &indices](const float& m, const double& d2, const Position& dp)
        {
            indices.push_back(map.grid.get_nearest_point(dp));
        }
    );

    return indices;
}
// -- }}}

// -- Construct the environment around a given grid point
vector<float> Denoiser::get_env(const Map& map, const int& u, const int& v, const int& w, const vector<grid_point>& indices)
{
    // Vector that will contain the environment values separated into octancts
    vector<float> raw_env[Octanct::No];

    // Vectors used to assign points to octancts
    vector<octanct> u_oct, v_oct, w_oct;

    // Constants to avoid magic numbers
    const unsigned char positive_u_flag = 0b001; // 1
    const unsigned char positive_v_flag = 0b010; // 2
    const unsigned char positive_w_flag = 0b100; // 4

    // Iterate for all points in the table
    for (auto& p : indices) {

        // Obtain the right octancts in the u direction
        get_octancts(u_oct, positive_u_flag, p.u);

        // Obtain the right octancts in the v direction
        get_octancts(v_oct, positive_v_flag, p.v);

        // Obtain the right octancts in the w direction
        get_octancts(w_oct, positive_w_flag, p.w);

        // Normalisation factor used to divide points among octancts
        const int norm = u_oct.size() * v_oct.size() * w_oct.size();

        // Append the normalised map value to the relevant octancts
        for (auto& ou : u_oct) {
            for (auto& ov : v_oct) {
                for (auto& ow : w_oct) {
                    raw_env[(ow | ov | ou)].push_back(
                        map.get_value(p.u + u, p.v + v, p.w + w) / norm
                    );
                }
            }
        }

        // Clear the memory in all octanct vectors for next iteration
        u_oct.clear(); v_oct.clear(); w_oct.clear();
    }

    // Calculate the average of the raw environment for each octanct
    vector<float> env(Octanct::No);

    for (octanct o = 0; o < Octanct::No; o++) {
        // Calculate the sum of all point in the octanct
        env[o] = std::accumulate(raw_env[o].begin(), raw_env[o].end(), 0.0f);

        // Normalise by the number of points to obtain the average
        env[o] = env[o] / raw_env[o].size();
    }

    return env;
}
// -- }}}

// -- Get the average points per octanct {{{
int Denoiser::avg_points_per_octanct(Map& map, const float& r_env)
{
    // Obtain the table of indices
    const auto indices = table_of_indices(map, r_env);

    // Vectors that will contain the points
    vector<int> count_env[Octanct::No];

    // Vector that will contain the average of each octanct
    vector<float> pp_oct(Octanct::No);

    // Vectors used to assign points to quadrants
    vector<octanct> u_oct, v_oct, w_oct;

    // Constants to avoid magic numbers
    const unsigned char positive_u_flag = 0b001; // 1
    const unsigned char positive_v_flag = 0b010; // 2
    const unsigned char positive_w_flag = 0b100; // 4

    // Iterate for all points in the table
    for (auto& p : indices) {

        // Obtain the right octancts in the u direction
        get_octancts(u_oct, positive_u_flag, p.u);

        // Obtain the right octancts in the v direction
        get_octancts(v_oct, positive_v_flag, p.v);

        // Obtain the right octancts in the w direction
        get_octancts(w_oct, positive_w_flag, p.w);

        // Add one count to each relevant quadrant
        for (auto& ou : u_oct) {
            for (auto& ov : v_oct) {
                for (auto& ow : w_oct) {
                    count_env[(ow | ov | ou)].push_back(1);
                }
            }
        }

        // Clear the vectors for the next iteration
        u_oct.clear(); v_oct.clear(); w_oct.clear();
    }

    // Calculate the average number of points per octact
    float avg_count = 0.0f;

    // Get the number of points per octanct
    for (octanct o = 0; o < Octanct::No; o++) {
        avg_count += count_env[o].size();
    }

    return avg_count / Octanct::No;
}
// -- }}}

// -- Table containing the environment data, its average and standard deviation {{{
float* Denoiser::table_of_envs(Map& map, const float& r_env)
{
    // First, obtain a table of near indices
    const auto indices = table_of_indices(map, r_env);

    // Number of rows and columns in the array
    const int rows = map.get_volume();
    const int cols = Octanct::No + 2;

    // Allocate memory for all octancts in the grid
    float* envs = new float[rows * cols];

    // Count the corresponding environment for each point
    int env_idx = 0;

    // iterate for each point in the grid to obtain its environment
    for (int w = 0; w < map.Nw; w++) {
        for (int v = 0; v < map.Nv; v++) {
            for (int u = 0; u < map.Nu; u++) {

                // Append the quadrant to the table
                const auto env = get_env(map, u, v, w, indices);

                // Temporary that will contain the average of the environment
                float env_avg = 0.0f;

                // Copy the octanct data and calculate the average value
                for (octanct o = 0; o < Octanct::No; o++) {

                    // Index in the contiguos memory
                    const int env_oct = env_idx * cols + o;

                    // Octanct for the current environment
                    const float& oct = env[o];

                    // Copy the data into the memory
                    envs[env_oct] = oct;

                    // Add the environment to the sum
                    env_avg += oct;
                }

                // Calculate the average of the environment
                env_avg = env_avg / Octanct::No;

                // Temporary containing the standard deviation of the environment
                float env_std = 0.0f;

                // Calculate the standard deviation of the current environment
                for (octanct o = 0; o < Octanct::No; o++) {

                    // Index in the contiguos memory
                    const int env_oct = env_idx * cols + o;

                    // Octanct for the current environment
                    const float& oct = env[o];

                    // Add the environment to the standard deviation
                    env_std += std::pow(oct - env_avg, 2);
                }

                // Append the standard deviation to the table
                envs[env_idx * cols + Octanct::No + 0] = env_avg;
                envs[env_idx * cols + Octanct::No + 1] = std::sqrt(env_std / (Octanct::No - 1));

                // Move to the next environment in the grid
                env_idx++;
            }
        }
    }

    // Return the table of environments
    return envs;
}
// -- }}}

// -- Table containing all averaged environments {{{
vector<float> Denoiser::table_of_stats(Map& map, const float& r_env)
{
    // First, obtain a table of near indices
    const auto indices = table_of_indices(map, r_env);

    // Number of rows and columns in the array
    const int Ne = map.get_volume();

    // Allocate memory for all octancts in the grid
    vector<float> env_stats(Ne * 2);

    // Count the corresponding environment for each point
    int env_idx = 0;

    // iterate for each point in the grid to obtain its environment
    for (int w = 0; w < map.Nw; w++) {
        for (int v = 0; v < map.Nv; v++) {
            for (int u = 0; u < map.Nu; u++) {

                // Append the quadrant to the table
                const auto env = get_env(map, u, v, w, indices);

                // Temporary that will contain the average of the environment
                float avg = 0.0f;

                // Sum all octancts in the environment
                for (octanct o = 0; o < Octanct::No; o++) { 
                    avg += env[o]; 
                }

                // Normalise the average
                avg = avg / Octanct::No;

                // Temporary containing the standard deviation of the environment
                float std = 0.0f;

                // Calculate the standard deviation of the current environment
                for (octanct o = 0; o < Octanct::No; o++) { 
                    std += std::pow(env[o] - avg, 2);
                }

                // Append the average and the standard deviation to the vector
                env_stats[env_idx * 2 + 0] = avg;
                env_stats[env_idx * 2 + 1] = std::sqrt(std / (Octanct::No - 1));

                // Move to the next environment in the grid
                env_idx++;
            }
        }
    }

    // Return the table of environments
    return env_stats;
}
// -- }}}

// -- Main algorithm to denoise a map using non-local means {{{
std::tuple<Map, float, vector<float>, vector<float>> Denoiser::nlmeans_denoiser(
    Map& map, const float& p_thresh, const float& r_env, const float& eps
) {
    // Construct some needed aliases
    const int& Ne = map.get_volume();  // -- Number of environments (points) in the map
    const int& No = Octanct::No;       // -- Number of octancts in an env (8)
    const int& Nr = Octanct::Nr;       // -- Number of rotations per comp (10)
    const int  Nv = No + 2;            // -- Number of octancts + env_avg + env_std

    // Generate a copy of the map to denoise it
    Map denoised_map = map;

    // Set the map data to zero
    denoised_map.set_data(0.0f);

    // Pointers to the relevant map data used in the algorithm
    float* denoised_M = denoised_map.data();
    float* original_M = map.data();

    // Memory used to calculate the normalisation for each map
    float* sum_kernels = new float[Ne]{0.0f};

    // Memory used to count the amount of points that pass the prefilter
    float* prefilter_passed = new float[Ne]{0};

    // Generate a table containing all the environments, their avg and std
    const float* envs = table_of_envs(map, r_env);

    // Generate a table containing the rotated indices for each rotation
    const octanct* rots = Octanct::table_of_rotations();

    // Construct a vector of avg environments, needed for output
    vector<float> env_stats(Ne * 2); vector<float> env_avg(Ne);

    // Copy the environment averages inside the vector
    for (int e = 0; e < Ne; e++) { 
        env_stats[e * 2 + 0] = envs[e * Nv + No + 0]; // Average
        env_stats[e * 2 + 1] = envs[e * Nv + No + 1]; // Standard deviation
        env_avg[e] = envs[e * Nv + No];               // Average
    }

    // Get the maximum and minimum environment average
    const auto min = std::min_element(env_avg.begin(), env_avg.end());
    const auto max = std::max_element(env_avg.begin(), env_avg.end());

    // Calculate the denoising parameter using the threshold provided
    const float hd  = p_thresh * (*max - *min);
    const float den = 2 * hd * hd;

    // Iterate for each reference environment in the grid
    for (int er = 0; er < Ne; er++) {

        // Append the central value with maximum weight
        denoised_M[er] += 1.0f * original_M[er];

        // References to the reference environment avg and std
        const float& rA = envs[er * Nv + No];

        // The prefilter is passed when comparing reference with reference
        prefilter_passed[er] += 1;

        // Iterate for all comparision environments without repetition
        for (int ec = er + 1; ec < Ne; ec++) {

            // References to the comparison environment avg and std
            const float& cA = envs[ec * Nv + No];

            // Compute the difference for averages and deviations
            const float dA = rA - cA;

            // Filter to enhance performance of the denoiser
            if (std::abs(dA) < eps * hd) {

                // Variable containing the minimum distance squared
                float min_dsq = 1000000000;

                // Compare both environments using all possible rotations
                for (int r = 0; r < Nr; r++) {

                    // Temporary containing the distance squared for this rot
                    float d_sq = 0.0f;

                    // Iterate over all octancts
                    for (octanct o = 0; o < No; o++) {

                        // Access the current environment
                        const float& refr = envs[er * Nv + rots[r * No + o]];
                        const float& comp = envs[ec * Nv + o];

                        // Update the distance squared with the current value
                        d_sq += std::pow(refr - comp, 2);
                    }

                    // Normalise the distance squared
                    d_sq = d_sq / No;

                    // Update the minimum distance
                    min_dsq = (min_dsq > d_sq) ? d_sq : min_dsq;
                }

                // Compute the kernel for the current comparison
                const float kernel = std::exp(- min_dsq / den);

                // Update the map value with the weighted sum
                denoised_M[er] += kernel * original_M[ec];
                denoised_M[ec] += kernel * original_M[er];

                // Update the sum of kernels for each
                sum_kernels[er] += kernel;
                sum_kernels[ec] += kernel;

                // The prefilter is passed both for the reference and comparison env
                prefilter_passed[er] += 1;
                prefilter_passed[ec] += 1;

            } // -- End of prefilter
        } // -- End of comparison environment

        // Denoise the value by computing the weighted average
        denoised_M[er] /= (sum_kernels[er] + 1.0f);

    } // -- End of main loop

    // Normalise the prefilter with the number of examples
    for (int e = 0; e < Ne; e++) { prefilter_passed[e] /= Ne; }

    // Calculate some statistics of the prefilter
    std::vector<float> monitor(7);

    monitor[0] = Stats::mean(prefilter_passed, Ne);
    monitor[1] = Stats::std(prefilter_passed, Ne);
    monitor[2] = Stats::median(prefilter_passed, Ne);
    monitor[3] = Stats::max(prefilter_passed, Ne);
    monitor[4] = Stats::min(prefilter_passed, Ne);
    monitor[5] = Ne;
    monitor[6] = avg_points_per_octanct(map, r_env);

    // Delete the heap allocated data
    delete[] envs;
    delete[] rots;
    delete[] sum_kernels;
    delete[] prefilter_passed;

    // Return a tuple containing the denoised map and the denoised parameter
    return std::make_tuple(denoised_map, hd, env_stats, monitor);
}
// -- }}}
