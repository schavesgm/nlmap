#include <octanct.hpp>

// Function to rotate an octanct around the w axis a n * 90 degrees
octanct Octanct::rotate_w(const octanct& oct, int n)
{
    // Rotate an octanct a multiple of 90 degrees around the w axis. The resulting
    // octanct would be the position of the first octanct after rotation.
    
    // Restrict n between 0 and 3
    n = n % 4;

    // Variables that will hold the binary result
    unsigned char u, v;
        
    // W is constant in this rotation
    const unsigned char w = ((oct >> 2) & 1);

    if (n == 1) {        // Rotate by 90 degrees
        u = ((~oct >> 1) & 1);
        v = (( oct >> 0) & 1);
    } else if (n == 2) { // Rotate by 180 degrees
        u = ((~oct >> 0) & 1);
        v = ((~oct >> 1) & 1);
    } else if (n == 3) { // Rotate by 270 degrees
        u = (( oct >> 1) & 1);
        v = ((~oct >> 0) & 1);
    } else  {            // Do not rotate anything
        u = ((oct >> 0)  & 1);
        v = ((oct >> 1)  & 1);
    }

    // Return the new position of the octanct after rotation
    return (4 * w) | (2 * v) | (1 * u);
}

// Function to rotate an octanct around the v axis a n * 90 degrees
octanct Octanct::rotate_v(const octanct& oct, int n)
{
    // Rotate an octanct a multiple of 90 degrees around the v axis. The resulting
    // octanct would be the position of the first octanct after rotation.
    
    // Restrict n between 0 and 3
    n = n % 4;

    // Variables that will hold the binary result
    unsigned char u, w;
        
    // W is constant in this rotation
    const unsigned char v = ((oct >> 1) & 1);

    if (n == 1) {        // Rotate by 90 degrees
        u = ((~oct >> 2) & 1);
        w = (( oct >> 0) & 1);
    } else if (n == 2) { // Rotate by 180 degrees
        u = ((~oct >> 0) & 1);
        w = ((~oct >> 2) & 1);
    } else if (n == 3) { // Rotate by 270 degrees
        u = (( oct >> 2) & 1);
        w = ((~oct >> 0) & 1);
    } else {             // Rotate by 0 degrees
        u = ((oct >> 0)  & 1);
        w = ((oct >> 2)  & 1);
    }
    return (4 * w) | (2 * v) | (1 * u);
}

// Function to rotate an octanct around the u axis a n * 90 degrees
octanct Octanct::rotate_u(const octanct& oct, int n)
{
    // Restrict n between 0 and 3
    n = n % 4;

    // Variables that will hold the binary result
    unsigned char v, w;
        
    // W is constant in this rotation
    const unsigned char u = ((oct >> 0) & 1);

    if (n == 1) {        // Rotate by 90 degrees
        v = ((~oct >> 2) & 1);
        w = (( oct >> 1) & 1);
    } else if (n == 2) { // Rotate by 180 degrees
        v = ((~oct >> 1) & 1);
        w = ((~oct >> 2) & 1);
    } else if (n == 3) { // Rotate by 270 degrees
        v = (( oct >> 2) & 1);
        w = ((~oct >> 1) & 1);
    } else {             // Rotate by 0 degrees
        v = ((oct >> 1)  & 1);
        w = ((oct >> 2)  & 1);
    }
    return (4 * w) | (2 * v) | (1 * u);
}

// Table containing all possible rotations in a matrix (10 * No)
octanct* Octanct::table_of_rotations()
{
    // Allocate some memory for the table
    octanct* table_of_rotations = new octanct[Nr * No];

    // Fill the data with the indices
    for (octanct o = 0; o < No; o++) {

        // Unrotated indices
        table_of_rotations[0 * No + o] = o;

        // Rotated indices around w 90 degrees
        for (int n = 1; n <= 3; n++) {
            table_of_rotations[(n + 0) * No + o] = rotate_w(o, n);
            table_of_rotations[(n + 3) * No + o] = rotate_v(o, n);
            table_of_rotations[(n + 6) * No + o] = rotate_u(o, n);
        }
    }

    return table_of_rotations;
}

