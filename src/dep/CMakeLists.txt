file(GLOB_RECURSE HEADER_FILES
     "./*.h"
)

add_library(raygui INTERFACE)
target_include_directories(raygui INTERFACE ./)
target_link_libraries(raygui INTERFACE raylib)