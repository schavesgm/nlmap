#pragma once

#include <fstream>
#include <iostream>
#include <string>

namespace Utils 
{
    // -- Save the environment average in a file
    template <typename T>
    void save_envstats(const T, const std::vector<float>&, const Map&);

    // -- Save the prefilter statistics in a file
    template <typename T>
    void save_prefilter(const T, const std::vector<float>&);
};

template <typename T>
void Utils::save_envstats(
    const T path, const std::vector<float>& estat, const Map& map
) {
    // Open an stream to flush the data
    std::ofstream stream;

    // Open the stream
    stream.open(path, std::ios::out);

    if (stream.is_open()) {

        // Flush some important data in the stream
        stream << -3 << " " << map.min_value() << " " << 0.0f << "\n";
        stream << -2 << " " << map.max_value() << " " << 0.0f << "\n";
        stream << -1 << " " << map.avg_value() << " " << 0.0f << "\n";

        // Flush the environments stats into the stream
        for (int e = 0; e < map.get_volume(); e++) {
            stream << e << " " << estat[e * 2 + 0] << " " << estat[e * 2 + 1] << "\n";
        }
    }

    // Close the stream
    stream.close();
}

template <typename T>
void Utils::save_prefilter(const T path, const std::vector<float>& stats)
{
    // Open an stream to flush the data
    std::ofstream stream;

    // Open the stream
    stream.open(path, std::ios::out);

    if (stream.is_open()) {

        // Flush some important data in the stream
        stream << stats[0] << " # Mean\n";
        stream << stats[1] << " # Std\n";
        stream << stats[2] << " # Median\n";
        stream << stats[3] << " # Max\n";
        stream << stats[4] << " # Min\n";
        stream << stats[5] << " # Number of points\n";
    }

    // Close the stream
    stream.close();
}
