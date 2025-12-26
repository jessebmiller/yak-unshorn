include(FetchContent)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-cursor wayland-egl xkbcommon)
pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

set(XDG_SHELL_XML "${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml")
set(XDG_SHELL_H "${CMAKE_CURRENT_BINARY_DIR}/xdg-shell.h")
set(XDG_SHELL_C "${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c")

add_custom_command(
    OUTPUT "${XDG_SHELL_H}"
    COMMAND ${WAYLAND_SCANNER} client-header ${XDG_SHELL_XML} ${XDG_SHELL_H}
    DEPENDS ${XDG_SHELL_XML}
)

add_custom_command(
    OUTPUT "${XDG_SHELL_C}"
    COMMAND ${WAYLAND_SCANNER} private-code ${XDG_SHELL_XML} ${XDG_SHELL_C}
    DEPENDS ${XDG_SHELL_XML}
)

FetchContent_Declare(
  RGFW
  GIT_REPOSITORY https://github.com/ColleagueRiley/RGFW.git
  GIT_TAG        1.8.1
)
FetchContent_MakeAvailable(RGFW)

add_executable(ox main.c "${XDG_SHELL_C}" "${XDG_SHELL_H}")

target_compile_definitions(ox PRIVATE RGFW_WAYLAND)

target_include_directories(ox PRIVATE 
    ${rgfw_SOURCE_DIR} 
    ${WAYLAND_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(ox PRIVATE ${WAYLAND_LIBRARIES} m dl)
