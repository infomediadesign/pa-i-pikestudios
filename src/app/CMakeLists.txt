if (WIN32)
    set(app_icon "${PROJECT_SOURCE_DIR}/ressources/appIcon.rc")
endif()

file(GLOB_RECURSE HEADER_FILES
     "./*.h"
)

file(GLOB_RECURSE SOURCE_FILES
     "./*.cpp"
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

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
