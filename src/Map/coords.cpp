#include <Map.hpp>

// -- Get the position in real space of given indices
Position Map::get_position(const int& u, const int& v, const int& w) const
{
    return this->grid.get_position(u, v, w);
}

// -- Get the fractional coordinates given some grid indiced
Fractional Map::get_fractional(const int& u, const int& v, const int& w) const
{
    return this->grid.get_fractional(u, v, w);
}
