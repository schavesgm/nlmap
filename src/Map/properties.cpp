#include <Map.hpp>

// -- Grid properties {{{
int Map::get_Nu() const     { return this->grid.nu; }
int Map::get_Nv() const     { return this->grid.nv; }
int Map::get_Nw() const     { return this->grid.nw; }
int Map::get_volume() const { return get_Nu() * get_Nv() * get_Nw(); };
// }}}

// -- Unit cell dimensions {{{
float Map::get_a() const     { return this->grid.unit_cell.a; }
float Map::get_b() const     { return this->grid.unit_cell.b; }
float Map::get_c() const     { return this->grid.unit_cell.c; }
float Map::get_alpha() const { return this->grid.unit_cell.alpha; }
float Map::get_beta() const  { return this->grid.unit_cell.beta; }
float Map::get_gamma() const { return this->grid.unit_cell.gamma; }
// }}}

// -- Mutable properties of the map {{{
float Map::max_value() const
{
    return *std::max_element(
        this->grid.data.begin(), this->grid.data.end()
    );
}

float Map::min_value() const
{
    return *std::min_element(
        this->grid.data.begin(), this->grid.data.end()
    );
}

float Map::avg_value() const
{
    // Calculate the average value in the map
    float avg_value = 0.0f;

    for (int i = 0; i < get_volume(); i++) {
        avg_value += this->grid.data[i];
    }

    return avg_value / get_volume();
}
// }}}
