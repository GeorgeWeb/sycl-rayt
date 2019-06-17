#ifndef RAYT_HITABLE_HPP
#define RAYT_HITABLE_HPP

#include "hit_record.hpp"

// ...
namespace crtp {

template <typename derived>
struct helper {
  derived& underlying() { return static_cast<derived&>(*this); }
  const derived& underlying() const {
    return static_cast<const derived&>(*this);
  }
};

}  // namespace crtp

// ...
template <class derived>
class hitable : crtp::helper<derived> {
  bool hit(const ray& r, real_t t_min, real_t t_max, hit_record& rec) const {
    return this->underlying().hit(r, t_min, t_max, rec);
  }
};

#endif  // RAYT_HITABLE_HPP
