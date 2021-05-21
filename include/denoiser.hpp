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
#include "cudenoiser.hpp"

// Some definitions to clean the code
#define grid_point gemmi::GridBase<float>::Point

// Use the standard library vector
using std::vector;

// Use the namespace of Cudenoiser functions
using namespace Cudenoiser;

// Namespace containing all relevant functions to denoise maps
namespace Denoiser
{
    // -- Basic function used for denoising {{{
    std::tuple<Map, float> nlmeans_denoiser(Map&, const float&, const float&);
    // -- }}}

    // -- Indices of the grid whose distance to a central point is less than a given one {{{
    vector<grid_point> table_of_indices(Map&, const float&);
    // -- }}}

    // -- Calculate the environments, their averages and standard deviation {{{
    float* table_of_envs(Map&, const float&);
    // -- }}}

    // -- Assign all points in the environment to the correct octanct {{{
    vector<float>* get_octs(const Map&, const int&, const int&, const int&, const vector<grid_point>&);
    int avg_points_per_octanct(Map&, const float&);
    // -- }}}

    // -- Construct a table containing the average of each environment {{{
    vector<float> table_of_stats(Map&, const float&);
    // -- }}}
};
