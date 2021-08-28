# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Add source to this project's executable.
add_executable(CMakeProject1 "")

target_sources(CMakeProject1
    PRIVATE
        main.cpp
        main.h
)

target_link_libraries(CMakeProject1 PRIVATE glad::glad)
target_link_libraries(CMakeProject1 PRIVATE glfw)
target_link_libraries(CMakeProject1 PRIVATE imgui::imgui)


if(UNIX)
    add_custom_command(
        TARGET CMakeProject1 POST_BUILD
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

    add_dependencies(CMakeProject1 copy_resources)

endif(UNIX)
