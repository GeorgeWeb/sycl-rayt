#ifndef RAYT_CAMERA_HPP
#define RAYT_CAMERA_HPP

#include "common.hpp"
#include "ray.hpp"

class camera {
 public:
  camera() = default;
  // `vfov` (vertical field of view) is top to bottom in degrees
  camera(vec3 lookfrom, vec3 lookat, vec3 vup, real_t vfov, real_t aspect,
         real_t aperture,
         real_t focus_dist) {  // vfov is top to bottom in degrees
    set_view(lookfrom, lookat, vup, vfov, aspect, aperture, focus_dist);
  }

  template <class rng_t>  //, class state_t
  ray get_ray(real_t s, real_t t, rng_t rng, xorwow_state_t* local_rand_state) {
    vec3 rd = lens_radius * random_in_unit_disk(rng, local_rand_state);
    vec3 offset = u * rd.x() + v * rd.y();
    return ray(origin + offset, lower_left_corner + s * horizontal +
                                    t * vertical - origin - offset);
  }

  void set_view(vec3 lookfrom, vec3 lookat, vec3 vup, real_t vfov, real_t aspect,
                real_t aperture, real_t focus_dist) {
    lens_radius = aperture / 2;
    real_t theta = vfov * M_PI / 180;
    real_t half_height = tan(theta / 2);
    real_t half_width = aspect * half_height;
    origin = lookfrom;
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);
    lower_left_corner = origin - half_width * focus_dist * u -
                        half_height * focus_dist * v - focus_dist * w;
    horizontal = 2 * half_width * focus_dist * u;
    vertical = 2 * half_height * focus_dist * v;
  }

  vec3 origin;
  vec3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  real_t lens_radius;
};

#endif  // RAYT_CAMERA_HPP
