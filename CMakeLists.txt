cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(template VERSION 0.0.1 DESCRIPTION "A Template")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# Provide compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(export-compile-commands ALL
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_BINARY_DIR}/compile_commands.json"
		"${CMAKE_SOURCE_DIR}/compile_commands.json"
		COMMENT "Export compile_commands.json to source root"
)

# OPTIONAL support variable framerate
# add_compile_definitions(SUPPORT_CUSTOM_FRAME_CONTROL=1)

include(cmake/CPM.cmake)

CPMAddPackage("gh:raysan5/raylib#5.5")
# CPMAddPackage("gh:skypjack/entt@3.15.0")

add_subdirectory(src)

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
