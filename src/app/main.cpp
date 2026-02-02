#include <entities/director.h>
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <psinterfaces/layer.h>

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

	PSCore::Application::AppSpec spec{"Fortunas Echo", {1440, 780}};

	PSCore::Application app(spec);
	if ( start_with_menu )
		app.push_layer<MainMenuLayer>();
	else
		app.push_layer<AppLayer>();
	
	app.add_game_director<FortunaDirector>();
	app.run();

	return 0;
}
