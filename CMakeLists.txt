cmake_minimum_required(VERSION 3.15...4.0)
project(
  Renderer
  VERSION 1.0
  DESCRIPTION "Simple software rasterizer"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Any CMAKE_ variables can go here

# Enable AVX2 (and SSE, but not needed)
add_compile_options(-mavx2 -msse4.1)

# Optional: enable strict warnings and debug info
add_compile_options(-Wall -Wextra -O2 -g)

# Find packages can go here

add_executable(render src/main.cpp src/structs.h src/screen.cpp src/math.cpp
                      src/utils.h src/random.cpp)
