#ifndef RAYT_MATH_VEC3_HPP
#define RAYT_MATH_VEC3_HPP

#include <type_traits>
#include "funcs.hpp"

template <typename data_t, typename enable = void>
class vector3;

template <typename data_t>
class vector3<data_t, typename std::enable_if<
                          std::is_floating_point<data_t>::value>::type> {
 public:
  vector3() = default;

  vector3(data_t e0, data_t e1, data_t e2) {
    e[0] = e0;
    e[1] = e1;
    e[2] = e2;
  }

  inline data_t x() const { return e[0]; }
  inline data_t y() const { return e[1]; }
  inline data_t z() const { return e[2]; }
  inline data_t r() const { return e[0]; }
  inline data_t g() const { return e[1]; }
  inline data_t b() const { return e[2]; }

  inline const vector3& operator+() const { return *this; }
  inline vector3 operator-() const { return vector3(-e[0], -e[1], -e[2]); }
  inline data_t operator[](int i) const { return e[i]; }
  inline data_t& operator[](int i) { return e[i]; };

  inline vector3& operator+=(const vector3& v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
  }
  inline vector3& operator-=(const vector3& v) {
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
  }
  inline vector3& operator*=(const vector3& v) {
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    e[2] *= v.e[2];
    return *this;
  }
  inline vector3& operator/=(const vector3& v) {
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    e[2] /= v.e[2];
    return *this;
  }
  inline vector3& operator*=(const data_t t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
  }
  inline vector3& operator/=(const data_t t) {
    auto k = data_t{1.0} / t;
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
    return *this;
  }

  inline data_t length() const {
    return Q_sqrt<data_t>(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
  }
  inline data_t squared_length() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }
  inline void make_unit_vector() {
    auto k = Q_rsqrt<data_t>(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
  }

  // representation
  data_t e[3];
};

template <typename data_t>
inline std::istream& operator>>(std::istream& is, vector3<data_t>& t) {
  is >> t.e[0] >> t.e[1] >> t.e[2];
  return is;
}

template <typename data_t>
inline std::ostream& operator<<(std::ostream& os, const vector3<data_t>& t) {
  os << t.e[0] << " " << t.e[1] << " " << t.e[2];
  return os;
}

/*
inline void vector3::make_unit_vector() {
  auto k = Q_rsqrt<data_t>(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
  e[0] *= k;
  e[1] *= k;
  e[2] *= k;
}
*/

template <typename data_t>
inline vector3<data_t> operator+(const vector3<data_t>& v1,
                                 const vector3<data_t>& v2) {
  return vector3<data_t>(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1],
                         v1.e[2] + v2.e[2]);
}

template <typename data_t>
inline vector3<data_t> operator-(const vector3<data_t>& v1,
                                 const vector3<data_t>& v2) {
  return vector3<data_t>(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1],
                         v1.e[2] - v2.e[2]);
}

template <typename data_t>
inline vector3<data_t> operator*(const vector3<data_t>& v1,
                                 const vector3<data_t>& v2) {
  return vector3<data_t>(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1],
                         v1.e[2] * v2.e[2]);
}

template <typename data_t>
inline vector3<data_t> operator/(const vector3<data_t>& v1,
                                 const vector3<data_t>& v2) {
  return vector3<data_t>(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1],
                         v1.e[2] / v2.e[2]);
}

template <typename data_t>
inline vector3<data_t> operator*(data_t t, const vector3<data_t>& v) {
  return vector3<data_t>(t * v.e[0], t * v.e[1], t * v.e[2]);
}

template <typename data_t>
inline vector3<data_t> operator/(vector3<data_t> v, data_t t) {
  return vector3<data_t>(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

template <typename data_t>
inline vector3<data_t> operator*(const vector3<data_t>& v, data_t t) {
  return vector3<data_t>(t * v.e[0], t * v.e[1], t * v.e[2]);
}

template <typename data_t>
inline data_t dot(const vector3<data_t>& v1, const vector3<data_t>& v2) {
  return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

template <typename data_t>
inline vector3<data_t> cross(const vector3<data_t>& v1,
                             const vector3<data_t>& v2) {
  return vector3<data_t>((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
                         (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
                         (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

/*
inline vector3& vector3::operator+=(const vector3& v) {
  e[0] += v.e[0];
  e[1] += v.e[1];
  e[2] += v.e[2];
  return *this;
}

inline vector3& vector3::operator*=(const vector3& v) {
  e[0] *= v.e[0];
  e[1] *= v.e[1];
  e[2] *= v.e[2];
  return *this;
}

inline vector3& vector3::operator/=(const vector3& v) {
  e[0] /= v.e[0];
  e[1] /= v.e[1];
  e[2] /= v.e[2];
  return *this;
}

inline vector3& vector3::operator-=(const vector3& v) {
  e[0] -= v.e[0];
  e[1] -= v.e[1];
  e[2] -= v.e[2];
  return *this;
}

inline vector3& vector3::operator*=(const data_t t) {
  e[0] *= t;
  e[1] *= t;
  e[2] *= t;
  return *this;
}

inline vector3& vector3::operator/=(const data_t t) {
  data_t k = data_t{1.0} / t;
  e[0] *= k;
  e[1] *= k;
  e[2] *= k;
  return *this;
}
*/

template <typename data_t>
inline vector3<data_t> unit_vector(vector3<data_t> v) {
  return v / v.length();
}

using vec3 = vector3<float>;

#endif  // RAYT_MATH_VEC3_HPP
