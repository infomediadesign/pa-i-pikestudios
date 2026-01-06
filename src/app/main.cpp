#include <pscore/application.h>
#include <psinterfaces/layer.h>
#include <layers/applayer.h>

int main(void)
{
	PSCore::Application::AppSpec spec{"Fortunas Echo", {1440, 780}};

	PSCore::Application app(spec);
	app.push_layer<AppLayer>();
	app.run();

	return 0;
}
