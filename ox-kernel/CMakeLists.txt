include(FetchContent)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(SDL_X11_XSCRNSAVER "OFF")
set(SDL_X11_XTEST      "OFF")
set(SDL_X11_XCURSOR    "OFF")
set(SDL_X11_XINPUT     "OFF")
set(SDL_X11_XFILES     "OFF")
set(SDL_X11_XFIXES     "OFF")
set(SDL_X11_XRANDR     "OFF")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(ox main.c)

target_link_libraries(ox PRIVATE SDL3::SDL3)

