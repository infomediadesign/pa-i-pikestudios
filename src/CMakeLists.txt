if (WIN32)
	set(app_icon "${PROJECT_SOURCE_DIR}/ressources/appIcon.rc")
endif()
add_executable(${PROJECT_NAME} main.cpp ${app_icon})

target_link_libraries(${PROJECT_NAME} raylib )
# EnTT::EnTT
