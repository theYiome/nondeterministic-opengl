cmake_minimum_required(VERSION 3.20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS YES CACHE BOOL "Export all symbols")

find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_library(windowing "")

target_link_libraries(windowing PRIVATE glad::glad)
target_link_libraries(windowing PRIVATE glm::glm)
target_link_libraries(windowing PRIVATE glfw)
target_link_libraries(windowing PRIVATE imgui::imgui)

target_sources(windowing
    PRIVATE
        "include/Callbacks.hpp"
        "include/SimpleWindow.hpp"
        "include/CameraWindow.hpp"
        "include/FirstPersonCamera.hpp"

        "src/Callbacks.cpp"
        "src/SimpleWindow.cpp"
        "src/CameraWindow.cpp"
        "src/FirstPersonCamera.cpp"
)

set_target_properties(windowing PROPERTIES VERSION 1)

target_include_directories(windowing PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties(windowing PROPERTIES PUBLIC_HEADER "windowing.hpp")