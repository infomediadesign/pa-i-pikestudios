#pragma once

#include <cstdint>
#include <imgui.h>

typedef struct HDC__* HDC;

/*
 * functions from https://github.com/ocornut/imgui/blob/docking/examples/example_win32_opengl3/main.cpp
 */
namespace GLHooks {

	struct WGL_WindowData
	{
		HDC hDC;
	};

	// Helper functions
	bool CreateDeviceWGL(std::uintptr_t hWnd, WGL_WindowData* data);

	void CleanupDeviceWGL(std::uintptr_t hWnd, WGL_WindowData* data);

	void Hook_Renderer_CreateWindow(ImGuiViewport* viewport);

	void Hook_Renderer_DestroyWindow(ImGuiViewport* viewport);

	void Hook_Platform_RenderWindow(ImGuiViewport* viewport, void*);

	void Hook_Renderer_SwapBuffers(ImGuiViewport* viewport, void*);

} // namespace GLHooks
