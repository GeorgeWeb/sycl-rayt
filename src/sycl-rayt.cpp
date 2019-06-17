#include <CL/sycl.hpp>

#include <algorithm>
#include <limits>
#include <memory>

#include "kernels/create_world.hpp"
#include "kernels/render.hpp"
#include "kernels/render_init.hpp"

#include "image-io/image_io.hpp"

static constexpr auto tileX = 8;
static constexpr auto tileY = 8;

template <int width, int height>
void render_init(sycl::queue& queue, state_t* rand_state_data) {
  constexpr auto num_pixels = width * height;
  try {
    // setup
    auto rand_state_buf =
        sycl::buffer<state_t, 1>(rand_state_data, sycl::range<1>(num_pixels));
    // compute
    auto global = sycl::range<2>(width, height);
    auto local = sycl::range<2>(tileX, tileY);
    queue.submit([&](sycl::handler& cgh) {
      auto rand_state_ptr =
          rand_state_buf.get_access<sycl::access::mode::write>(cgh);
      // execute kernel
      cgh.parallel_for(
          sycl::nd_range<2>(global, local),
          kernels::render_init_kernel<width, height>(rand_state_ptr));
    });
  } catch (const sycl::exception& e) {
    std::cerr << "SYCL exception caught: " << e.what() << "\n";
    throw;
  }
}

// TODO: Description
template <int num_spheres, typename hitable_t>
void create_world(sycl::queue& queue, hitable_t* spheres, state_t* rand_state) {
  try {
    // setup
    auto spheres_buf =
        sycl::buffer<hitable_t, 1>(spheres, sycl::range<1>(num_spheres));
    auto rand_state_buf =
        sycl::buffer<state_t, 1>(rand_state, sycl::range<1>(1));
    // compute
    queue.submit([&](sycl::handler& cgh) {
      auto spheres_ptr =
          spheres_buf.template get_access<sycl::access::mode::write>(cgh);
      auto rand_state_ptr =
          rand_state_buf.get_access<sycl::access::mode::read>(cgh);
      // execute kernel
      cgh.parallel_for(
          sycl::nd_range<1>(sycl::range<1>(num_spheres), sycl::range<1>(1)),
          kernels::create_world_kernel<num_spheres, hitable_t>(spheres_ptr,
                                                               rand_state_ptr));
    });
  } catch (const sycl::exception& e) {
    std::cerr << "SYCL exception caught: " << e.what() << "\n";
    throw;
  }
}

// TODO: Description
template <int width, int height, int samples, int max_depth, int num_spheres,
          typename hitable_t>
void render(sycl::queue& queue, vec3* fb_data, sphere* spheres, camera* cam,
            state_t* rand_state_data) {
  constexpr auto num_pixels = width * height;
  try {
    // setup
    auto frame_buf = sycl::buffer<vec3, 1>(fb_data, sycl::range<1>(num_pixels));
    auto spheres_buf =
        sycl::buffer<sphere, 1>(spheres, sycl::range<1>(num_spheres));
    auto camera_buf = sycl::buffer<camera, 1>(cam, sycl::range<1>(1));
    auto rand_state_buf =
        sycl::buffer<state_t, 1>(rand_state_data, sycl::range<1>(num_pixels));
    // compute
    auto global = sycl::range<2>(width, height);
    auto local = sycl::range<2>(tileX, tileY);
    queue.submit([&](sycl::handler& cgh) {
      auto frame_ptr = frame_buf.get_access<sycl::access::mode::write>(cgh);
      auto spheres_ptr = spheres_buf.get_access<sycl::access::mode::read>(cgh);
      auto camera_ptr = camera_buf.get_access<sycl::access::mode::read>(cgh);
      auto rand_state_ptr =
          rand_state_buf.get_access<sycl::access::mode::read>(cgh);
      // execute kernel
      cgh.parallel_for(sycl::nd_range<2>(global, local),
                       kernels::render_kernel<width, height, samples, max_depth,
                                              num_spheres, hitable_t>(
                           frame_ptr, spheres_ptr, camera_ptr, rand_state_ptr));
    });
  } catch (const sycl::exception& e) {
    std::cerr << "SYCL exception caught: " << e.what() << "\n";
    throw;
  }
}

int main() {
  // ...
  constexpr auto width = 1200;
  constexpr auto height = 800;
  constexpr auto samples = 10;
  constexpr auto max_depth = 50;
  // ...
  constexpr auto num_spheres = 22 * 22 + 1 + 3;
  constexpr auto num_pixels = width * height;
  // ...
  bool create_world_on_device = false;
  // ...
  auto queue = sycl::queue(sycl::gpu_selector{});

  /* initialize renderer */

  // ...
  std::vector<xorwow_state_t> rand_states(num_pixels);
  render_init<width, height>(queue, rand_states.data());

  /* initialize/create world (of spheres) */

  std::vector<sphere> spheres;
  if (create_world_on_device) {
    spheres.resize(num_spheres);
    xorwow_state_t rand_state;
    create_world<num_spheres, sphere>(queue, spheres.data(), &rand_state);
  }
  // otherwise, it will be initialized on the host
  else {
    // define material types
    auto lambertian = material_t::LAMBERTIAN;
    auto metal = material_t::METAL;
    auto dielectric = material_t::DIELECTRIC;
    // define random generator
    auto randf = [] { return xorand<real_t>(); };
    // lambda to define the logic for random world generation
    auto create_world = [&spheres, lambertian, metal, dielectric, randf]() {
      // ground
      spheres.push_back(sphere(vec3(0.0f, -1000.0f, -1.0f), 1000.0f, lambertian,
                               vec3(0.5f, 0.5f, 0.5f)));
      for (auto i = -11; i < 11; i++) {
        for (auto j = -11; j < 11; j++) {
          auto choose_mat = randf();
          vec3 center(i + randf(), 0.2f, j + randf());
          if (choose_mat < 0.8f) {  // lambertian
            spheres.push_back(sphere(center, 0.2f, lambertian,
                                     vec3(randf(), randf(), randf())));
          } else if (choose_mat < 0.95f) {  // metal
            spheres.push_back(
                sphere(center, 0.2f, metal,
                       vec3(0.5f * (1.0f + randf()), 0.5f * (1.0f + randf()),
                            0.5f * (1.0f + randf())),
                       0.5f * randf()));
          } else {  // dielectric
            spheres.push_back(sphere(center, 0.2f, dielectric, 1.5f));
          }
        }
      }
      spheres.push_back(sphere(vec3(0.0f, 1.0f, 0.0f), 1.0f, dielectric, 1.5f));
      spheres.push_back(sphere(vec3(-4.0f, 1.0f, 0.0f), 1.0f, lambertian,
                               vec3(0.4f, 0.2f, 0.1f)));
      spheres.push_back(sphere(vec3(4.0f, 1.0f, 0.0f), 1.0f, metal,
                               vec3(0.7f, 0.6f, 0.5f), 0.0f));
    };
    // create it.
    create_world();
  }

  /* render world */

  // ...
  std::vector<vec3> fb(num_pixels);
  // initialize camera
  auto look_from = vec3(13.0f, 2.0f, 3.0f);
  auto look_at = vec3(0.0f, 0.0f, 0.0f);
  auto up = vec3(0.0f, 1.0f, 0.0f);
  auto dist_to_focus = 10.0f;
  auto aperture = 0.1f;
  auto angle = 30.0f;
  camera cam(look_from, look_at, up, angle,
             static_cast<real_t>(width) / static_cast<real_t>(height), aperture,
             dist_to_focus);
  // ...
  render<width, height, samples, max_depth, num_spheres, sphere>(
      queue, fb.data(), spheres.data(), &cam, rand_states.data());

  // ...
  image_io<width, height>::save_as(fb.data(), format_t::ppm);

  return 0;
}
