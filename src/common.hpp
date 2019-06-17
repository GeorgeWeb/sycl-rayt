#ifndef RAYT_COMMON_HPP
#define RAYT_COMMON_HPP

#include "math/vec3.hpp"
#include "random-util/random_util.hpp"

#include <stdlib.h>
#include <iostream>
#include <vector>

#include <CL/sycl.hpp>
namespace sycl = cl::sycl;

// ...
#define RAND rand_uniform(rng, local_rand_state)
#define RAND_VEC3 vec3(RAND, RAND, RAND)

using real_t = float;
using state_t = xorwow_state_t;

template <class rng_t>
inline vec3 random_in_unit_sphere(rng_t rng, state_t* local_rand_state) {
  vec3 pt;
  do {
    pt = 2.0f * RAND_VEC3 - vec3(1.0f, 1.0f, 1.0f);
  } while (pt.squared_length() >= 1.0f);
  return pt;
}

template <class rng_t>
inline vec3 random_in_unit_disk(rng_t rng, state_t* local_rand_state) {
  vec3 pt;
  do {
    pt = 2.0f * vec3(RAND, RAND, 0.0f) - vec3(1.0f, 1.0f, 0.0f);
  } while (dot(pt, pt) >= 1.0f);
  return pt;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
  return v - 2.0f * dot(v, n) * n;
}

inline bool refract(const vec3& v, const vec3& n, real_t ni_over_nt,
                    vec3& refracted) {
  vec3 uv = unit_vector(v);
  real_t dt = dot(uv, n);
  real_t discriminant = 1.0f - ni_over_nt * ni_over_nt * (1 - dt * dt);
  if (discriminant > 0) {
    refracted = ni_over_nt * (uv - n * dt) - n * sycl::sqrt(discriminant);
    return true;
  }
  return false;
}

inline real_t schlick(real_t cosine, real_t ref_idx) {
  real_t r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
  r0 = r0 * r0;
  return r0 + (1.0f - r0) * cl::sycl::pow((1.0f - cosine), 5.0f);
}

template <typename data_t>
constexpr auto clamp(data_t val, data_t min, data_t max) {
  return val < min ? min : val > max ? max : static_cast<data_t>(val);
}

#endif  // RAYT_COMMON_HPP
