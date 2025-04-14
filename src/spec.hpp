#pragma once

namespace tls
{

using int_pixels_t = uint16_t;

// Used for indices of time points
using idx_t = uint64_t;
using time_point_t = int;

namespace spec
{
constexpr int_pixels_t screen_w = 1440;
constexpr int_pixels_t screen_h = 1024;
constexpr idx_t max_bins = 32768;
} // namespace spec
} // namespace tls