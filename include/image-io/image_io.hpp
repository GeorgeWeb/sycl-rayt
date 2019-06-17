#ifndef RAYT_IMAGE_UTIL_HPP
#define RAYT_IMAGE_UTIL_HPP

#include "file-io/file_io.hpp"
#include "math/vec3.hpp"

// TODO: Description
enum class format_t { none, ppm, jpeg };

template <int width, int height>
class image_io {
 public:
  static void save_as(vec3* fb_data, format_t fmt) {
    switch (fmt) {
      default:
      case format_t::none:
        break;
      case format_t::ppm:
        save_as_ppm(fb_data);
        break;
      case format_t::jpeg:
        // TODO: Add jpeg write support (stb_image)
        break;
    }
  }

 private:
  static void save_as_ppm(vec3* fb_data) {
    std::cout << "P3\n" << width << " " << height << "\n255\n";
    for (int y = height - 1; y >= 0; y--) {
      for (int x = 0; x < width; x++) {
        auto pixel_index = y * width + x;
        int r = static_cast<int>(255.99 * fb_data[pixel_index].r());
        int g = static_cast<int>(255.99 * fb_data[pixel_index].g());
        int b = static_cast<int>(255.99 * fb_data[pixel_index].b());
        std::cout << r << " " << g << " " << b << "\n";
      }
    }
  }

  static void save_as_jpeg(vec3* fb_data) {
    std::cout << "Not implemented yet.\n";
  }

 private:
  image_io() = default;
};

#endif  // RAYT_IMAGE_UTIL_HPP
