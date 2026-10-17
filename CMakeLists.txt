cmake_minimum_required(VERSION 3.15)
project(renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 3.2 REQUIRED CONFIG)

add_subdirectory(math)
add_subdirectory(geometry)

add_executable(run main.cpp sdl_interface.cpp)
target_link_libraries(run PRIVATE SDL3::SDL3 math geometry)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(run PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Ship both formats. SDL selects a supported driver at runtime; the application
# has no OS-specific rendering code or runtime shader-cross-compiler dependency.
set(SHADER_ASSETS
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.vert.spv"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.frag.spv"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.metal"
)
set_property(TARGET run APPEND PROPERTY LINK_DEPENDS "${SHADER_ASSETS}")
add_custom_command(TARGET run POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:run>/shaders"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        ${SHADER_ASSETS} "$<TARGET_FILE_DIR:run>/shaders"
    VERBATIM
)
