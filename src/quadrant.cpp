#include <quadrant.hpp>

// Rotate around the w axis a multiple of 90 degrees
byte Quadrant::rotate_w(const byte& q, int n)
{
    // Restrict n between 0 and 3
    n = n % 4;

    // Variables that will hold the binary result
    byte u, v;
        
    // W is constant in this rotation
    const byte w = ((q >> 2) & 1);

    if (n == 1) {        // Rotate by 90 degrees
        u = ((~q >> 1) & 1);
        v = ((q >> 0)  & 1);
    } else if (n == 2) { // Rotate by 180 degrees
        u = ((~q >> 0) & 1);
        v = ((~q >> 1) & 1);
    } else if (n == 3) { // Rotate by 270 degrees
        u = ((q >> 1)  & 1);
        v = ((~q >> 0) & 1);
    } else  {            // Rotate by 0 degrees
        u = ((q >> 0)  & 1);
        v = ((q >> 1)  & 1);
    }
    return (4 * w) | (2 * v) | (1 * u);
}

// Rotate around the v axis a multiple of 90 degrees
byte Quadrant::rotate_v(const byte& q, int n)
{
    // Restrict n between 0 and 3
    n = n % 4;

    // Variables that will hold the binary result
    byte u, w;
        
    // W is constant in this rotation
    const byte v = ((q >> 1) & 1);

    if (n == 1) {        // Rotate by 90 degrees
        u = ((q >> 2)  & 1);
        w = ((~q >> 0) & 1);
    } else if (n == 2) { // Rotate by 180 degrees
        u = ((~q >> 0) & 1);
        w = ((~q >> 2) & 1);
    } else if (n == 3) { // Rotate by 270 degrees
        u = ((~q >> 2) & 1);
        w = ((q >> 0)  & 1);
    } else {             // Rotate by 0 degrees
        u = ((q >> 0)  & 1);
        w = ((q >> 2)  & 1);
    }
    return (4 * w) | (2 * v) | (1 * u);
}

// Rotate around the u axis a multiple of 90 degrees
byte Quadrant::rotate_u(const byte& q, int n)
{
    // Restrict n between 0 and 3
    n = n % 4;

    // Variables that will hold the binary result
    byte v, w;
        
    // W is constant in this rotation
    const byte u = ((q >> 0) & 1);

    if (n == 1) {        // Rotate by 90 degrees
        v = ((~q >> 2) & 1);
        w = ((q  >> 1) & 1);
    } else if (n == 2) { // Rotate by 180 degrees
        v = ((~q >> 1) & 1);
        w = ((~q >> 2) & 1);
    } else if (n == 3) { // Rotate by 270 degrees
        v = ((q  >> 2) & 1);
        w = ((~q >> 1) & 1);
    } else {             // Rotate by 0 degrees
        v = ((q >> 1)  & 1);
        w = ((q >> 2)  & 1);
    }
    return (4 * w) | (2 * v) | (1 * u);
}

// Some inline functions to clean the loops {{{
inline float compare_unrot(const vec_q& refr, const vec_q& comp) 
{
    // Distance square variable
    float d_sq = 0.0f;

    // Compare the unrotated quadrants
    for (byte q = 0; q < Quadrant::NQ; q++) {

        // References to the values in the quadrants
        auto& r = refr[q]; auto& c = comp[q];

        d_sq += (r - c) * (r - c);
    }

    return d_sq / Quadrant::NQ;
}

inline float compare_rot_w(
    const vec_q& refr, const vec_q& comp, const int& multiple
) {
    // Distance square variable
    float d_sq = 0.0f;

    // Compare the unrotated quadrants
    for (byte q = 0; q < Quadrant::NQ; q++) {

        // References to the values in the quadrants
        auto& r = refr[q];
        auto& c = comp[Quadrant::rotate_w(q, multiple)];

        d_sq += (r - c) * (r - c);
    }

    return d_sq / Quadrant::NQ;
}

inline float compare_rot_v(
    const vec_q& refr, const vec_q& comp, const int& multiple
) {
    // Distance square variable
    float d_sq = 0.0f;

    // Compare the unrotated quadrants
    for (byte q = 0; q < Quadrant::NQ; q++) {

        // References to the values in the quadrants
        auto& r = refr[q];
        auto& c = comp[Quadrant::rotate_v(q, multiple)];

        d_sq += (r - c) * (r - c);
    }

    return d_sq / Quadrant::NQ;
}

inline float compare_rot_u(
    const vec_q& refr, const vec_q& comp, const int& multiple
) {
    // Distance square variable
    float d_sq = 0.0f;

    // Compare the unrotated quadrants
    for (byte q = 0; q < Quadrant::NQ; q++) {

        // References to the values in the quadrants
        auto& r = refr[q];
        auto& c = comp[Quadrant::rotate_u(q, multiple)];

        d_sq += (r - c) * (r - c);
    }

    return d_sq / Quadrant::NQ;
}
// }}}


// Compare two volumes in all possible relevant angles
float Quadrant::compare_quadrants(const vec_q& refr, const vec_q& comp)
{
    // Minimum distance to return
    float min_d_sq = 340282346638528859811704183484516925440.0f;

    // Update the minimum distance
    min_d_sq = std::min(min_d_sq, compare_unrot(refr, comp));

    // Perform all rotations in each axis
    for (int rot = 1; rot < 4; rot++) {

        // Compare rotating in the w axis
        min_d_sq = std::min(min_d_sq, compare_rot_w(refr, comp, rot));

        // Compare rotation in the v axis
        min_d_sq = std::min(min_d_sq, compare_rot_v(refr, comp, rot));
        
        // Compare rotation in the u axis
        min_d_sq = std::min(min_d_sq, compare_rot_u(refr, comp, rot));
    }

    return min_d_sq;
}
