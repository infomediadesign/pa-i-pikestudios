cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(template VERSION 0.0.1 DESCRIPTION "A Template")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# Provide compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(export-compile-commands ALL
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_BINARY_DIR}/compile_commands.json"
		"${CMAKE_SOURCE_DIR}/compile_commands.json"
		COMMENT "Export compile_commands.json to source root"
)

add_library(raylib_imgui_compiler_flags INTERFACE)

target_compile_features(raylib_imgui_compiler_flags INTERFACE cxx_std_17)
# set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
# set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
# target_compile_options(
#      raylib_imgui_compiler_flags
#      INTERFACE
#          "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-pedantic-errors;-Werror;-Wall;-Weffc++;-Wextra;-Wconversion;-Wsign-conversion>>"
#          "$<${msvc_cxx}:$<BUILD_INTERFACE:-W4>>")

add_compile_definitions(SUPPORT_CUSTOM_FRAME_CONTROL=1)

include(cmake/CPM.cmake)

CPMAddPackage("gh:raysan5/raylib#5.5")
CPMAddPackage("gh:skypjack/entt@3.15.0")

include(cmake/Dependencies.cmake)
raylib_imgui_setup_dependencies()
 
add_subdirectory(src)

file(COPY "${CMAKE_CURRENT_LIST_DIR}/ressources" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
				BUNDLE DESTINATION .
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)

	set(CPACK_GENERATOR "INNOSETUP")

	set(CPACK_PACKAGE_VENDOR "no8f") # Replace
	set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
	set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
	set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME};${PROJECT_NAME})

	set(CPACK_INNOSETUP_USE_MODERN_WIZARD ON)
	set(CPACK_INNOSETUP_ICON_FILE "${PROJECT_SOURCE_DIR}/ressources/raylib.ico") # Change to a proper icon
	set(CPACK_INNOSETUP_RUN_EXECUTABLES ${PROJECT_NAME})

	include(CPack)

endif()
