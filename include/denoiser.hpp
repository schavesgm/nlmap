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
#include "stats.hpp"

// Some definitions to clean the code
#define grid_point gemmi::GridBase<float>::Point

// Use the standard library vector
using std::vector;

// Namespace containing all relevant functions to denoise maps
namespace Denoiser
{
    // -- Basic function used for denoising {{{
    std::tuple<Map, float, vector<float>, vector<float>> 
        nlmeans_denoiser(Map&, const float&, const float&, const float&);
    // -- }}}

    // -- Indices of the grid whose distance to a central point is less than a given one {{{
    vector<grid_point> table_of_indices(Map&, const float&);
    // -- }}}

    // -- Calculate the environments, their averages and standard deviation {{{
    float* table_of_envs(Map&, const float&);
    // -- }}}

    // -- Construct the environment around each point {{{
    vector<float> get_env(const Map&, const int&, const int&, const int&, const vector<grid_point>&);
    vector<float> avg_env(const vector<float>*);
    // -- }}}

    // -- Construct a table containing the average of each environment {{{
    vector<float> table_of_stats(Map&, const float&);
    // -- }}}
};
