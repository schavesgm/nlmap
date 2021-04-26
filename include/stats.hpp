#pragma once

#include <cmath>
#include <algorithm>

namespace Stats
{
    float mean(const float*, const int&);
    float std(const float*,  const int&);
    float median(const float*, const int&);

    float max(const float*,  const int&);
    float min(const float*,  const int&);
};
