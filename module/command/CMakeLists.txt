add_library(command SHARED command.c)
target_link_libraries(command PRIVATE ox-kernel)

set_target_properties(command PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/module/command
)
