if (WIN32)
	set(app_icon "${PROJECT_SOURCE_DIR}/ressources/appIcon.rc")
endif()

set(SOURCE_FILES
	main.cpp
	debuglayer.cpp
	applayer.cpp
	pauselayer.cpp
	#winglviewporthooks.cpp
)

set(HEADER_FILES
	#winglviewporthooks.h
	debuglayer.h
	applayer.h
	pauselayer.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${app_icon})

target_link_libraries(${PROJECT_NAME} PUBLIC
	EnTT::EnTT
	imgui
	raylib
	rlimgui
	spdlog::spdlog_header_only
	OpenGL::GL

	PSCore
)

target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_FMT_EXTERNAL)
