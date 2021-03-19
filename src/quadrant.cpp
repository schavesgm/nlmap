#include <quadrant.hpp>
#include <iostream>

// Rotate around the w axis a multiple of 90 degrees
// byte Quadrant::rotate_w(const byte& q, int n)
// {
//     // Restrict n between 0 and 3
//     n = n % 4;
// 
//     // Variables that will hold the binary result
//     byte u, v;
//         
//     // W is constant in this rotation
//     const byte w = ((q >> 2) & 1);
// 
//     if (n == 1) {        // Rotate by 90 degrees
//         u = ((~q >> 1) & 1);
//         v = ((q >> 0)  & 1);
//     } else if (n == 2) { // Rotate by 180 degrees
//         u = ((~q >> 0) & 1);
//         v = ((~q >> 1) & 1);
//     } else if (n == 3) { // Rotate by 270 degrees
//         u = ((q >> 1)  & 1);
//         v = ((~q >> 0) & 1);
//     } else  {            // Rotate by 0 degrees
//         u = ((q >> 0)  & 1);
//         v = ((q >> 1)  & 1);
//     }
//     return (4 * w) | (2 * v) | (1 * u);
// }
// 
// // Rotate around the v axis a multiple of 90 degrees
// byte Quadrant::rotate_v(const byte& q, int n)
// {
//     // Restrict n between 0 and 3
//     n = n % 4;
// 
//     // Variables that will hold the binary result
//     byte u, w;
//         
//     // W is constant in this rotation
//     const byte v = ((q >> 1) & 1);
// 
//     if (n == 1) {        // Rotate by 90 degrees
//         u = ((q >> 2)  & 1);
//         w = ((~q >> 0) & 1);
//     } else if (n == 2) { // Rotate by 180 degrees
//         u = ((~q >> 0) & 1);
//         w = ((~q >> 2) & 1);
//     } else if (n == 3) { // Rotate by 270 degrees
//         u = ((~q >> 2) & 1);
//         w = ((q >> 0)  & 1);
//     } else {             // Rotate by 0 degrees
//         u = ((q >> 0)  & 1);
//         w = ((q >> 2)  & 1);
//     }
//     return (4 * w) | (2 * v) | (1 * u);
// }
// 
// // Rotate around the u axis a multiple of 90 degrees
// byte Quadrant::rotate_u(const byte& q, int n)
// {
//     // Restrict n between 0 and 3
//     n = n % 4;
// 
//     // Variables that will hold the binary result
//     byte v, w;
//         
//     // W is constant in this rotation
//     const byte u = ((q >> 0) & 1);
// 
//     if (n == 1) {        // Rotate by 90 degrees
//         v = ((~q >> 2) & 1);
//         w = ((q  >> 1) & 1);
//     } else if (n == 2) { // Rotate by 180 degrees
//         v = ((~q >> 1) & 1);
//         w = ((~q >> 2) & 1);
//     } else if (n == 3) { // Rotate by 270 degrees
//         v = ((q  >> 2) & 1);
//         w = ((~q >> 1) & 1);
//     } else {             // Rotate by 0 degrees
//         v = ((q >> 1)  & 1);
//         w = ((q >> 2)  & 1);
//     }
//     return (4 * w) | (2 * v) | (1 * u);
// }
// 
// inline float compare_rot_w(
//     const vec_q& refr, const vec_q& comp, const int& multiple
// ) {
//     // Distance square variable
//     float d_sq = 0.0f;
// 
//     // Compare the unrotated quadrants
//     for (byte q = 0; q < Quadrant::NQ; q++) {
// 
//         // References to the values in the quadrants
//         auto& r = refr[q];
//         auto& c = comp[Quadrant::rotate_w(q, multiple)];
// 
//         d_sq += (r - c) * (r - c);
//     }
// 
//     return d_sq / Quadrant::NQ;
// }
// 
// inline float compare_rot_v(
//     const vec_q& refr, const vec_q& comp, const int& multiple
// ) {
//     // Distance square variable
//     float d_sq = 0.0f;
// 
//     // Compare the unrotated quadrants
//     for (byte q = 0; q < Quadrant::NQ; q++) {
// 
//         // References to the values in the quadrants
//         auto& r = refr[q];
//         auto& c = comp[Quadrant::rotate_v(q, multiple)];
// 
//         d_sq += (r - c) * (r - c);
//     }
// 
//     return d_sq / Quadrant::NQ;
// }
// 
// inline float compare_rot_u(
//     const vec_q& refr, const vec_q& comp, const int& multiple
// ) {
//     // Distance square variable
//     float d_sq = 0.0f;
// 
//     // Compare the unrotated quadrants
//     for (byte q = 0; q < Quadrant::NQ; q++) {
// 
//         // References to the values in the quadrants
//         auto& r = refr[q];
//         auto& c = comp[Quadrant::rotate_u(q, multiple)];
// 
//         d_sq += (r - c) * (r - c);
//     }
// 
//     return d_sq / Quadrant::NQ;
// }
// }}}

// inline void compare_quads(float& min_dsq, const vec_q& refr, const vec_q& comp, byte* q_idx) 
// {
//     float d_sq = 0.0f;
// 
//     // Distance square between quadrants
//     d_sq += square(refr[0] - comp[q_idx[0]]);
//     d_sq += square(refr[1] - comp[q_idx[1]]);
//     d_sq += square(refr[2] - comp[q_idx[2]]);
//     d_sq += square(refr[3] - comp[q_idx[3]]);
//     d_sq += square(refr[4] - comp[q_idx[4]]);
//     d_sq += square(refr[5] - comp[q_idx[5]]);
//     d_sq += square(refr[6] - comp[q_idx[6]]);
// 
//     min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);
// }

// inline void rotate_90_w(byte* q_idx)
// {
//     // Rotate the quadrants 90 degrees in the w direction
//     
//     // Rotate each of the quadrants
//     q_idx[0] = (4 *((q_idx[0] >> 2) & 1)) | (2 * ((~q_idx[0] >> 1) & 1)) | (1 * ((q_idx[0] >> 0) & 1));
//     q_idx[1] = (4 *((q_idx[1] >> 2) & 1)) | (2 * ((~q_idx[1] >> 1) & 1)) | (1 * ((q_idx[1] >> 0) & 1));
//     q_idx[2] = (4 *((q_idx[2] >> 2) & 1)) | (2 * ((~q_idx[2] >> 1) & 1)) | (1 * ((q_idx[2] >> 0) & 1));
//     q_idx[3] = (4 *((q_idx[3] >> 2) & 1)) | (2 * ((~q_idx[3] >> 1) & 1)) | (1 * ((q_idx[3] >> 0) & 1));
//     q_idx[4] = (4 *((q_idx[4] >> 2) & 1)) | (2 * ((~q_idx[4] >> 1) & 1)) | (1 * ((q_idx[4] >> 0) & 1));
//     q_idx[5] = (4 *((q_idx[5] >> 2) & 1)) | (2 * ((~q_idx[5] >> 1) & 1)) | (1 * ((q_idx[5] >> 0) & 1));
//     q_idx[6] = (4 *((q_idx[6] >> 2) & 1)) | (2 * ((~q_idx[6] >> 1) & 1)) | (1 * ((q_idx[6] >> 0) & 1));
//     q_idx[7] = (4 *((q_idx[7] >> 2) & 1)) | (2 * ((~q_idx[7] >> 1) & 1)) | (1 * ((q_idx[7] >> 0) & 1));
// }

// inline void compare_rotation_w(float& min_dsq, const vec_q& refr, const vec_q& comp, byte* q_idx)
// {
//     // Compare rotating 90 degrees
//     rotate_90_w(q_idx); compare_quads(min_dsq, refr, comp, q_idx);
// 
//     // Compare rotating 180 degrees
//     rotate_90_w(q_idx); compare_quads(min_dsq, refr, comp, q_idx);
// 
//     // Compare rotating 270 degrees
//     rotate_90_w(q_idx); compare_quads(min_dsq, refr, comp, q_idx);
// 
//     // Rotate 360 to end at the initial position
//     rotate_90_w(q_idx);
// }

/*
 * Each quadrant is a vector of 8 floats. Each float corresponds to the average value in
 * a given eight of a spheroid. In a 3D system with directions (u, v, w), each point is
 * assigned to a quadrant using the rule,
 *          
 *      q = (4 if w >= 0 else 0) + (2 if v >= 0 else 0) + (1 if u >= 0 else 0)
 *
 *  Therefore, the first quadrant with q = 0 corresponds to (< 0, < 0, <0). For example,
 *  the fifth quadrant corresponds to the points that fulfill (>= 0, <= 0, >= 0). Rotating
 *  the system by 90 degrees in each axis is done by using permutations of the indices.
 *  The permutations are hardcoded to avoid integer manipulations.
 */

inline float square(const float& a) { return a * a; }

inline void compare_original(float& min_dsq, const vec_q& refr, const vec_q& comp)
{
    // L2 distance squared between both environments
    float d_sq = 0.0f;
    
    // Distance square between quadrants
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q1]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q2]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q3]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q4]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q5]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q6]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q7]);
    
    // Return the comparison distance
    std::min(min_dsq, d_sq / Quadrant::NQ);
};

inline void compare_rotating_w(float& min_dsq, const vec_q& refr, const vec_q& comp)
{
    // Compare two environments rotating one in the w direction
    
    // L2 distance squared between both environments
    float d_sq = 0.0f;

    // First, rotate 90 degrees along the w axis
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q1]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q3]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q2]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q5]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q7]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q4]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q6]);

    // Update the distance squared
    min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);

    // Set the distance squared to zero
    d_sq = 0.0f;

    // Second, rotate 180 degrees along the w axis
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q3]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q2]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q1]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q7]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q6]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q5]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q4]);

    // Update the distance squared
    min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);

    // Set the distance squared to zero
    d_sq = 0.0f;

    // Third, rotate 270 degrees along the w axis
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q2]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q3]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q1]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q6]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q4]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q7]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q5]);

    // Update the distance squared
    min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);
};

inline void compare_rotating_v(float& min_dsq, const vec_q& refr, const vec_q& comp)
{
    // Compare two environments rotating one in the v direction
    
    // L2 distance squared between both environments
    float d_sq = 0.0f;

    // First, rotate 90 degrees along the v axis
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q1]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q5]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q3]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q7]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q4]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q2]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q6]);

    // Update the distance squared
    min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);

    // Set the distance squared to zero
    d_sq = 0.0f;

    // Second, rotate 180 degrees along the v axis
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q5]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q4]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q7]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q6]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q1]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q3]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q2]);

    // Update the distance squared
    min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);

    // Set the distance squared to zero
    d_sq = 0.0f;

    // Third, rotate 270 degrees along the v axis
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q4]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q6]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q2]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q5]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q1]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q7]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q3]);

    // Update the distance squared
    min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);
};

inline void compare_rotating_u(float& min_dsq, const vec_q& refr, const vec_q& comp)
{
    // Compare two environments rotating one in the u direction
    
    // L2 distance squared between both environments
    float d_sq = 0.0f;

    // First, rotate 90 degrees along the y axis
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q2]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q3]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q6]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q7]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q1]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q4]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q5]);

    // Update the distance squared
    min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);

    // Set the distance squared to zero
    d_sq = 0.0f;

    // Second, rotate 180 degrees along the v axis
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q6]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q7]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q4]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q5]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q2]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q3]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q1]);

    // Update the distance squared
    min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);

    // Set the distance squared to zero
    d_sq = 0.0f;

    // Third, rotate 270 degrees along the v axis
    d_sq += square(refr[Quadrant::q0] - comp[Quadrant::q4]);
    d_sq += square(refr[Quadrant::q1] - comp[Quadrant::q5]);
    d_sq += square(refr[Quadrant::q2] - comp[Quadrant::q0]);
    d_sq += square(refr[Quadrant::q3] - comp[Quadrant::q1]);
    d_sq += square(refr[Quadrant::q4] - comp[Quadrant::q6]);
    d_sq += square(refr[Quadrant::q5] - comp[Quadrant::q7]);
    d_sq += square(refr[Quadrant::q6] - comp[Quadrant::q2]);
    d_sq += square(refr[Quadrant::q7] - comp[Quadrant::q3]);

    // Update the distance squared
    min_dsq = std::min(min_dsq, d_sq / Quadrant::NQ);
};

// Compare two volumes in all possible relevant angles
float Quadrant::compare_quadrants(const vec_q& refr, const vec_q& comp)
{
    // Minimum distance to return
    float min_dsq = 340282346638528859811704183484516925440.0f;
    
    // Compare two quadrants without rotating any of them
    compare_original(min_dsq, refr, comp);

    // Compare two quadrants rotating one in the w axis
    compare_rotating_w(min_dsq, refr, comp);

    // Compare two quadrants rotating one in the v axis
    compare_rotating_v(min_dsq, refr, comp);

    // Compare two quadrants rotating one in the u axis
    compare_rotating_u(min_dsq, refr, comp);

    return min_dsq;
}
