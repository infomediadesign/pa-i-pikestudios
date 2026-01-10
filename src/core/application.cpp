#include <chrono>
#include <memory>
#include <pscore/application.h>
#include <pscore/time.h>
#include <raylib.h>
#include <stdexcept>


using PSCore::Application;
static Application* g_app = nullptr;

class PSCore::ApplicationPriv
{
	friend class Application;
	bool m_running = false;

	void handle_global_inputs()
	{
		// Toggle fullscreen on f11
		if ( IsKeyPressed(KEY_F11) ) {
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
				std::cout << (" [INFO] : ");
				break;
			case LOG_ERROR:
				std::cout << (" [ERROR]: ");
				break;
			case LOG_WARNING:
				std::cout << (" [WARN] : ");
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
		std::cout << std::endl;
	}

	std::unique_ptr<PSCore::DeltaTimeManager> m_time_manager = std::make_unique<PSCore::DeltaTimeManager>();
};

Application::Application(const AppSpec& spec)
{
	_p = std::make_unique<ApplicationPriv>();

	// Init
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogCallback(_p->log_callback);

	g_app = this;

	InitWindow(spec.size.x, spec.size.y, spec.title);

	SetExitKey(KEY_NULL);
	//
}

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

		// temporary fix for invalidated entities
		for ( auto itr = m_entity_registry.begin(); itr != m_entity_registry.end(); ) {
			if ( auto r_locked = itr->lock() ) {
				++itr;
			} else // entity is expired; we dont need it
				itr = m_entity_registry.erase(itr);
		}

		try { // call the update of every layer
			for ( int i = 0; i < m_layer_stack.size(); ++i )
				m_layer_stack.at(i)->on_update(_p->m_time_manager->delta_t().count());
		} catch ( std::out_of_range e ) {
		}

		BeginDrawing();
		ClearBackground(BLANK);

		// call render of every layer
		for ( const std::unique_ptr<PSInterfaces::Layer>& layer: m_layer_stack )
			layer->on_render();

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

std::vector<std::weak_ptr<PSInterfaces::IEntity>> PSCore::Application::entities() const
{
	return m_entity_registry;
}

void Application::log(TraceLogLevel type, const char* text) const
{
	_p->print_log_prefix(type);
	std::cout << text << std::endl;
}
