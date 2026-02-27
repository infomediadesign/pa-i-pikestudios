#include <chrono>
#include <iostream>
#include <memory>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <psinterfaces/entity.h>
#include <psinterfaces/layer.h>
#include <stdexcept>

#include <pscore/application.h>
#include <pscore/settings.h>
#include <pscore/time.h>
#include <psinterfaces/entity.h>
#include <raylib.h>

using PSCore::Application;
static Application* g_app = nullptr;

class PSCore::ApplicationPriv
{
	friend class Application;
	bool m_running = false;

	void toggle_fullscreen()
	{
#ifdef _WIN32 // on modern windows a "borderless" fullscreen is better
		ToggleBorderlessWindowed();
#elif defined unix // on linux the default fullscreen behavior works just fine
		int display = GetCurrentMonitor();
		if ( IsWindowFullscreen() )
			SetWindowSize(GetScreenWidth(), GetScreenHeight());
		else
			SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));

		ToggleFullscreen();
#endif
	}

	void handle_global_inputs()
	{
		// Toggle fullscreen on f11
		if ( IsKeyPressed(KEY_F11) ) {
			toggle_fullscreen();
		}
	}

	static void print_log_prefix(int type)
	{
		{ // Print current time stamp
			using namespace std::chrono;
			auto local = zoned_time{current_zone(), system_clock::now()};
			std::cout << local;
		}

		switch ( type ) { // Print the log level
			case LOG_INFO:
				std::cout << "\x1B[32m" << (" [INFO] : ");
				break;
			case LOG_ERROR:
				std::cout << "\x1B[31m" << (" [ERROR]: ");
				break;
			case LOG_WARNING:
				std::cout << "\x1B[33m" << (" [WARN] : ");
				break;
			case LOG_DEBUG:
				std::cout << (" [DEBUG]: ");
				break;
			default:
				break;
		}
	}

	// used as the log callback for raylib
	static void log_callback(int type, const char* text, va_list args)
	{
		print_log_prefix(type);
		vprintf(text, args);
		std::cout << "\033[0m" << std::endl;
	}

	std::unique_ptr<PSCore::DeltaTimeManager> m_time_manager	   = std::make_unique<PSCore::DeltaTimeManager>();
	std::unique_ptr<PSCore::Viewport> m_viewport				   = std::make_unique<PSCore::Viewport>();
	std::unique_ptr<PSCore::sprites::SpriteLoader> m_sprite_loader = std::make_unique<PSCore::sprites::SpriteLoader>();

	std::unique_ptr<SunLight> m_sunlight_shader = std::make_unique<SunLight>();
};

Application::Application()
{
	_p = std::make_unique<ApplicationPriv>();

	g_app = this;
}

void PSCore::Application::init(const AppSpec& spec)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogCallback(_p->log_callback);

	if ( CFG_VALUE<bool>("enable_vsync", false) )
		SetConfigFlags(FLAG_VSYNC_HINT);

	if ( CFG_VALUE<bool>("enable_msaa", false) )
		SetConfigFlags(FLAG_MSAA_4X_HINT);

	if ( CFG_VALUE<bool>("default_fullscreeen", true) )
		_p->toggle_fullscreen();

	InitWindow(spec.size.x, spec.size.y, spec.title);

	SetWindowIcon(LoadImage(spec.icon_path));

	SetExitKey(KEY_NULL);
};

Application::~Application()
{
	CloseWindow();
}

void Application::run()
{
	_p->m_running = true;

	// main event loop
	while ( _p->m_running ) {
		PollInputEvents();

		_p->handle_global_inputs();

		if ( WindowShouldClose() ) {
			stop();
			break;
		}

		while ( m_call_stack.size() > 0 ) {
			m_call_stack.at(0)();
			m_call_stack.pop_front();
		}

		try { // call the update of every layer
			auto dt = _p->m_time_manager->delta_t().count();
			if ( m_game_director )
				m_game_director->update(dt);

			_p->m_viewport->update(dt);

			for ( int i = 0; i < m_layer_stack.size(); ++i )
				m_layer_stack.at(i)->on_update(dt);
		} catch ( std::out_of_range e ) {
			PS_LOG(LOG_WARNING, "Tried calling update on invalid layer.");
		}

		BeginDrawing();
		ClearBackground(BLANK);

		// call render of every layer
		for ( const std::unique_ptr<PSInterfaces::Layer>& layer: m_layer_stack )
			layer->on_render();

		_p->m_viewport->render();

		EndDrawing();
		SwapScreenBuffer();

		_p->m_time_manager->calc_delta_t();
	}
}

void Application::stop()
{
	_p->m_running = false;
}

Application* Application::get()
{
	return g_app;
}

void Application::log(TraceLogLevel type, const char* text) const
{
	_p->print_log_prefix(type);
	std::cout << text << "\033[0m" << std::endl;
}

std::unique_ptr<PSCore::Viewport>& PSCore::Application::viewport()
{
	return _p->m_viewport;
};

std::unique_ptr<PSCore::sprites::SpriteLoader>& PSCore::Application::sprite_loader()
{
	return _p->m_sprite_loader;
}

void Application::set_current_player_name(std::string& name)
{
	m_current_player_name = name;
}

std::string Application::current_player_name()
{
	return m_current_player_name;
}

float PSCore::Application::delta_time()
{
	return _p->m_time_manager->delta_t().count();
};

void PSCore::Application::set_sunlight_shader(const char* shader_path)
{
	_p->m_sunlight_shader->shader = LoadShader(nullptr, shader_path);
	_p->m_sunlight_shader->update_shader();
}

SunLight* PSCore::Application::sunlight_shader() const
{
	return _p->m_sunlight_shader.get();
}
