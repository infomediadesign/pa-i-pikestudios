function(raylib_imgui_setup_dependencies)
    message(STATUS "Include Dear ImGui")
    FetchContent_Declare(
        ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG cb16be3a3fc1f9cd146ae24d52b615f8a05fa93d) # v1.90.9

    FetchContent_MakeAvailable(ImGui)
    FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

    add_library(
        imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp)
    target_include_directories(imgui INTERFACE ${imgui_SOURCE_DIR})

    message(STATUS "Include spdlog")
    cpmaddpackage("gh:gabime/spdlog#27cb4c76708608465c413f6d0e6b8d99a4d84302"
    )# v1.14.1

    message(STATUS "Include rlImGui")
    FetchContent_Declare(
        rlImGui
        GIT_REPOSITORY https://github.com/raylib-extras/rlImGui
        GIT_TAG d765c1ef3d37cf939f88aaa272a59a2713d654c9)
    FetchContent_MakeAvailable(rlImGui)
    FetchContent_GetProperties(rlImGui SOURCE_DIR RLIMGUI_DIR)

    add_library(rlimgui STATIC ${rlimgui_SOURCE_DIR}/rlImgui.cpp)
    target_link_libraries(rlimgui PRIVATE imgui raylib)
    target_include_directories(rlimgui INTERFACE ${rlimgui_SOURCE_DIR})
endfunction()
