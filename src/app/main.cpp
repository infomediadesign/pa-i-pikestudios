#include <entities/director.h>
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <psinterfaces/layer.h>

// Disable the console in Windows releases
# if defined(WIN32) && !defined(_DEBUG)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char* argv[])
{
	bool start_with_menu = true;
#ifndef NDEBUG
	for ( int i = 0; i < argc; i++ ) {
		std::string arg = argv[i];
		if ( arg == "-s" )
			start_with_menu = false;
	}
#endif

	PSCore::Application::AppSpec spec{"Fortunas Echo", "resources/appicon.png", {1440, 780}};

	PSCore::Application app;
	app.init(spec);
	if ( start_with_menu )
		app.push_layer<MainMenuLayer>();
	else
		app.push_layer<AppLayer>();
	
	app.add_game_director<FortunaDirector>();
	app.run();

	return 0;
}
