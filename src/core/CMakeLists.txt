set(HEADER_FILES
	include/PSCore/utils.h
)

set(SOURCE_FILES
	utils.cpp
)

add_library(PSCore STATIC ${SOURCE_FILES} ${HEADER_FILES}) # TODO: Build this as a shared lib
target_include_directories(PSCore PUBLIC include)