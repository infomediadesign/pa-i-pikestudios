#include "winglviewporthooks.h"

#include <windows.h>
#include <tchar.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_win32.h>

static HGLRC g_hRC;
static GLHooks::WGL_WindowData g_MainWindow;
static int g_Width;
static int g_Height;

bool GLHooks::CreateDeviceWGL(std::uintptr_t hWnd, WGL_WindowData* data)
{
	HWND l_hWnd = reinterpret_cast<HWND>(hWnd);
	
	HDC hDc					  = ::GetDC(l_hWnd);
	PIXELFORMATDESCRIPTOR pfd = {0};
	pfd.nSize				  = sizeof(pfd);
	pfd.nVersion			  = 1;
	pfd.dwFlags				  = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType			  = PFD_TYPE_RGBA;
	pfd.cColorBits			  = 32;

	const int pf = ::ChoosePixelFormat(hDc, &pfd);
	if ( pf == 0 )
		return false;
	if ( ::SetPixelFormat(hDc, pf, &pfd) == FALSE )
		return false;
	::ReleaseDC(l_hWnd, hDc);

	data->hDC = ::GetDC(l_hWnd);
	if ( !g_hRC )
		g_hRC = wglCreateContext(data->hDC);
	return true;
}

void GLHooks::CleanupDeviceWGL(std::uintptr_t hWnd, WGL_WindowData* data)
{
	wglMakeCurrent(nullptr, nullptr);
	::ReleaseDC(reinterpret_cast<HWND>(hWnd), data->hDC);
}

void GLHooks::Hook_Renderer_CreateWindow(ImGuiViewport* viewport)
{
	assert(viewport->RendererUserData == NULL);

	WGL_WindowData* data = IM_NEW(WGL_WindowData);
	CreateDeviceWGL(reinterpret_cast<std::uintptr_t>((HWND) viewport->PlatformHandle), data);
	viewport->RendererUserData = data;
}

void GLHooks::Hook_Renderer_DestroyWindow(ImGuiViewport* viewport)
{
	if ( viewport->RendererUserData != NULL ) {
		WGL_WindowData* data = (WGL_WindowData*) viewport->RendererUserData;
		CleanupDeviceWGL(reinterpret_cast<std::uintptr_t>((HWND) viewport->PlatformHandle), data);
		IM_DELETE(data);
		viewport->RendererUserData = NULL;
	}
}

void GLHooks::Hook_Platform_RenderWindow(ImGuiViewport* viewport, void*)
{
	// Activate the platform window DC in the OpenGL rendering context
	if ( WGL_WindowData* data = (WGL_WindowData*) viewport->RendererUserData )
		wglMakeCurrent(data->hDC, g_hRC);
}

void GLHooks::Hook_Renderer_SwapBuffers(ImGuiViewport* viewport, void*)
{
	if ( WGL_WindowData* data = (WGL_WindowData*) viewport->RendererUserData )
		::SwapBuffers(data->hDC);
}
