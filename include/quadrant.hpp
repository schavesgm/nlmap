#pragma once

#include <vector>
#include <algorithm>

#define byte unsigned char
#define vec_q std::vector<float>

namespace Quadrant {

    // Total number of cuadrants
    static const byte NQ = 8;

    // Names for the quadrants
    static const byte q0 = 0b000;
    static const byte q1 = 0b001;
    static const byte q2 = 0b010;
    static const byte q3 = 0b011;
    static const byte q4 = 0b100;
    static const byte q5 = 0b101;
    static const byte q6 = 0b110;
    static const byte q7 = 0b111;

    // Rotate around the w axis a multiple of 90 degrees
    byte rotate_w(const byte& q, int n);

    // Rotate around the v axis a multiple of 90 degrees
    byte rotate_v(const byte& q, int n);

    // Rotate around the u axis a multiple of 90 degrees
    byte rotate_u(const byte& q, int n);

    // Compare two averaged volumes in all relevant angles
    float compare_quadrants(const vec_q&, const vec_q&);
};
