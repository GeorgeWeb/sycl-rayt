#ifndef RAYT_KERNELS_RENDER_INIT_HPP
#define RAYT_KERNELS_RENDER_INIT_HPP

// ...
#include "../common.hpp"

namespace kernels {

// initializes the random generator state
template <int width, int height>
class render_init_kernel {
  template <typename data_t>
  using write_accessor_t = sycl::accessor<data_t, 1, sycl::access::mode::write,
                                          sycl::access::target::global_buffer>;

 public:
  render_init_kernel(write_accessor_t<state_t> rand_state_ptr)
      : m_rand_state_ptr(rand_state_ptr) {}

  void operator()(sycl::nd_item<2> item) {
    auto x = item.get_global_id(0);
    auto y = item.get_global_id(1);

    auto pixel_index = y * width + x;

    auto state = get_initial_xorwow_state(pixel_index);
    m_rand_state_ptr[pixel_index] = state;
  }

 private:
  write_accessor_t<state_t> m_rand_state_ptr;
};

}  // namespace kernels

#endif  // RAYT_KERNELS_RENDER_INIT_HPP
