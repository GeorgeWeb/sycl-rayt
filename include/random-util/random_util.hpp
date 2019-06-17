#ifndef RAYT_RANDOM_UTIL_HPP
#define RAYT_RANDOM_UTIL_HPP

// #include "rngpu.hpp"

#include <stdlib.h>
#include <iostream>
#include <type_traits>
#include "math/funcs.hpp"

/* xorand variation for use on the device */

/**
 * Sources which has aided the development of this random utility:
 *
 * AMD ROCm's rocRAND:
 * https://github.com/ROCmSoftwarePlatform/rocRAND/blob/master/library/include/rocrand_xorwow.h
 *
 * StackOverlow thread on good int hash functions:
 * https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
 */

/**
 * hash functions
 */

constexpr inline auto mueller_hash(unsigned int x) {
  x = ((x >> 16u) ^ x) * 0x45d9f3bu;
  x = ((x >> 16u) ^ x) * 0x45d9f3bu;
  x = ((x >> 16u) ^ x);
  return x;
}

#define hash(x) (mueller_hash(x))

/**
 * random states and generators
 */

struct xorwow_state_t {
  // xorshift values (160 bits)
  unsigned int x;
  unsigned int y;
  unsigned int z;
  unsigned int w;
  unsigned int v;
  // sequence value
  unsigned int d;
};

constexpr inline auto xorwow(xorwow_state_t* state) {
  unsigned int t = (state->x ^ (state->x >> 2u));
  state->x = state->y;
  state->y = state->z;
  state->z = state->w;
  state->v = (state->v ^ (state->v << 4u)) ^ (t ^ (t << 1u));
  state->d = state->d + 362437u;
  return state->v + state->d;
}

template <unsigned int num_iters = 8>
constexpr inline auto get_initial_xorwow_state(unsigned int seed) {
  xorwow_state_t state;
  state.d = !seed ? 4294967295ul : seed;
  for (auto i = 0u; i < num_iters; i++) {
    state.x = hash(state.d);
    state.y = hash(state.x);
    state.z = hash(state.y);
    state.w = hash(state.z);
    state.v = hash(state.w);
    state.d = hash(state.v);
  }
  return state;
}

/**
 * meta function(s) for uniform sampling in range: [0, 1)
 * using the defined generator(s) and matching state(s)
 */

template <class rng_t, class state_t, typename data_t = float>
constexpr inline auto rand_uniform(rng_t rng, state_t* state) {
  auto res = data_t{0.0};
  if (std::is_same<data_t, float>::value) {
    auto a = rng(state) >> 9u;
    auto res = 0.f;
    *(reinterpret_cast<unsigned int*>(&res)) = a | 0x3F800000u;
    return res - 1.f;
  } else if (std::is_same<data_t, double>::value) {
    auto a = rng(state) >> 6u;
    auto b = rng(state) >> 5u;
    res = (a * 134217728.0 + b) / 9007199254740992.0;
    return res;
  } else {
    // TODO: better handling of unspecified cases
    return res;
  }
}

/**
 * 'xorand' variation for quick and easy use on host the device
 * the result of calling it will return a uniform random value
 */

template <typename data_t>
constexpr inline auto xorand() {
  union {
    unsigned int ui;
    data_t uf;
  } result{};

  unsigned int fmask = (1u << 23u) - 1u;
  result.ui = (rand() & fmask) | 0x3f800000u;
  return result.uf - data_t{1.0};
};

#endif  // RAYT_RANDOM_UTIL_HPP
