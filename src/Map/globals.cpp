#include <Map.hpp>

void Map::normalise()
{
    // Normalise map to the interval [0, 1]

    // Get the maximum and minimum value of the map
    float min_val = this->min_value();
    float max_val = this->max_value();

    // Denominator
    float den = (max_val - min_val);

    // Normalise each value in the map
    for (int i = 0; i < get_volume(); i++) {
        auto norm = (this->grid.data[i] - min_val) / den;
        this->operator[](i) = norm;
    }
}

#include <iostream>

float Map::add_noise(const float& sigma, const bool& normalise)
{
    // Add gaussian noise with std sigma to the whole map
    
    // If the sigma value is zero, do not add any noise
    if (sigma == 0) return sigma;

    // Instantiate a random engine
    std::default_random_engine engine;
    std::normal_distribution<float> normal(0.0, sigma);

    // Add some random noise to each point in the grid
    for (int i = 0; i < get_volume(); i++) {
        this->operator[](i) += normal(engine);
    }

    // Normalise from 0 to 1
    if (normalise) this->normalise();

    return sigma;
}
