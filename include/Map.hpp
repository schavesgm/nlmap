#pragma once

// Standard libraries
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <numeric>
#include <fstream>

// Include gemmi libraries
#include <gemmi/ccp4.hpp>
#include <gemmi/unitcell.hpp>

using namespace gemmi;

struct Map : public gemmi::Ccp4<float>
{
    // -- Constructors and destructors
    Map()  = default; ~Map() = default;
    Map(const std::string&, const float& = 0.0f);
    Map(const Map&);
    void operator=(const Map&);

    // -- Dimensions of the grid
    int get_Nu() const;
    int get_Nv() const;
    int get_Nw() const;
    int get_volume()  const;
    
    // -- Properties of the unit cell
    float get_a() const;     // (a, 0, 0, 0, 0, 0)
    float get_b() const;     // (0, b, 0, 0, 0, 0)
    float get_c() const;     // (0, 0, c, 0, 0, 0)
    float get_alpha() const; // (0, 0, 0, alpha, 0, 0)
    float get_beta() const;  // (0, 0, 0, 0, beta, 0)
    float get_gamma() const; // (0, 0, 0, 0, 0, gamma)

    // -- Mutable properties of the map
    float max_value() const;
    float min_value() const;
    float avg_value() const;

    // -- Real space coordinate treatment
    Position get_position(const int&, const int&, const int&) const;
    Fractional get_fractional(const int&, const int&, const int&) const;

    // -- Accessors to the data in the grid
    float* data();
    float get_value(const int&, const int&, const int&) const;
    float& get_value(const int&, const int&, const int&);
    float operator[](const int&) const;
    float& operator[](const int&);

    // -- Setters for the data in the grid
    void set_value(const int&, const int&, const int&, const float&);
    void set_value(const int&, const float&);
    void set_data(const std::vector<float>&, const bool& = false);
    void set_data(const float&, const bool& = false);

    // -- Global modifications of the map
    void normalise();
    float add_noise(const float& = 1.0, const bool& = false);

    // Save the map into a file
    void save_map(const std::string&);

    // -- Fields of the class used to access important data {{{ 
    // Dimensions of the map
    const int& Nu = grid.nu; 
    const int& Nv = grid.nv;
    const int& Nw = grid.nw;

    // Dimensions of the unit cell represented by the map
    const double& a     = grid.unit_cell.a;
    const double& b     = grid.unit_cell.b;
    const double& c     = grid.unit_cell.c;
    const double& alpha = grid.unit_cell.alpha;
    const double& beta  = grid.unit_cell.beta;
    const double& gamma = grid.unit_cell.gamma;

    // Lattice spacing in each direction
    const double& a_u = grid.spacing[0];
    const double& a_v = grid.spacing[1];
    const double& a_w = grid.spacing[2];
    // -- }}}
};
