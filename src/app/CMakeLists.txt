if (WIN32)
	set(app_icon "${PROJECT_SOURCE_DIR}/ressources/appIcon.rc")
endif()

set(SOURCE_FILES
	main.cpp
	#winglviewporthooks.cpp
)

set(HEADER_FILES
	#winglviewporthooks.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${app_icon})

target_link_libraries(${PROJECT_NAME} PUBLIC
	raylib
	EnTT::EnTT
	imgui
	raylib
	rlimgui
	spdlog::spdlog_header_only
	OpenGL::GL

	PSCore
)

target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_FMT_EXTERNAL)
