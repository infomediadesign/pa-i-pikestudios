cmake_minimum_required(VERSION 4.0)

project(FortunasEcho VERSION 0.0.1 DESCRIPTION "A Template")
include(cmake/Config.cmake)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

add_custom_target(export-compile-commands ALL
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_BINARY_DIR}/compile_commands.json"
		"${CMAKE_SOURCE_DIR}/compile_commands.json"
		COMMENT "Export compile_commands.json to source root"
)

add_library(raylib_imgui_compiler_flags INTERFACE)

target_compile_features(raylib_imgui_compiler_flags INTERFACE cxx_std_17)

add_compile_definitions(SUPPORT_CUSTOM_FRAME_CONTROL=1)

include(cmake/CPM.cmake)

include(cmake/Dependencies.cmake)
raylib_imgui_setup_dependencies()

add_subdirectory(src)

file(COPY "${CMAKE_CURRENT_LIST_DIR}/resources" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
				BUNDLE DESTINATION .
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(cmake/Package.cmake)
