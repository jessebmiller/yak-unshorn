include(FetchContent)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG	       7f3ae3d57459e59943a4ecfefc8f6277ec6bf540 # v3.2.28
	GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(SDL3)

add_executable(ox ox.c event.c display.c)

target_link_libraries(ox PRIVATE SDL3::SDL3)

