cmake_minimum_required(VERSION 3.4.3)

project (sycl-rayt)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
find_package(ComputeCpp REQUIRED)

set(CMAKE_CXX_STANDARD 14)

# compile executable
add_executable(
  ${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.cpp
)
# setup project include dirs
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# link SYCL
add_sycl_to_target(
  TARGET ${PROJECT_NAME}
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.cpp
)
