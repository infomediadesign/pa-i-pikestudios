#include <chrono>
#include <functional>
#include <pscore/application.h>
#include <raylib.h>
#include <stdexcept>
#include <string>

using PSCore::Application;
static Application* g_app = nullptr;

class PSCore::ApplicationPriv
{
	friend class Application;
	bool m_running = false;

	float deltaTime		 = 0.0f;
	float currentTime	 = GetTime();
	float updateDrawTime = 0.0f, previousTime = 0.0f, waitTime = 0.0f;

	int targetFPS	  = 0;
	float timeCounter = 0.0f;

	void calc_delta_t()
	{
		currentTime	   = GetTime();
		updateDrawTime = currentTime - previousTime;

		if ( targetFPS > 0 ) // We want a fixed frame rate
		{
			waitTime = (1.0f / targetFPS) - updateDrawTime;
			if ( waitTime > 0.0 ) {
				WaitTime(float{waitTime});
				currentTime = GetTime();
				deltaTime	= float{(currentTime - previousTime)};
			}
		} else
			deltaTime = updateDrawTime;
	}

	void handle_global_inputs()
	{
		if ( IsKeyPressed(KEY_F11) ) {
#ifdef _WIN32
			ToggleBorderlessWindowed();
#elif defined unix
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

		switch ( type ) {
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

	static void log_callback(int type, const char* text, va_list args)
	{
		print_log_prefix(type);
		vprintf(text, args);
		std::cout << std::endl;
	}
};

Application::Application(const AppSpec& spec)
{
	_p = std::make_unique<ApplicationPriv>();

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogCallback(_p->log_callback);

	g_app = this;

	InitWindow(spec.size.x, spec.size.y, spec.title);
	PS_LOG(LOG_WARNING, "lol");

	SetExitKey(KEY_NULL);
}

Application::~Application()
{
	CloseWindow();
}

void Application::run()
{
	_p->m_running = true;

	while ( _p->m_running ) {
		_p->timeCounter += _p->deltaTime;

		PollInputEvents();

		_p->handle_global_inputs();

		if ( WindowShouldClose() ) {
			stop();
			break;
		}

		for ( auto itr = m_entity_registry.begin(); itr != m_entity_registry.end(); ) {
			if ( auto r_locked = itr->lock() ) {
				++itr;
			} else // entity is expired; we dont need it
				itr = m_entity_registry.erase(itr);
		}

		try {
			for ( int i = 0; i < m_layer_stack.size(); ++i )
				m_layer_stack.at(i)->on_update(_p->deltaTime);
		} catch ( std::out_of_range e ) {
		}

		BeginDrawing();
		ClearBackground(BLANK);

		for ( const std::unique_ptr<PSInterfaces::Layer>& layer: m_layer_stack )
			layer->on_render();

		EndDrawing();
		SwapScreenBuffer();

		_p->calc_delta_t();
		_p->previousTime = _p->currentTime;
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
