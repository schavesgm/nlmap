#include <Map.hpp>

// Construct the average of all points in a quadrant
vec_q construct_quadrant(const std::vector<float>* vals_in_quad, const float& m_central) 
{
    // Generate the vector containing the average in each quadrant
    vec_q quadrant_avg(Quadrant::NQ);

    // Calculate the average for each quadrant -- adding the central
    quadrant_avg[Quadrant::q0] = std::accumulate(
        vals_in_quad[Quadrant::q0].begin(), 
        vals_in_quad[Quadrant::q0].end(),
        m_central
    ) / (vals_in_quad[Quadrant::q0].size() + 1);

    quadrant_avg[Quadrant::q1] = std::accumulate(
        vals_in_quad[Quadrant::q1].begin(), 
        vals_in_quad[Quadrant::q1].end(),
        m_central
    ) / (vals_in_quad[Quadrant::q1].size() + 1);

    quadrant_avg[Quadrant::q2] = std::accumulate(
        vals_in_quad[Quadrant::q2].begin(), 
        vals_in_quad[Quadrant::q2].end(),
        m_central
    ) / (vals_in_quad[Quadrant::q2].size() + 1);

    quadrant_avg[Quadrant::q3] = std::accumulate(
        vals_in_quad[Quadrant::q3].begin(), 
        vals_in_quad[Quadrant::q3].end(),
        m_central
    ) / (vals_in_quad[Quadrant::q3].size() + 1);

    quadrant_avg[Quadrant::q4] = std::accumulate(
        vals_in_quad[Quadrant::q4].begin(), 
        vals_in_quad[Quadrant::q4].end(),
        m_central
    ) / (vals_in_quad[Quadrant::q4].size() + 1);

    quadrant_avg[Quadrant::q5] = std::accumulate(
        vals_in_quad[Quadrant::q5].begin(), 
        vals_in_quad[Quadrant::q5].end(),
        m_central
    ) / (vals_in_quad[Quadrant::q5].size() + 1);

    quadrant_avg[Quadrant::q6] = std::accumulate(
        vals_in_quad[Quadrant::q6].begin(), 
        vals_in_quad[Quadrant::q6].end(),
        m_central
    ) / (vals_in_quad[Quadrant::q6].size() + 1);

    // The last quadrant contains the central value
    quadrant_avg[Quadrant::q7] = std::accumulate(
        vals_in_quad[Quadrant::q7].begin(), 
        vals_in_quad[Quadrant::q7].end(),
        0.0f
    ) / (vals_in_quad[Quadrant::q7].size());

    return quadrant_avg;
}

// Construct the average of points around a given grid point (u, v, w)
vec_q Map::get_quadrants(
    const int& u, const int& v, const int& w, 
    const std::vector<gemmi::GridBase<float>::Point>& table_of_indices
) {
    // Vector that will contain the values for each quadrant
    std::vector<float> values_in_each_quadrant[Quadrant::NQ];

    // Get the map value for (u, v, w)
    const float m_central = get_value(u, v, w);

    // Iterate for all points in the table
    for (auto& p : table_of_indices) {

        // Obtain the right quadrant for the current point
        const byte bw = (p.w >= 0) ? 0b100 : 0b000;
        const byte bv = (p.v >= 0) ? 0b010 : 0b000;
        const byte bu = (p.u >= 0) ? 0b001 : 0b000;

        // Append the map value to the corresponding quadrant
        values_in_each_quadrant[(bw | bv | bu)].push_back(
            get_value(p.u + u, p.v + v, p.w + w)
        );
    }

    return construct_quadrant(values_in_each_quadrant, m_central);
}

// Obtain a table containing the nearest indices in the lattice
std::vector<gemmi::GridBase<float>::Point> Map::table_of_near_indices(const double& r)
{
    // Vector that will contain all the nearest indices
    std::vector<gemmi::GridBase<float>::Point> table;

    grid.use_points_around(Fractional(0, 0, 0), r,
        [this, &table](const float& m, const double& d2, const Position& dp)
        {
            // Get the nearest point from the position
            table.push_back(grid.get_nearest_point(dp));
        }
    );

    return table;
}

// Precalculate all quadrants in the map
vec_q* Map::table_of_quadrants(const double& r)
{
    // First, precalculate the table of near indices
    const auto table_of_indices = table_of_near_indices(r);

    // Allocate memory for all the quadrants in the grid
    vec_q* table_q = new vec_q[get_volume()]; 

    // Count the corresponding quadrant for each point
    int q = 0; 
    // Iterate for each point in the grid to obtain its quadrant
    for (int w = 0; w < this->Nw; w++) {
        for (int v = 0; v < this->Nv; v++) {
            for (int u = 0; u < this->Nu; u++) {

                // Append the quadrant to the table
                table_q[q] = get_quadrants(u, v, w, table_of_indices);

                // Move to the next point in the grid
                q++;

            }
        }
    }

    return table_q;
}

// Calculate the average of all environments into a table
std::vector<float> Map::table_of_avg_quadrants(const vec_q* table_q)
{
    // Heap allocate some memory for the averages
    std::vector<float> table_avg(get_volume());

    // Iterate for each point in the grid to obtain its quadrant
    for (int i = 0; i < get_volume(); i++) {
        table_avg[i] = std::accumulate(
            table_q[i].begin(), table_q[i].end(), 0.0f
        ) / Quadrant::NQ;
    }

    return table_avg;
}

// Non local means denoiser
Map Map::nlmeans_denoise(const float& perc_threshold, const double& r_comp) 
{
    // Generate a copy of the current map
    Map denoised_map = *this;

    // Generate a table containing all the quadrants
    const vec_q* table_q = table_of_quadrants(r_comp);

    // Generate a table containing all averages quadrants
    const auto table_avg = table_of_avg_quadrants(table_q);

    // Get the maximum and minimum environment average
    const auto min = std::min_element(table_avg.begin(), table_avg.end());
    const auto max = std::max_element(table_avg.begin(), table_avg.end());

    // Calculate the denoising parameter using the threshold provided
    const float hd = perc_threshold * (*max - *min);

    // Iterate for each value in the grid
    for (int wr = 0; wr < this->Nw; wr++) {
    for (int vr = 0; vr < this->Nv; vr++) {
    for (int ur = 0; ur < this->Nu; ur++) {

        // Obtain the linear index in the grid for (ur, vr, wr)
        const auto ir = size_t(wr * Nv + vr) * Nu + ur;

        // New denoised value of the map at (ur, vr, wr)
        float m_hat = 0.0f;

        // Maximum value of the kernel and normalisation constant
        float sum_kernels = 0.0f;

        // Iterate for all other comparison environments in the grid
        for (int wc = 0; wc < this->Nw; wc++) {
        for (int vc = 0; vc < this->Nv; vc++) {
        for (int uc = 0; uc < this->Nu; uc++) {

            // Obtain the linear index in the grid for (uc, vc, wc)
            const auto ic = size_t(wc * Nv + vc) * Nu + uc;

            // Compare the two quadrants
            const float min_dsq = Quadrant::compare_quadrants(
                table_q[ir], table_q[ic]
            );

            // Using the distance, obtain the denoising kernel
            const float kernel = std::exp(- min_dsq / (2 * hd * hd));

            // Update the denoised value
            m_hat += kernel * grid.data[ic];

            // Append the kernel to the normalising constant
            sum_kernels += kernel;

        } // -- End of the main comparison uc iteration
        } // -- End of the main comparison vc iteration
        } // -- End of the main comparison wc iteration

        // Change the pixel value with the denoised version
        denoised_map.set_value(ur, vr, wr, m_hat / sum_kernels);

    } // -- End of the main reference ur iteration
    } // -- End of the main reference vr iteration
    } // -- End of the main reference wr iteration

    // // Set the denoising parameter used as field in denoised_map
    denoised_map.hd = hd;

    // Delete the heap allocated data
    delete[] table_q;

    return denoised_map;
}
