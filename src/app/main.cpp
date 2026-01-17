#include <pscore/application.h>
#include <psinterfaces/layer.h>
#include <layers/applayer.h>
#include <entities/director.h>

int main(void)
{
	PSCore::Application::AppSpec spec{"Fortunas Echo", {1440, 780}};

	PSCore::Application app(spec);
	app.push_layer<AppLayer>();
	app.add_game_director<FortunaDirector>();
	if ( auto director = dynamic_cast<FortunaDirector*>(app.game_director()) )
		director->initialize_entities();
	app.run();

	return 0;
}
