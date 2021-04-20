#include <denoiser.hpp>

// -- Compute a table containing all possible octancts in the map {{{
vector<float>* Denoiser::table_of_envs_vector(Map& map, const double& R_max)
{
    // First, obtain a table of near indices
    const auto indices = table_of_near_indices(map, R_max);

    // Allocate memory for all octancts in the grid
    vector<float>* table_of_envs = new vector<float>[map.get_volume()];

    // Count the corresponding environment for each point
    int env_idx = 0;

    // iterate for each point in the grid to obtain its environment
    for (int w = 0; w < map.Nw; w++) {
        for (int v = 0; v < map.Nv; v++) {
            for (int u = 0; u < map.Nu; u++) {

                // Add the environment to the table
                table_of_envs[env_idx] = construct_environment(map, u, v, w, indices);

                // Move to the next environment in the grid
                env_idx++;
            }
        }
    }

    // Return the table of environments
    return table_of_envs;
}

float* Denoiser::table_of_envs_ptr(Map& map, const double& R_max)
{
    // First, obtain a table of near indices
    const auto indices = table_of_near_indices(map, R_max);

    // Allocate memory for all octancts in the grid
    float* table_of_envs = new float[map.get_volume() * Octanct::No];

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
                    const int ieo = env_idx * Octanct::No + o;

                    // Copy the data into the memory
                    table_of_envs[ieo] = env[o];
                }

                // Move to the next environment in the grid
                env_idx++;
            }
        }
    }

    // Return the table of environments
    return table_of_envs;
}
// -- }}}

// -- Get all points whose distance is less or equal to a given distance {{{
vector<grid_point> Denoiser::table_of_near_indices(Map& map, const double& R_max) 
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
    const Map& map, const int& u, const int& v, const int& w, const vector<grid_point>& table_of_indices
) {
    // Vector that will contain the environment values separated into octancts
    vector<float> raw_environment[Octanct::No];

    // Get the map value for (u, v, w)
    const float m_central = map.get_value(u, v, w);

    // Iterate for all points in the table
    for (auto& p : table_of_indices) {

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
vector<float> Denoiser::average_environment(
    const vector<float>* raw_environment, const float& m_central) 
{
    // Generate the vector containing the average in each quadrant
    vector<float> environment(Octanct::No);

    for (octanct o = 0; o < Octanct::No; o++) {
        // Calculate the average for each octanct adding the central
        if (o != Octanct::o7) {
            environment[o] = std::accumulate(
                raw_environment[o].begin(), raw_environment[o].end(), m_central
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
vector<float> Denoiser::table_of_avg_envs(Map& map, const double& R_max)
{
    // Allocate memory for the table of averaged environments
    vector<float> avg_envs(map.get_volume());

    // Construct the environments for the given map
    auto table_of_envs = Denoiser::table_of_envs_vector(map, R_max);

    // Build the average for each environment
    for (int e = 0; e < map.get_volume(); e++) {
        avg_envs[e] = std::accumulate(
            table_of_envs[e].begin(), table_of_envs[e].end(), 0.0f
        ) / table_of_envs[e].size();
    }

    // Delete the heap allocated memory
    delete[] table_of_envs;

    return avg_envs;
}

vector<float> Denoiser::table_of_avg_envs(Map& map, const float* table_of_envs)
{
    // Allocate memory for the table of averaged environments
    vector<float> avg_envs(map.get_volume());

    // Build the average for each environment
    for (int e = 0; e < map.get_volume(); e++) {

        // Temporary variable to calculate the environments sum
        float env_sum = 0.0f;

        for (octanct o = Octanct::o0; o < Octanct::No; o++) {
            env_sum += table_of_envs[e * Octanct::No + o];
        }

        // Calculate the average
        avg_envs[e] = env_sum / Octanct::No;
    }

    return avg_envs;
}

vector<float> Denoiser::table_of_avg_envs(Map& map, const vector<float>* table_of_envs)
{
    // Allocate memory for the table of averaged environments
    vector<float> avg_envs(map.get_volume());

    // Build the average for each environment
    for (int e = 0; e < map.get_volume(); e++) {

        // Temporary variable to calculate the environments sum
        float env_sum = 0.0f;

        // Calculate the average
        avg_envs[e] = std::accumulate(
            table_of_envs[e].begin(), table_of_envs[e].end(), 0.0f
        ) / Octanct::No;
    }

    return avg_envs;
}
// -- }}}

// -- Function to compare two environments included in a table {{{
void Denoiser::compare_envs( 
    float* mdsq, const float* envs, const int& er, const int& Ne, const octanct* rots
) {
    // Get the total number of rotations and octancts
    const int& Nr = Octanct::Nr;
    const int& No = Octanct::No;

    // Loop over all possible environments contained in envs
    for (int e = 0; e < Ne; e++) {

        // Memory that will contain the 10 comparisons
        float comps[Nr] = {};

        // Compare the environments for each rotation
        for (int r = 0; r < Nr; r++) {
            for (int o = 0; o < No; o++) {

                // Obtain the rotated reference environment and the comparison octanct
                const float refr = envs[er * No + rots[r * No + o]];
                const float comp = envs[e * No + o];

                // Compute the distance squared
                comps[r] += (refr - comp) * (refr - comp);
            }
        }

        // Get the minimum distance squared for all rotations
        float env_mindsq = comps[0];

        // Update the minimum distance
        for (int r = 1; r < Nr; r++) {
            env_mindsq = (env_mindsq > comps[r]) ? comps[r] : env_mindsq;
        }

        // Append the minimum distance to the vector of minimum distances
        mdsq[e] = env_mindsq;
    }
}
// -- }}}

// -- Function to compute the kernels given the distance squared values {{{
void Denoiser::compute_kernel(
    float* kernel, const float* mdsq, const int& Ne, const double& h
) {
    // Loop over all possible environments represented in the memory
    for (int e = 0; e < Ne; e++) {
        // Compute the kernel by exponentiating
        kernel[e] = exp(- mdsq[e] / (2 * h * h));
    }
}
// -- }}}


// -- Function to compute the denoised version of the map value {{{
float Denoiser::compute_uhat(const float* kernel, const float* maps, const int& Ne)
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
std::tuple<Map, double, vector<float>> Denoiser::nlmeans_denoiser(
    Map& map, const float& p_thresh, const double& R_max
) {
    // Construct some needed aliases
    const int& Ne = map.get_volume();  // -- Number of environments in the map
    const int& No = Octanct::No;       // -- Number of octancts in an env (8)
    const int& Nr = Octanct::Nr;       // -- Number of rotations per comp (10)
    const int& Nw = map.Nw;            // -- Number of points in the w direction
    const int& Nv = map.Nv;            // -- Number of points in the v direction
    const int& Nu = map.Nu;            // -- Number of points in the u direction

    // Generate a copy of the map to denoise it
    Map denoised_map = map;

    // Generate a table containing all the environments
    const float* table_envs = table_of_envs_ptr(map, R_max);

    // Generate a table containing all averaged quadrants
    const vector<float> table_avg = table_of_avg_envs(map, table_envs);

    // Generate a table containing the rotated indices for each rotation
    const octanct* table_rots = Octanct::table_of_rotations();

    // Pointer to the map data in memory
    const float* map_data = map.data();

    // Get the maximum and minimum environment average
    const auto min = std::min_element(table_avg.begin(), table_avg.end());
    const auto max = std::max_element(table_avg.begin(), table_avg.end());

    // Calculate the denoising parameter using the threshold provided
    const float hd = p_thresh * (*max - *min);

    // Buffer to store the minimum distance squared and the kernel values
    float* table_mdsq = new float[Ne];
    float* table_kern = new float[Ne];

    // Iterate for each position in the grid
    for (int er = 0; er < Ne; er++) {

        // Get all comparisons using the the current reference environment
        compare_envs(table_mdsq, table_envs, er, Ne, table_rots);

        // Calculate the kernel corresponding to each distance squared
        compute_kernel(table_kern, table_mdsq, Ne, hd);

        // Compute the denoised version of the map at the current location
        const float u_hat = compute_uhat(table_kern, map_data, Ne);

        // Change the map value with the denoised version
        denoised_map.set_value(er, u_hat);
    }

    // Delete the heap allocated data
    delete[] table_mdsq;
    delete[] table_kern;
    delete[] table_envs;
    delete[] table_rots;

    // Return a tuple containing the denoised map and the denoised parameter
    return std::make_tuple(denoised_map, hd, table_avg);
}
// -- }}}
