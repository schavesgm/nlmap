#pragma once

#include <tuple>
#include <iostream>
#include <vector>
#include <cmath>

// -- Some external libraries
#include <gemmi/ccp4.hpp>
#include <gemmi/unitcell.hpp>

// -- User defined libraries
#include "octanct.hpp"
#include "Map.hpp"

// Some definitions to clean the code
#define grid_point gemmi::GridBase<float>::Point

// Use the standard library vector
using std::vector;

// Namespace containing all relevant functions to denoise maps
namespace Denoiser
{
    // -- Basic function used for denoising {{{
    std::tuple<Map, float, vector<float>> nlmeans_denoiser(Map&, const float&, const float&);
    // -- }}}

    // -- Functions to precalculate needed tables for the denoiser {{{
    float* table_of_envs(Map&, const float&);
    vector<grid_point> table_of_indices(Map&, const float&);
    // -- }}}

    // -- Construct the environment around each point {{{
    vector<float> construct_environment(const Map&, const int&, const int&, const int&, const vector<grid_point>&);
    vector<float> average_environment(const vector<float>*, const float&);
    // -- }}}

    // -- Construct a table containing the average of each environment {{{
    vector<float> table_of_envavg(Map&, const float&);
    vector<float> table_of_envavg(Map&, const float*);
    // -- }}}
    
    // -- Construct a table containing the standard deviation of each environment {{{
    vector<float> table_of_envstd(Map&, const float*);
    // -- }}}
    
    // -- Compare environments by computing the kernels and the denoised map value {{{
    void compute_kernels(
        float*, const float*, const octanct*, const int&, const int&, 
        const float&, const vector<float>&, const vector<float>&
    );

    float compute_uhat(const float*, const float*, const int&);
    // -- }}}
};
