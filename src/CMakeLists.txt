if (WIN32)
	set(app_icon "${PROJECT_SOURCE_DIR}/ressources/appIcon.rc")
endif()
add_executable(${PROJECT_NAME} main.cpp ${app_icon})

target_link_libraries(${PROJECT_NAME} PUBLIC
	raylib
	EnTT::EnTT
	imgui
	raylib
	rlimgui
	spdlog::spdlog_header_only

	PSCore
)

target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_FMT_EXTERNAL)


add_subdirectory(core)
