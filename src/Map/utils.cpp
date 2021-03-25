#include <Map.hpp>

// Save the map value into a path
void Map::save_map(const std::string& path)
{
    this->update_ccp4_header();
    this->write_ccp4_map(path);
}

// Save the averages for each environment into a path
void Map::save_table_of_avg(const std::string& path, const double& r_comp)
{
    // Generate a table containing all the quadrants
    const vec_q* table_q = table_of_quadrants(r_comp);

    // Generate a table containing all averages quadrants
    const auto table_avg = table_of_avg_quadrants(table_q);

    // Generate a stream to save the data
    std::ofstream out_stream;

    // Open the stream to save the data
    out_stream.open(path, std::ios::out);

    // Flush some important data into the file
    out_stream << -3 << " " << min_value() << "\n";
    out_stream << -2 << " " << max_value() << "\n";
    out_stream << -1 << " " << avg_value() << "\n";

    // Flush the average of each environment into the path
    for (int i = 0; i < get_volume(); i++)
        out_stream << i << " " << table_avg[i] << "\n";

    // Close the stream
    out_stream.close();

    // Delete the heap allocated data
    delete[] table_q;
}
