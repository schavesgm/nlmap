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
    std::tuple<Map, double, vector<float>> nlmeans_denoiser(Map&, const float&, const double&);
    // -- }}}

    // -- Functions to precalculate needed tables for the denoiser {{{
    vector<float>* table_of_envs_vector(Map&, const double&);
    float* table_of_envs_ptr(Map&, const double&);
    vector<grid_point> table_of_near_indices(Map&, const double&);
    // -- }}}

    // -- Construct the environment around each point {{{
    vector<float> construct_environment(const Map&, const int&, const int&, const int&, const vector<grid_point>&);
    vector<float> average_environment(const vector<float>*, const float&);
    // -- }}}

    // -- Construct the table of averaged environments for of the map {{{
    vector<float> table_of_avg_envs(Map&, const double&);
    vector<float> table_of_avg_envs(Map&, const float*);
    vector<float> table_of_avg_envs(Map&, const vector<float>*);
    // -- }}}
    
    // -- Compare two environments {{{
    void compare_envs(float*, const float*, const int&, const int&, const octanct*);
    void compute_kernel(float*, const float*, const int&, const double&);
    float compute_uhat(const float*, const float*, const int&);
    // -- }}}
};
