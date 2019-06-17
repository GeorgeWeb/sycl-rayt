#ifndef RAYT_MATH_FUNCS_HPP
#define RAYT_MATH_FUNCS_HPP

#include <math.h>

namespace detail {
constexpr int default_iterations = 3;
}

/** @description: evil floating point bit level hacking
 *  @source: https://en.wikipedia.org/wiki/Fast_inverse_square_root
 *  @author: John Carmak (while at 'idSoftware') */
template <typename data_t>
inline auto Q_rsqrt(data_t number,
                    int iterations = detail::default_iterations) {
  long i;
  data_t x2, y;
  constexpr auto threehalfs = data_t{1.5};

  x2 = number * 0.5f;
  y = number;
  i = *(long*) &y;            // floating point bit level hacking [sic]
  i = 0x5f3759df - (i >> 1);  // Newton's approximation
  y = *(data_t*) &i;
  for (auto i = 0; i < iterations; i++) {
    y = y * (threehalfs - (x2 * y * y));  // 1st iteration
  }
  // less iterations -> faster but loss in accuracy
  // for speed and reasonable accuracy: 1 to 3 iter.

  return y;
}

// compute square root based on Carmak's fast inverse sqrt
template <typename data_t>
constexpr inline auto Q_sqrt(data_t number,
                             int iterations = detail::default_iterations) {
  return 1 / Q_rsqrt<data_t>(number, iterations);
}

#endif  // RAYT_MATH_FUNCS_HPP
