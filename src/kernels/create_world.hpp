#ifndef RAYT_KERNELS_CREATE_WORLD_HPP
#define RAYT_KERNELS_CREATE_WORLD_HPP

// ...
#include "../camera.hpp"
#include "../common.hpp"
#include "../hit_record.hpp"
#include "../sphere.hpp"

namespace kernels {

// TODO: Description
class init_world_view {};

// TODO: Description
// creates the scene to be rendered (world of spheres)
template <int num_spheres, typename hitable_t>
class create_world_kernel {
  template <typename data_t>
  using write_accessor_t = sycl::accessor<data_t, 1, sycl::access::mode::write,
                                          sycl::access::target::global_buffer>;
  template <typename data_t>
  using read_accessor_t = sycl::accessor<data_t, 1, sycl::access::mode::read,
                                         sycl::access::target::global_buffer>;

 public:
  create_world_kernel(write_accessor_t<hitable_t> spheres_ptr,
                      read_accessor_t<state_t> rand_state_ptr)
      : m_spheres_ptr(spheres_ptr), m_rand_state_ptr(rand_state_ptr) {}

  void operator()(sycl::nd_item<1> item) {
    auto local_id = item.get_local_id(0);
    auto group_id = item.get_group(0);

    if (local_id == 0 && group_id == 0) {
      auto global_id = item.get_global_id(0);

      // seed rand gen
      m_rand_state_ptr[global_id] = get_initial_xorwow_state(global_id);

      // use rand gen
      // ...
      auto local_rand_state = m_rand_state_ptr[global_id];
      // ...
      auto rng = [](auto state) { return xorwow(state); };
      // ...
      auto randf = [&local_rand_state, rng]() {
        return rand_uniform(rng, &local_rand_state);
      };
      // ...
      auto rand_vec3f = [randf]() { return vec3(randf(), randf(), randf()); };

      // define materials
      auto lambertian = material_t::LAMBERTIAN;
      auto metal = material_t::METAL;
      auto dielectric = material_t::DIELECTRIC;
      
      // initialize world of spheres
      auto index = global_id;
      m_spheres_ptr[index + 0] = sphere(vec3(0.0f, -1000.0f, -1.0f), 1000.0f,
                                        lambertian, vec3(0.5f, 0.5f, 0.5f));
      index += 1;
      for (auto i = -11; i < 11; i++) {
        for (auto j = -11; j < 11; j++) {
          auto choose_mat = randf();
          vec3 center(i + randf(), 0.2f, j + randf());
          if (choose_mat < 0.8f) {  // lambertian
            m_spheres_ptr[index++] =
                sphere(center, 0.2f, lambertian, rand_vec3f());
          } else if (choose_mat < 0.95f) {  // metal
            m_spheres_ptr[index++] =
                sphere(center, 0.2f, metal,
                       vec3(0.5f * (1.0f + randf()), 0.5f * (1.0f + randf()),
                            0.5f * (1.0f + randf())),
                       0.5f * randf());
          } else {  // dielectric
            m_spheres_ptr[index++] = sphere(center, 0.2f, dielectric, 1.5f);
          }
        }
      }
      m_spheres_ptr[index++] =
          sphere(vec3(0.0f, 1.0f, 0.0f), 1.0f, dielectric, 1.5f);
      m_spheres_ptr[index++] = sphere(vec3(-4.0f, 1.0f, 0.0f), 1.0f, lambertian,
                                      vec3(0.4f, 0.2f, 0.1f));
      m_spheres_ptr[index++] = sphere(vec3(4.0f, 1.0f, 0.0f), 1.0f, metal,
                                      vec3(0.7f, 0.6f, 0.5f), 0.0f);
    }
  }

 private:
  write_accessor_t<hitable_t> m_spheres_ptr;
  read_accessor_t<state_t> m_rand_state_ptr;
};

}  // namespace kernels

#endif  // RAYT_KERNELS_CREATE_WORLD_HPP
