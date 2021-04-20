#pragma once

#include <fstream>
#include <iostream>
#include <string>

namespace Utils 
{
    // -- Save the environment average in a file
    template <typename C>
    void save_envavg(const C, const std::vector<float>&, const Map&);
};

template <typename C>
void Utils::save_envavg(const C path, const std::vector<float>& envavg, const Map& map)
{
    // Open an stream to flush the data
    std::ofstream stream;

    // Open the stream
    stream.open(path, std::ios::out);

    if (stream.is_open()) {

        // Flush some important data in the stream
        stream << -3 << " " << map.min_value() << "\n";
        stream << -2 << " " << map.max_value() << "\n";
        stream << -1 << " " << map.avg_value() << "\n";

        // Flush the environments average into the stream
        for (int e = 0; e < map.get_volume(); e++)
            stream << e << " " << envavg[e] << "\n";
    }

    // Close the stream
    stream.close();
}
