#pragma once

#include <cstdint>

namespace tls
{

using int_pixels_t = uint16_t;

// Used for indices of years
using idx_t = uint32_t;
using year_t = int;

namespace spec
{
constexpr int_pixels_t screen_w = 1440;
constexpr int_pixels_t screen_h = 680;
constexpr idx_t max_bins = 32768;
// constexpr idx_t BINS_OFFSET = 32768 - 3000;
constexpr idx_t bins_split = 32768 - 3000;
}
}