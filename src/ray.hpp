#ifndef RAYT_RAY_HPP
#define RAYT_RAY_HPP

#include "common.hpp"

class ray {
 public:
  ray() = default;
  ray(vec3 a, vec3 b) : A(a), B(b) {}

  vec3 origin() const { return A; }
  vec3 direction() const { return B; }
  vec3 point_at_parameter(real_t t) const { return A + t * B; }

  vec3 A, B;
};

#endif // RAYT_RAY_HPP
