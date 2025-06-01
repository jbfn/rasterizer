# Almost all CMake files should start with this You should always specify a
# range with the newest and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.15...4.0)

set(CMAKE_CXX_STANDARD 17)

project(
  Renderer
  VERSION 1.0
  DESCRIPTION "First attempt at rendering"
  LANGUAGES CXX)

# If you set any CMAKE_ variables, that can go here. (But usually don't do this,
# except maybe for C++ standard)

# Find packages go here.

# You should usually split this into folders, but this is a simple example

add_executable(render src/main.cpp src/models.cpp src/image.cpp src/utils.cpp
                      src/random.cpp)
