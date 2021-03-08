#include <Map.hpp>

void Map::save_map(const std::string& path)
{
    this->update_ccp4_header();
    this->write_ccp4_map(path);
}
