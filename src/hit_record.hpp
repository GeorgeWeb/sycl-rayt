#ifndef RAYT_HIT_RECORD_HPP
#define RAYT_HIT_RECORD_HPP

#include "common.hpp"
#include "ray.hpp"

// supported material types
enum class material_t { LAMBERTIAN, METAL, DIELECTRIC };

class hit_record {
 public:
  hit_record() = default;

  // ...
  template <class rng_t>
  bool scatter_material(const ray& r_in, const hit_record& rec,
                        vec3& attenuation, ray& scattered, rng_t rng,
                        xorwow_state_t* local_rand_state) {
    switch (material_type) {
      case material_t::LAMBERTIAN:
        // scatter lambertian
        {
          vec3 target =
              rec.p + rec.normal + random_in_unit_sphere(rng, local_rand_state);
          scattered = ray(rec.p, target - rec.p);
          attenuation = material_color;
          return true;
        }
      case material_t::METAL:
        // scatter metal
        {
          vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
          scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(
                                                        rng, local_rand_state));
          attenuation = material_color;
          return (dot(scattered.direction(), rec.normal) > 0.0f);
        }
      case material_t::DIELECTRIC:
        // scatter dielectric
        {
          vec3 outward_normal;
          vec3 reflected = reflect(r_in.direction(), rec.normal);
          real_t ni_over_nt;
          attenuation = vec3(1.0f, 1.0f, 1.0f);
          vec3 refracted;
          real_t reflect_prob;
          real_t cosine;
          if (dot(r_in.direction(), rec.normal) > 0.0f) {
            outward_normal = -rec.normal;
            ni_over_nt = refraction_index;
            cosine =
                dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = sycl::sqrt(1.0f - refraction_index * refraction_index *
                                           (1.0f - cosine * cosine));
          } else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0f / refraction_index;
            cosine =
                -dot(r_in.direction(), rec.normal) / r_in.direction().length();
          }
          if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
            reflect_prob = schlick(cosine, refraction_index);
          else {
            reflect_prob = 1.0f;
          }
          auto rf = rand_uniform(rng, local_rand_state);
          scattered =
              rf < reflect_prob ? ray(rec.p, reflected) : ray(rec.p, refracted);
          return true;
        }
      default:
        return false;
    }
  }

 public:
  real_t t;
  vec3 p;
  vec3 normal;

  // ...
  material_t material_type;
  vec3 material_color;
  real_t fuzz;
  real_t refraction_index;

 private:
  // TODO: implement ...
  void scatter_lambertian() {}
  void scatter_metal() {}
  void scatter_dielectric() {}
};

#endif  // RAYT_HIT_RECORD_HPP
