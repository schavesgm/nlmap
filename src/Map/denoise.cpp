#include <Map.hpp>

// Construct the average of all points in a quadrant
vec_q construct_quadrant(
    const std::vector<float>* vals_in_quad, const float& m_central
) {
    // Generate the vector containing the average in each quadrant
    vec_q quadrant_avg(Quadrant::NQ);

    // Calculate the average for each quadrant
    for (byte q = 0; q < Quadrant::NQ; q++) {

        // Variable that will hold the average
        float avg = 0.0f;

        // Number points in the q quadrant
        const size_t N = vals_in_quad[q].size();

        // Calculate the average in the quadrant
        for (auto& m : vals_in_quad[q]) avg += m;

        // If the quadrant is not 0b111 = 7, then add central point
        if (q != 0b111) {
            avg += m_central; avg = avg / (N + 1);
        } else {
            avg = avg / N;
        }

        // Get the average in its corresponding quadrant
        quadrant_avg[q] = avg;
    }

    return quadrant_avg;
}

// Construct the average of points around a given grid point (u, v, w)
vec_q Map::get_quadrants(
    const int& u, const int& v, const int& w, const double& r_max
) {
    // Get the Fractional coordinates of the grid values
    const Fractional fp = get_fractional(u, v, w);

    // Vector that will contain the values for each quadrant
    std::vector<float> values_in_each_quadrant[Quadrant::NQ];

    // Map value for the (u, v, w) coordinates
    const float m_central = get_value(u, v, w);

    // Use the points around fp and append them into each quadrant
    this->grid.use_points_around(fp, r_max,
        [&](const float& m, const double& d2, const Position& dp)
        {
            // Obtain the byte that will represent the quadrant
            const byte bw = (dp.z >= 0.0) ? 0b100 : 0b000;
            const byte bv = (dp.y >= 0.0) ? 0b010 : 0b000;
            const byte bu = (dp.x >= 0.0) ? 0b001 : 0b000;

            // Append the map value to the corresponding quadrant
            values_in_each_quadrant[(bw | bv | bu)].push_back(m);
        }
    );

    return construct_quadrant(values_in_each_quadrant, m_central);
}

// Non local means denoiser
Map Map::nlmeans_denoise(
    const float& h2, const double& search_r, const double& comp_r
) {
    // Generate a copy of the current map
    Map denoised_map = *this;

    // Iterate for each value in the grid
    for (int wr = 0; wr < this->Nw; wr++) {
    for (int vr = 0; vr < this->Nv; vr++) {
    for (int ur = 0; ur < this->Nu; ur++) {

        // Obtain the quadrant vector around (ur, vr, wr)
        const vec_q refr_q = get_quadrants(ur, vr, wr, comp_r);

        // Obtain the Fractional coordinates of (ur, vr, wr)
        const Fractional refr_F = get_fractional(ur, vr, wr);

        // Obtain the real space coordinates of (ur, vr, wr)
        const Position refr_P = get_position(ur, vr, wr);

        // New denoised value of the map at (ur, vr, wr)
        float m_hat = 0.0f;

        // Maximum value of the kernel and normalisation constant
        float max_kernel = -1.0f, sum_kernels = 0.0f;

        // Process all points around (ur, vr, wr) with distance search_r
        this->grid.use_points_around(refr_F, search_r,
            [&](const float& m, const double& d2, const Position& dp)
            {
                // Obtain the closes point (uc, vc, wc)
                const auto comp_p = this->grid.get_nearest_point(
                    refr_P + dp
                );

                // References to the (uc, vc, wc) coordinates
                const int& uc = comp_p.u;
                const int& vc = comp_p.v;
                const int& wc = comp_p.w;

                // Obtain the average quadrants for this position
                const vec_q comp_q = get_quadrants(uc, vc, wc, comp_r);

                // Compare reference and comparision quadrants
                const float min_dsq = Quadrant::compare_quadrants(
                    refr_q, comp_q
                );

                // Using the distance, obtain the denoising kernel
                const float kernel = std::exp(- min_dsq / h2);

                // Update the maximum kernel value
                max_kernel = std::max(max_kernel, kernel);

                // Update the denoised value
                m_hat += kernel * m;

                // Append the kernel to the normalising constant
                sum_kernels += kernel;
            }
        );

        // Update the denoised value with the central pixel
        m_hat += max_kernel * get_value(ur, vr, wr);

        // Add the central kernel to the normalising constant
        sum_kernels += max_kernel;

        // Change the pixel value
        denoised_map.set_value(ur, vr, wr, m_hat / sum_kernels);

    } // End of the main ur iteration
    } // End of the main vr iteration
    } // End of the main wr iteration

    return denoised_map;
}
