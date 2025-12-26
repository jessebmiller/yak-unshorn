include(FetchContent)

# 1. Configure Raylib to build for Wayland
set(USE_WAYLAND ON CACHE BOOL "Enable Wayland support")
set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build raylib examples")

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(raylib)

add_executable(ox main.c)

target_link_libraries(ox PRIVATE raylib)

