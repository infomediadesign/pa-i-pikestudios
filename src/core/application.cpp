#include <pscore/application.h>
#include <raylib.h>
#include <stdexcept>

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
};

Application::Application(const AppSpec& spec)
{
	_p = std::make_unique<ApplicationPriv>();

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	g_app = this;

	InitWindow(spec.size.x, spec.size.y, spec.title);

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
