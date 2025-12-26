add_library(init SHARED init.c)

set_target_properties(init PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/module/init
)
