# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Add source to this project's executable.
add_executable(tutorialopengl "")

target_sources(tutorialopengl
    PRIVATE
        "main.cpp"
        "main.hpp"
        "utils_gl.hpp"
        "stb.cpp"
        "Texture.hpp"
)

find_path(STB_INCLUDE_DIRS "stb.h")
target_include_directories(tutorialopengl PRIVATE ${STB_INCLUDE_DIRS})

target_link_libraries(tutorialopengl PRIVATE glad::glad)
target_link_libraries(tutorialopengl PRIVATE glm::glm)
target_link_libraries(tutorialopengl PRIVATE glfw)
target_link_libraries(tutorialopengl PRIVATE imgui::imgui)

# own libs

#target_include_directories(tutorialopengl windowinglib)
target_link_libraries(tutorialopengl PRIVATE windowing)
install(TARGETS tutorialopengl DESTINATION bin)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

if(UNIX)
    add_custom_command(
        TARGET tutorialopengl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink              
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
    )

else()
    add_custom_target(copy_resources)

    add_custom_command(
	    TARGET copy_resources PRE_BUILD 
	    COMMAND ${CMAKE_COMMAND} -E copy_directory
	    ${CMAKE_CURRENT_SOURCE_DIR}/resources
	    ${CMAKE_CURRENT_BINARY_DIR}/resources
    )

    add_dependencies(tutorialopengl copy_resources)

endif(UNIX)
