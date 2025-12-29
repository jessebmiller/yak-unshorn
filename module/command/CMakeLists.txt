add_library(command SHARED command.c)

set_target_properties(command PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/module/command
)
