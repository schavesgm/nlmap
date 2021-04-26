#include <stats.hpp>

// Calculate the arithmetic mean of an array
float Stats::mean(const float* array, const int& size)
{
    // Memory where the average will be calculated
    float avg_value = 0.0f;

    for (int i = 0; i < size; i++)
        avg_value += array[i];

    return avg_value / size;
}

// Calculate the standard deviation of an array
float Stats::std(const float* array, const int& size)
{
    // Calculate the average
    const float avg_value = mean(array, size);

    // Memory where the standard deviation will be calculated
    float std_value = 0.0f;

    for (int i = 0; i < size; i++)
        std_value += std::pow(array[i] - avg_value, 2);
    
    return std::sqrt(std_value / (size - 1));
}

// Calculate the median of the array
float Stats::median(const float* array, const int& size)
{
    // Make a copy of the array
    float* copy = new float[size];
    for (int i = 0; i < size; i++)
        copy[i] = array[i];

    // Sort the copied array using std::sort
    std::sort(copy, copy + size);

    if (size % 2 != 0)
      return copy[size / 2];

    return (float)(copy[(size-1) / 2] + copy[size / 2]) / 2.0;
}

// Get the maximum value of the array
float Stats::max(const float* array, const int& size)
{
    // Temporary to hold the maximum value
    float max_value = array[0];

    for (int i = 1; i < size; i++)
        max_value = (max_value < array[i]) ? array[i] : max_value;

    return max_value;
}

// Get the minimum value of the array
float Stats::min(const float* array, const int& size)
{
    // Temporary to hold the maximum value
    float min_value = array[0];

    for (int i = 1; i < size; i++)
        min_value = (min_value > array[i]) ? array[i] : min_value;

    return min_value;
}
