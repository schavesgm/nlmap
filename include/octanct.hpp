#pragma once

/*
 * The octanct type is an unsigned char (1 byte = 8 bits) that represents the
 * location of an octanct in a 3-dimensional Euclidean space. An octanct is one of 
 * the eights divisions of a 3-dimensional Euclidean space coordinate system defined
 * by the signs of the coordinates. In our case, the representation is encoded in
 * the last three bits of the byte, therefore, the octanct is encoded in binary
 * the binary numbers from (000) to (111). Each bit represents the sign of one
 * dimension. The dimensions are encoded in the order (wvu), therefore, the first
 * bit encodes the sign of the w-axis, the second axis encodes the sign of the v-axis
 * and so on. An active bit (1) means the sign is positive, an inactive bit (0) 
 * means the sign is negative. 
 *
 * In the following diagram, we define our axis nomenclature. The u axis would
 * correspond to x, the v axis would correspond to y and the w axis would
 * corresponds to z. The asterisk means the w points upwards.
 *                           
 *                                 w * ----> v      
 *                                   |
 *                                   |
 *                                   u
 *
 * References:
 * -- https://en.wikipedia.org/wiki/Octant_(solid_geometry)
 *
 * Examples:
 * -- (111): Corresponds to the octanct with positive w, positive v and positive u.
 * -- (100): Corresponds to the octanct with positive w, negative v and negative u.
 * -- (001): Corresponds to the octanct with negative w, negative v and positive u.
 */

#define octanct unsigned char

namespace Octanct {

    // Define some aliases to the quadrants
    static const octanct o0 = 0b000;
    static const octanct o1 = 0b001;
    static const octanct o2 = 0b010;
    static const octanct o3 = 0b011;
    static const octanct o4 = 0b100;
    static const octanct o5 = 0b101;
    static const octanct o6 = 0b110;
    static const octanct o7 = 0b111;

    // Number of possible octancts
    static const int No = 8;

    // Number of possible rotations (9 rotations) + (1 unrotated)
    static const int Nr = 10;

    // Function to rotate an octanct around the w axis a n * 90 degrees
    octanct rotate_w(const octanct&, int n);

    // Function to rotate an octanct around the v axis a n * 90 degrees
    octanct rotate_v(const octanct&, int n);

    // Function to rotate an octanct around the u axis a n * 90 degrees
    octanct rotate_u(const octanct&, int n);

    // Table containing all possible rotations in a matrix (10 * No)
    octanct* table_of_rotations();
};
