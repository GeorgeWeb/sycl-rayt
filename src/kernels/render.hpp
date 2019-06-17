#ifndef RAYT_KERNELS_RENDER_HPP
#define RAYT_KERNELS_RENDER_HPP

// ...
#include "../camera.hpp"
#include "../common.hpp"
#include "../hit_record.hpp"
#include "../sphere.hpp"

namespace kernels {

// ...
static const auto real_max = std::numeric_limits<real_t>::max();

// TODO: Description
template <int width, int height, int samples, int max_depth, int num_spheres,
          typename hitable_t>
class render_kernel {
  template <typename data_t>
  using write_accessor_t = sycl::accessor<data_t, 1, sycl::access::mode::write,
                                          sycl::access::target::global_buffer>;
  template <typename data_t>
  using read_accessor_t = sycl::accessor<data_t, 1, sycl::access::mode::read,
                                         sycl::access::target::global_buffer>;

 public:
  render_kernel(write_accessor_t<vec3> frame_ptr,
                read_accessor_t<hitable_t> spheres_ptr,
                read_accessor_t<camera> camera_ptr,
                read_accessor_t<state_t> rand_state_ptr)
      : m_frame_ptr(frame_ptr),
        m_spheres_ptr(spheres_ptr),
        m_camera_ptr(camera_ptr),
        m_rand_state_ptr(rand_state_ptr) {}

  void operator()(sycl::nd_item<2> item) {
    auto x = item.get_global_id(0);
    auto y = item.get_global_id(1);

    auto pixel_index = y * width + x;

    // ...
    auto local_rand_state = m_rand_state_ptr[pixel_index];
    // ...
    auto rng = [](auto state) { return xorwow(state); };
    // ...
    auto randf = [&local_rand_state, rng]() {
      return rand_uniform(rng, &local_rand_state);
    };

    // Sampling / Antialiasing
    vec3 final_color(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < samples; i++) {
      auto u = (x + randf()) / static_cast<real_t>(width);
      auto v = (y + randf()) / static_cast<real_t>(height);

      // camera cam;
      ray r = m_camera_ptr.get_pointer()->get_ray(u, v, rng, &local_rand_state);
      final_color += color(r, m_spheres_ptr, rng, &local_rand_state, max_depth);
    }
    final_color /= static_cast<real_t>(samples);

    // apply gamma correction
    final_color[0] = sycl::sqrt(final_color.x());
    final_color[1] = sycl::sqrt(final_color.y());
    final_color[2] = sycl::sqrt(final_color.z());

    // ...
    m_frame_ptr[pixel_index] = final_color;
  }

 private:
  /* --- Device Data --- */
  write_accessor_t<vec3> m_frame_ptr;
  read_accessor_t<hitable_t> m_spheres_ptr;
  read_accessor_t<camera> m_camera_ptr;
  read_accessor_t<state_t> m_rand_state_ptr;

  /* --- Device Functions --- */
  inline bool hit_world(const ray& r, real_t t_min, real_t t_max,
                        hit_record& rec, sycl::global_ptr<hitable_t> spheres) {
    hit_record temp_rec;
    auto hit_anything = false;
    auto closest_so_far = t_max;
    for (auto i = 0; i < num_spheres; i++) {
      if ((spheres + i)->hit(r, t_min, closest_so_far, temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
      }
    }
    return hit_anything;
  }

  template <class rng_t, class state_t>
  vec3 color(const ray& r, sycl::global_ptr<hitable_t> spheres, rng_t rng,
             state_t* local_rand_state, int depth) {
    ray cur_ray = r;
    vec3 cur_attenuation(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < depth; i++) {
      hit_record rec;
      if (hit_world(cur_ray, real_t{0.001}, real_max, rec, spheres)) {
        ray scattered;
        vec3 attenuation;
        if (rec.scatter_material(cur_ray, rec, attenuation, scattered, rng,
                                 local_rand_state)) {
          cur_attenuation *= attenuation;
          cur_ray = scattered;
        } else {
          return vec3(0.0f, 0.0f, 0.0f);
        }
      } else {
        vec3 unit_direction = unit_vector(cur_ray.direction());
        auto t = 0.5f * (unit_direction.y() + 1.0f);
        vec3 c =
            (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
        return cur_attenuation * c;
      }
    }
    // exceeded recursion ...
    return vec3(0.0f, 0.0f, 0.0f);
  }
};

}  // namespace kernels

#endif  // RAYT_KERNELS_RENDER_HPP
