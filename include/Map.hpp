#pragma once

// Standard libraries
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <numeric>

// Include gemmi libraries
#include <gemmi/ccp4.hpp>
#include <gemmi/unitcell.hpp>

// Include user-defined libraries
#include "quadrant.hpp"

using namespace gemmi;

struct Map : public gemmi::Ccp4<float>
{
    // Constructors and destructors
    Map()  = default; ~Map() = default;
    Map(const std::string&, const float& = 0.0f);
    Map(const Map&);
    void operator=(const Map&);

    // -- Inmutable properties of the map {{{
    // -- Dimensions of the grid
    const int get_Nu() const;
    const int get_Nv() const;
    const int get_Nw() const;
    const int get_volume()  const;
    
    // -- Properties of the unit cell
    const float get_a() const;     // (a, 0, 0, 0, 0, 0)
    const float get_b() const;     // (0, b, 0, 0, 0, 0)
    const float get_c() const;     // (0, 0, c, 0, 0, 0)
    const float get_alpha() const; // (0, 0, 0, alpha, 0, 0)
    const float get_beta() const;  // (0, 0, 0, 0, beta, 0)
    const float get_gamma() const; // (0, 0, 0, 0, 0, gamma)

    // -- Mutable properties of the map
    float max_value() const;
    float min_value() const;
    float avg_value() const;

    // -- Real space coordinate treatment {{{
    Position get_position(const int&, const int&, const int&) const;
    Fractional get_fractional(const int&, const int&, const int&) const;
    // }}}

    // -- Accessors to the data in the grid {{{
    float get_value(const int&, const int&, const int&) const;
    float& get_value(const int&, const int&, const int&);

    float operator[](const int&) const;
    float& operator[](const int&);
    // }}}

    // -- Setters for the data in the grid
    void set_value(const int&, const int&, const int&, const float&);
    void set_data(const std::vector<float>&, const bool& = false);

    // -- Global modifications of the map
    void normalise();
    float add_noise(const float& = 1.0, const bool& = false);

    // -- Denoiser methods
    Map nlmeans_denoise(const float&, const double&);

    // Table containing the nearest indices in the lattice
    std::vector<gemmi::GridBase<float>::Point> table_of_near_indices(const double&);

    // Precalculate all quadrants in the lattice
    vec_q* table_of_quadrants(const double&);
    std::vector<float> table_of_avg_quadrants(const vec_q*);

    // Get all quadrants around a given point in the grid
    vec_q get_quadrants(const int&, const int&, const int&, const std::vector<gemmi::GridBase<float>::Point>&);

    // -- Save the current status of the map into a file
    void save_map(const std::string&);

    // -- Fields of the class {{{ 
    const int& Nu = grid.nu; 
    const int& Nv = grid.nv;
    const int& Nw = grid.nw;

    const double& a     = grid.unit_cell.a;
    const double& b     = grid.unit_cell.b;
    const double& c     = grid.unit_cell.c;
    const double& alpha = grid.unit_cell.alpha;
    const double& beta  = grid.unit_cell.beta;
    const double& gamma = grid.unit_cell.gamma;

    // Field containing the h_value for the denoiser
    float hd = 0.0f;
    // -- }}}

};
