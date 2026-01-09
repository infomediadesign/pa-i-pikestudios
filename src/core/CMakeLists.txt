file(GLOB_RECURSE HEADER_FILES
     "./*.h"
)

file(GLOB_RECURSE SOURCE_FILES
     "./*.cpp"
)

add_library(PSCore STATIC ${SOURCE_FILES} ${HEADER_FILES}) # TODO: Build this as a shared lib
target_include_directories(PSCore PUBLIC include)
target_link_libraries(PSCore PRIVATE
 raylib
 OpenGL::GL
)
