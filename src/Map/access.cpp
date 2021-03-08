#include <Map.hpp>

// -- Accesors for the data in the grid {{{
float Map::get_value(const int& u, const int& v, const int& w) const
{
    return this->grid.get_value(u, v, w);
}

float& Map::get_value(const int& u, const int& v, const int& w)
{
    return this->grid.get_value(u, v, w);
}

float Map::operator[](const int& i) const
{ 
    return this->grid.data[i]; 
}

float& Map::operator[](const int& i)
{
    return this->grid.data[i]; 
}
// }}}
