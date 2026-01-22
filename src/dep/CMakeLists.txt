file(GLOB_RECURSE HEADER_FILES
     "./*.h"
)

add_library(raygui STATIC ${HEADER_FILES}) # TODO: Build this as a shared lib
set_target_properties(raygui PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(raygui PUBLIC ./)
target_link_libraries(raygui PRIVATE
	raylib
)