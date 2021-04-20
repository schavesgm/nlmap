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

// -- Main algorithm to denoise a map using non-local means {{{
Map Denoiser::nlmeans_denoiser(Map& map, const float& p_thresh, const double& R_max)
{
    // Construct some needed aliases
    const int& Ne = map.get_volume();  // -- Number of environments in the map
    const int& No = Octanct::No;       // -- Number of octancts in an env (8)
    const int& Nr = Octanct::Nr;       // -- Number of rotations per comp (10)

    // Generate a copy of the map to denoise it
    Map denoised_map = map;

    // Generate a table containing all the environments
    const float* table_envs = Denoiser::table_of_envs_ptr(map, R_max);

    // Generate a table containing all averaged quadrants
    const vector<float> table_avg = table_of_avg_envs(map, table_envs);

    // Get the maximum and minimum environment average
    const auto min = std::min_element(table_avg.begin(), table_avg.end());
    const auto max = std::max_element(table_avg.begin(), table_avg.end());

    // Calculate the denoising parameter using the threshold provided
    const float hd = p_thresh * (*max - *min);

#ifdef __NVCC__
    
    // Move the table of environments to the CUDA device
    float* d_table_envs;
    cudaMalloc(&d_table_envs, Ne * No * sizeof(float));
    cudaMemcpy(d_table_envs, table_envs, Ne * No * sizeof(float), cudaMemcpyHostToDevice);

    // Allocate some memory for the comparison results
    __device__ float d_table_comp[Ne * Nr];
    for (int c = 0; c < Ne * Nr ; c++) d_table_comp[c] = 0.0f;

    // Generate a table of possible rotations and move it to device
    octanct* table_rots = Octanct::table_of_rotations(); 
    octanct* d_table_rots; 
    cudaMalloc(&d_table_rots, Nr * No * sizeof(octanct));
    cudaMemcpy(d_table_rots, table_rots, Nr * No * sizeof(octanct), cudaMemcpyHostToDevice);

    // Define the CUDA grids and number of threads per blocks
    dim3 grid_dim  = dim3(Ne, Nr);
    dim3 block_dim = dim3(No);

    // Wrap the CUDA comparison pointer with a thrust pointer
    thrust::device_ptr<float> t_comp = thrust::device_pointer_cast(d_table_comp);

    // Loop for all reference environment to generate the denoised version
    for (int re = 0; re < Ne; re++) {

        // Fill the comparison matrix with the current comparison
        compare_environments<<<grid_dim, block_dim>>>(comp, envs, d_rots, re);
        cudaDeviceSynchronize();

        for (int ce = 0; ce < Ne; ce++) {

            // Sort the data to obtain the minimum distance for this comparison
            auto min_ptr = thrust::min_element(
                t_comp + Nr * ce, t_comp + Nr * (ce + 1) - 1
            );
        }

        // Fill the comparison pointer with zeroes
        thrust::fill(comp, comp + Ne * Nr, 0.0f);
    }

    // Free the CUDA allocated memory
    cudaFree(d_table_envs);
    cudaFree(d_table_comp);

    // Delete the heap allocated data
    delete[] rots;

    std::cout << "Compiled using CUDA\n";
#else
    std::cout << "Not compiled using CUDA\n";
#endif

    // // Iterate for each value in the grid
    // for (int wr = 0; wr < this->Nw; wr++) {
    // for (int vr = 0; vr < this->Nv; vr++) {
    // for (int ur = 0; ur < this->Nu; ur++) { 
    //     // Obtain the linear index in the grid for (ur, vr, wr)
    //     const auto ir = size_t(wr * Nv + vr) * Nu + ur;

    //     // New denoised value of the map at (ur, vr, wr)
    //     float m_hat = 0.0f;

    //     // Maximum value of the kernel and normalisation constant
    //     float sum_kernels = 0.0f;

    //     // Iterate for all other comparison environments in the grid
    //     for (int wc = 0; wc < this->Nw; wc++) {
    //     for (int vc = 0; vc < this->Nv; vc++) {
    //     for (int uc = 0; uc < this->Nu; uc++) {

    //         // Obtain the linear index in the grid for (uc, vc, wc)
    //         const auto ic = size_t(wc * Nv + vc) * Nu + uc;

    //         // Compare the two quadrants
    //         const float min_dsq = Quadrant::compare_quadrants(
    //             table_q[ir], table_q[ic]
    //         );

    //         // Using the distance, obtain the denoising kernel
    //         const float kernel = std::exp(- min_dsq / (2 * hd * hd));

    //         // Update the denoised value
    //         m_hat += kernel * grid.data[ic];

    //         // Append the kernel to the normalising constant
    //         sum_kernels += kernel;

    //     } // -- End of the main comparison uc iteration
    //     } // -- End of the main comparison vc iteration
    //     } // -- End of the main comparison wc iteration

    //     // Change the pixel value with the denoised version
    //     denoised_map.set_value(ur, vr, wr, m_hat / sum_kernels);

    // } // -- End of the main reference ur iteration
    // } // -- End of the main reference vr iteration
    // } // -- End of the main reference wr iteration

    // // Set the denoising parameter used as field in denoised_map
    // denoised_map.hd = hd;

    // // Delete the heap allocated data
    // delete[] table_q;

    return denoised_map;

}
// -- }}}

// -- CUDA kernel used to compute the comparisons on the GPU {{{
#ifdef __NVCC__
__global__
void compare_environments(float* comp, float* envs, octanct* rots, int ref_idx)
{
    // Select the environment and octanct index for the current thread
    int gidx = blockIdx.x * blockDim.x + threadIdx.x;

    // Select the rotated reference octanct index for the current thread
    int ridx = ref_idx * blockDim.x + rots[blockIdx.y * blockDim.x + threadIdx.x];

    // Select the environment and rotation index to output the data
    int oidx = blockIdx.x * gridDim.y + blockIdx.y;

    // Calculate the difference of environments using the calculated indices
    float diff = envs[ridx] - envs[gidx];

    // Update the comparison using the table of rotations
    atomicAdd(comp[oidx], diff * diff);

    // Synchronize the threads
    __syncthreads();
}

__global__
void compute_denoise(float* denoised, thrust::device_ptr<float> comp, float h)
{

}
#endif
// }}}
