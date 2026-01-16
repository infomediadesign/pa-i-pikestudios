if (WIN32)
    set(app_icon "${PROJECT_SOURCE_DIR}/ressources/appIcon.rc")
endif()

file(GLOB_RECURSE HEADER_FILES
     "./*.h"
)

file(GLOB_RECURSE SOURCE_FILES
     "./*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${app_icon} "entities/cannon.h" "entities/cannon.cpp")

target_link_libraries(${PROJECT_NAME} PUBLIC
 imgui
 raylib
 rlimgui
 OpenGL::GL

	PSCore
	CMathExtension
)

target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_FMT_EXTERNAL)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
