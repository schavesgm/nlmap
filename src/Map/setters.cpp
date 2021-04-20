#include <Map.hpp>

void Map::set_value(const int& u, const int& v, const int& w, const float& m)
{
    this->grid.set_value(u, v, w, m);
}

void Map::set_value(const int& e, const float& m)
{
    this->grid.data[e] = m;
}

void Map::set_data(const std::vector<float>& data, const bool& normalise)
{
    // Copy the new data inside the vector in the grid
    this->grid.data = data;

    // Normalise if needed
    if (normalise) this->normalise();
}
// -- }}}

