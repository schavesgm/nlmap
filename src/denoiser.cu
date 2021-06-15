#include <denoiser.hpp>
#include <iomanip>

// -- Inline function to get all octancts in a vector
__host__
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
__host__
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
__host__
vector<float>* Denoiser::get_octs(
    const Map& map, const int& u, const int& v, const int& w, 
    const vector<grid_point>& indices
) {
    // Vector containing the points per octanct
    vector<float>* oct_points = new vector<float>[Octanct::No];

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
                    oct_points[(ow | ov | ou)].push_back(
                        map.get_value(p.u + u, p.v + v, p.w + w) / norm
                    );
                }
            }
        }

        // Clear the memory in all octanct vectors for next iteration
        u_oct.clear(); v_oct.clear(); w_oct.clear();
    }

    return oct_points;
}
// -- }}}

// -- Get the average points per octanct {{{
__host__
int Denoiser::avg_points_per_octanct(Map& map, const float& r_env)
{
    // Obtain the table of indices
    const auto indices = table_of_indices(map, r_env);

    // Vectors that will contain the points
    vector<int> oct_points[Octanct::No];

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
                    oct_points[(ow | ov | ou)].push_back(1);
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
        avg_count += oct_points[o].size();
    }

    return avg_count / Octanct::No;
}
// -- }}}

// -- Table containing the environment data, its average and standard deviation {{{
__host__
float* Denoiser::table_of_envs(Map& map, const float& r_env)
{
    // First, obtain a table of near indices
    const auto indices = table_of_indices(map, r_env);

    // Number of rows and columns in the array
    const int& Ne = map.get_volume();
    const int& No = Octanct::No;

    // Allocate memory for all octancts in the grid
    float* envs = new float[Ne * No];

    // Count the corresponding environment for each point
    int eidx = 0;

    // iterate for each point in the grid to obtain its environment
    for (int w = 0; w < map.Nw; w++) {
        for (int v = 0; v < map.Nv; v++) {
            for (int u = 0; u < map.Nu; u++) {

                // Get all points in each octanct
                const vector<float>* oct_points = get_octs(map, u, v, w, indices);

                // Vector containing the sum of all points in each octanct
                float oct_sum[Octanct::No];

                // Iterate for each octanct to calculate its average value
                for (int o = 0; o < Octanct::No; o++) {

                    // Sum of all points in the current octanct
                    oct_sum[o] = std::accumulate(
                        oct_points[o].begin(), oct_points[o].end(), 0.0f
                    );

                    // Copy the octanct average to the correct environment
                    envs[eidx * No + o] = oct_sum[o] / oct_points[o].size();
                }

                // Move to the next environment in the grid
                eidx++;

                // Delete the heap allocated data
                delete[] oct_points;
            }
        }
    }

    // Return the table of environments
    return envs;
}
// -- }}}

// -- Table containing environment averages {{{
__host__
vector<float> Denoiser::table_of_stats(Map& map, const float& r_env)
{
    // First, obtain a table of near indices
    const auto indices = table_of_indices(map, r_env);

    // Number of rows and columns in the array
    const int Ne = map.get_volume();

    // Allocate memory for all octancts in the grid
    vector<float> env_stats(Ne);

    // Count the corresponding environment for each point
    int eidx = 0;

    // iterate for each point in the grid to obtain its environment
    for (int w = 0; w < map.Nw; w++) {
        for (int v = 0; v < map.Nv; v++) {
            for (int u = 0; u < map.Nu; u++) {

                // Get all points in each octanct
                const vector<float>* oct_points = get_octs(map, u, v, w, indices);

                // Vector containing the sum of all points in each octanct
                float oct_sum[Octanct::No];

                // Temporary that will contain the avg of the environment
                float env_avg = 0.0f;

                // Iterate for each octanct to calculate its average value
                for (int o = 0; o < Octanct::No; o++) {

                    // Sum of all points in the current octanct
                    oct_sum[o] = std::accumulate(
                        oct_points[o].begin(), oct_points[o].end(), 0.0f
                    );

                    // Add the value to the environment average
                    env_avg += oct_sum[o];
                }

                // Calculate the average of the environment
                env_stats[eidx] = env_avg / indices.size();

                // Move to the next environment in the grid
                eidx++;

                // Delete the heap allocated data
                delete[] oct_points;
            }
        }
    }

    // Return the table of environments
    return env_stats;
}

__host__
vector<float> Denoiser::table_of_stats(Map& map, const float* envs, const float& r_env)
{
    // First, obtain a table of near indices
    const auto indices = table_of_indices(map, r_env);

    // Number of rows and columns in the array
    const int Ne = map.get_volume();

    // Allocate memory for all octancts in the grid
    vector<float> env_stats(Ne);

    // Count the corresponding environment for each point
    int eidx = 0;

    // iterate for each point in the grid to obtain its environment
    for (int w = 0; w < map.Nw; w++) {
        for (int v = 0; v < map.Nv; v++) {
            for (int u = 0; u < map.Nu; u++) {

                // Temporary that will contain the avg of the environment
                float env_avg = 0.0f;

                // Add each octanct to the environment average
                for (int o = 0; o < Octanct::No; o++) {
                    env_avg += envs[eidx * Octanct::No + o];
                }

                // Calculate the average of the environment
                env_stats[eidx] = env_avg / indices.size();

                // Move to the next environment in the grid
                eidx++;
            }
        }
    }

    // Return the table of environments
    return env_stats;
}
// -- }}}

// -- Main algorithm to denoise a map using non-local means {{{
__host__
std::tuple<Map, float> Denoiser::nlmeans_denoiser(
    Map& map, const float& p_thresh, const float& r_env
) {
    // Construct some needed aliases
    const int& Ne = map.get_volume(); // -- Number of environments (points) in the map
    const int& No = Octanct::No;      // -- Number of octancts in an env (8)
    const int& Nr = Octanct::Nr;      // -- Number of rotations per comp (10)

    // Generate a copy of the map to denoise it
    Map denoised_map = map;

    // Pointers to the denoised map and original map memory blocks
    float* denoised_M = denoised_map.data();
    float* original_M = map.data();

    // Block of memory containing all environments and their averages
    const float* envs = table_of_envs(map, r_env);

    // Table containing the rotated indices for each needed rotation
    const octanct* rots = Octanct::table_of_rotations();

    // Get all average environments of the map using envs -- std::vector
    const auto env_avg   = table_of_stats(map, envs, r_env);

    // Get the maximum and minimum environment average
    const auto min = std::min_element(env_avg.begin(), env_avg.end());
    const auto max = std::max_element(env_avg.begin(), env_avg.end());

    // Calculate the denoising parameter using the threshold provided
    const float hd      = 0.5 * p_thresh * (*max - *min);
    const float inv_den = 1 / (2 * hd * hd);

    // Generate the device copies of the relevant objects
    float* d_omap; float* d_dmap; float* d_envs; 
    octanct* d_rots; float* d_sumk; float* d_dsq;

    // Allocate some memory for the needed objects
    cudaMalloc(&d_omap, Ne * sizeof(float));        // -- Original map
    cudaMalloc(&d_dmap, Ne * sizeof(float));        // -- Denoised map
    cudaMalloc(&d_sumk, Ne * sizeof(float));        // -- Sum of kernels
    cudaMalloc(&d_envs, Ne * No * sizeof(float));   // -- Environments in the map
    cudaMalloc(&d_rots, Nr * No * sizeof(octanct)); // -- Table of rotations
    cudaMalloc(&d_dsq,  Ne * Nr * sizeof(float));   // -- Distance squared values

    // Copy the original map, the environments and the table of rotations
    cudaMemcpy(d_envs, envs,       Ne * No * sizeof(float),   cudaMemcpyHostToDevice);
    cudaMemcpy(d_omap, original_M, Ne * sizeof(float),        cudaMemcpyHostToDevice);
    cudaMemcpy(d_rots, rots,       Nr * No * sizeof(octanct), cudaMemcpyHostToDevice);

    // Set the denoised map and the sum of kernels to zero
    cudaMemset(d_dmap, 0.0f, Ne * sizeof(float));
    cudaMemset(d_sumk, 0.0f, Ne * sizeof(float));

    // Iterate through all reference environments in the map
    for (int er = 0; er < Ne; er++) {

        // Generate the geometry of the blocks to compute the distance squared
        int T_dsq = 160;
        int B_dsq = (Ne - er) / 2 + 1;

        // Generate the geometry of the blocks to update the denoiser
        int T_den = 128;
        int B_den = (Ne - er) / T_den + 1;

        // Set enough memory in d_dsq to zero to compute the new distances
        cudaMemset(d_dsq, 0.0f, (Ne - er) * Nr * sizeof(float));

        // Calculate all possible distance squared in parallel
        calculate_dsq<<<B_dsq, T_dsq>>>(d_dsq, d_envs, d_rots, er, Ne, Nr, No);

        // Update the denoised map and the sum of kernels
        update_denoiser<<<B_den, T_den>>>(
            d_dmap, d_sumk, d_dsq, d_omap, er, Ne, Nr, inv_den
        );
    } // -- End of the denoiser loop

    // Host allocated version of the sum of kernels
    float* sum_kernels = new float[Ne];

    // Copy the sum of kernels and the denoised map to the host
    cudaMemcpy(denoised_M,  d_dmap, Ne * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(sum_kernels, d_sumk, Ne * sizeof(float), cudaMemcpyDeviceToHost);

    // Normalise the data using the sum of kernels
    for (int er = 0; er < Ne; er++) {
        denoised_M[er] = denoised_M[er] / sum_kernels[er];
    }

    // Delete the heap allocated data
    delete[] envs;
    delete[] rots;
    delete[] sum_kernels;

    // Delete the device allocated data
    cudaFree(d_omap);
    cudaFree(d_dmap);
    cudaFree(d_envs);
    cudaFree(d_rots);
    cudaFree(d_sumk);

    // Return a tuple containing the denoised map and the denoised parameter
    return std::make_tuple(denoised_map, hd);
}
// -- }}}
