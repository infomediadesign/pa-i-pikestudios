file(GLOB_RECURSE HEADER_FILES
     "./*.h"
)

file(GLOB_RECURSE SOURCE_FILES
     "./*.cpp"
)

add_library(CMathExtension STATIC ${SOURCE_FILES} ${HEADER_FILES}) # TODO: Build this as a shared lib
target_include_directories(CMathExtension PUBLIC include)
target_link_libraries(CMathExtension PRIVATE
	raylib
)