#include <Map.hpp>

Map::Map(const std::string& path, const float& missing)
{
    // Read the data from the file and extend to unit cell
    this->read_ccp4_file(path);
    this->setup(gemmi::GridSetup::Full, missing);
}

Map::Map(const Map& other)
{
    // Copy the contents of the map
    this->grid = other.grid;
    this->hstats = other.hstats;
    this->ccp4_header = other.ccp4_header;
    this->same_byte_order = other.same_byte_order;
}

void Map::operator=(const Map& other)
{
    // Copy the contents of the map
    this->grid = other.grid;
    this->hstats = other.hstats;
    this->ccp4_header = other.ccp4_header;
    this->same_byte_order = other.same_byte_order;
}
