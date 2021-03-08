#pragma once

#include <vector>
#include <algorithm>

#define byte unsigned char
#define vec_q std::vector<float>

namespace Quadrant {

    // Total number of cuadrants
    static const byte NQ = 8;

    // Rotate around the w axis a multiple of 90 degrees
    byte rotate_w(const byte& q, int n);

    // Rotate around the v axis a multiple of 90 degrees
    byte rotate_v(const byte& q, int n);

    // Rotate around the u axis a multiple of 90 degrees
    byte rotate_u(const byte& q, int n);

    // Compare two averaged volumes in all relevant angles
    float compare_quadrants(const vec_q&, const vec_q&);
};
