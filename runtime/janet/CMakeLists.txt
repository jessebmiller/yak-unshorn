set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_program(JPM_EXECUTABLE jpm REQUIRED)
add_custom_target(ox_janet_module ALL
    COMMAND ${JPM_EXECUTABLE} build --buildpath=${CMAKE_BINARY_DIR}/runtime/janet
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime/janet
    COMMENT "Building Janet runtime with jpm -> ${CMAKE_BINARY_DIR}/runtime/janet"
)

add_library(janet_runtime SHARED janet_runtime.c janet.h janet.c)
target_link_libraries(janet_runtime PRIVATE ox-kernel m ${CMAKE_DL_LIBS})

set_target_properties(janet_runtime PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/runtime/janet
)
