#pragma once
// -- Collection of CUDA functions used by the denoiser. This translation unit 
// -- will be included in the denoiser and compiled therein.

#include <cmath>
#include <iostream>

// Include some user defined modules
#include "octanct.hpp"

namespace Cudenoiser
{
    // Function to calculate the distance squared values for a given reference env
    __global__ void calculate_dsq(
        float*, float*, const octanct*, const int, const int, const int, const int
    );

    // Function to update the denoised map values and the sum of kernels for a given reference
    __global__ void update_denoiser(
        float*, float*, float*, float*, const int, const int, const int, const float 
    );
};
