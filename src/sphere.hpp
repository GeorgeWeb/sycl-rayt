#ifndef RAYT_SPHERE_HPP
#define RAYT_SPHERE_HPP

#include "common.hpp"
#include "hitable.hpp"

class sphere : public hitable<sphere> {
 public:
  sphere() = default;
  /**
   * Constructor for a sphere object with lambertian material
   **/
  sphere(const vec3& cen, real_t r, material_t mat_type, const vec3& mat_color) {
    center = cen;
    radius = r;
    material_type = mat_type;
    material_color = mat_color;
  }
  /**
   * Constructor for a sphere object with metal material
   **/
  sphere(const vec3& cen, real_t r, material_t mat_type, const vec3& mat_color,
         real_t f) {
    center = cen;
    radius = r;
    material_type = mat_type;
    material_color = mat_color;
    fuzz = clamp(f, 0.0f, 1.0f);
  }
  /**
   * Constructor for a sphere object with dielectric material
   **/
  sphere(const vec3& cen, real_t r, material_t mat_type, real_t ref_idx) {
    center = cen;
    radius = r;
    material_type = mat_type;
    refraction_index = ref_idx;
  }

  bool hit(const ray& r, real_t t_min, real_t t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    real_t a = dot(r.direction(), r.direction());
    real_t b = dot(oc, r.direction());
    real_t c = dot(oc, oc) - radius * radius;
    real_t discriminant = b * b - a * c;
    if (discriminant > 0) {
      real_t temp = (-b - sycl::sqrt(discriminant)) / a;
      if (temp < t_max && temp > t_min) {
        rec.t = temp;
        rec.p = r.point_at_parameter(rec.t);
        rec.normal = (rec.p - center) / radius;
        rec.material_type = material_type;
        rec.material_color = material_color;
        rec.fuzz = fuzz;
        rec.refraction_index = refraction_index;
        return true;
      }
      temp = (-b + sycl::sqrt(discriminant)) / a;
      if (temp < t_max && temp > t_min) {
        rec.t = temp;
        rec.p = r.point_at_parameter(rec.t);
        rec.normal = (rec.p - center) / radius;
        rec.material_type = material_type;
        rec.material_color = material_color;
        rec.fuzz = fuzz;
        rec.refraction_index = refraction_index;
        return true;
      }
    }
    return false;
  }

  vec3 center;
  real_t radius;

  // material properties
  material_t material_type;
  vec3 material_color;
  real_t fuzz;
  real_t refraction_index;
};

#endif  // RAYT_SPHERE_HPP
