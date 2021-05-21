#include <cudenoiser.hpp>

__forceinline__ __device__
int __get_oct(const int& t, const int& No)
{
    // -- Obtain the corresponding octanct for a given thread using the 2^n 
    // -- optimised modulo operator. The calculation is equivalent as t % No
    return t & (No - 1);
}

__forceinline__ __device__
int __get_rot(const int& t, const int& Nr, const int& No)
{
    // -- Obtain the corresponding rotation for a given thread using. Each block
    // -- contains 160 threads, so there are 2 comparisons of 10 rotations each
    return (t >= Nr * No) ? (t - Nr * No) / No : t / No;
}

__forceinline__ __device__
int __get_idc(const int& b, const int& t, const int& Nr, const int& No)
{
    // -- Obtain the corresponding index of comparison for the given thread and 
    // -- the given block. For each block, there are 2 comparisons.
    return b * 2 + (t >= Nr * No);
}

__global__
void Cudenoiser::calculate_dsq(
    float* dsq, float* envs, const octanct* rots, const int er, 
    const int Ne, const int Nr, const int No
) {
    // -- Calculate all distance squared for all possible rotations using er as
    // -- the reference environment and ec = [er, Ne] as the comparison environments.
    // -- Each thread in the calculation will performn one comparison for one octact,
    // -- rotation and comparison environment. It will be stored in the block of
    // --  memory dsq with dimensions (Ne - er, Nr)

    // Obtain the global index of the current thread
    const int gidx = blockIdx.x * blockDim.x + threadIdx.x;

    // Only use the threads that are inside bounds (Ne - er) * Nr * No
    if (gidx < (Ne - er) * Nr * No) {

        // Calculate the corresponding octact for the current thread
        const int oct = __get_oct(threadIdx.x, No);

        // Calculate the corresponding rotation for the current thread
        const int rot = __get_rot(threadIdx.x, Nr, No);

        // Calculate the comparison index for the current thread
        const int idc = __get_idc(blockIdx.x, threadIdx.x, Nr, No);

        // Compute the comparison environment ec = er + idc
        const int ec = er + idc;

        // Get a reference to the rotated reference octanct and comparison
        const float& r_oct = envs[er * No + rots[rot * No + oct]];
        const float& c_oct = envs[ec * No + oct];

        // Compute the octanct different between reference and comparison
        const float diff = r_oct - c_oct;

        // Atomic add the difference to the correct location
        atomicAdd(dsq + idc * Nr + rot, (diff * diff) / No);
    }
}

__global__
void Cudenoiser::update_denoiser(
    float* dmap, float* kernels, float* d_squared, float* omap,
    const int er, const int Ne, const int Nr, const float inv_den
) {
    // -- Update the denoised map and the sum of kernels using uhat and kernel 
    // -- at both relevant positions (er and ec). The kernel is computed by first
    // -- finding the minimum distance squared in the matrix of dimensions 
    // -- (Ne - er, Nr) for each row. Each thread in the calculation will find the
    // -- minimum value of each different row and then compute the kernel and u_hat,
    // --  kernel = exp(-min_dsq * inv_den), u_hat[e] = kernel[e] * map_values[e]

    // Get the global index of the current thread -- Corresponds to idc (previously)
    const int gidx = blockIdx.x * blockDim.x + threadIdx.x;

    // Check if the current thread is inside the bounds
    if (gidx < (Ne - er)) {

        // Generate a temporary variable to find the minimum distance squared
        float min_dsq = d_squared[gidx * Nr + 0];

        // Update the minimum value if needed
        for (int r = 1; r < Nr; r++) {
            if (min_dsq > d_squared[gidx * Nr + r]) {
                min_dsq = d_squared[gidx * Nr + r];
            }
        }

        // Compute the kernel using the minimum distance
        float kernel = expf(- min_dsq * inv_den);

        // Update the denoised map in the correct locations
        atomicAdd(dmap + gidx + er, kernel * omap[er]);
        atomicAdd(dmap + er,        kernel * omap[gidx + er]);

        // Add the kernels to the correct locations
        atomicAdd(kernels + gidx + er, kernel);
        atomicAdd(kernels + er,        kernel);
    }
}
