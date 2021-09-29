# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

if(CMAKE_MINOR_VERSION GREATER 20 OR CMAKE_MAJOR_VERSION GREATER 3)
    cmake_policy(SET CMP0126 NEW)
endif()

project("LearnOpenGL")

set (CMAKE_CXX_STANDARD 17)
if(MSVC)
    string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

# Include sub-projects.
add_subdirectory("windowing")
add_subdirectory("tutorialopengl")