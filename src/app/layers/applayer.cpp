#include "applayer.h"
#include "debuglayer.h"
#include "pauselayer.h"

#include <memory>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raylib.h>

#include <psinterfaces/renderable.h>

#include <entities/player.h>
#include <layers/uilayer.h>
#include <misc/mapborderinteraction.h>

class Water : public PSInterfaces::IRenderable
{
public:
	Water() : PSInterfaces::IEntity("water") {};
	~Water() {};
	void update(float dt) override {};
	void render() override
	{
		if ( auto& vp = gApp()->viewport() ) {
			auto sc = vp->viewport_scale();
			vp->draw_in_viewport(m_water, {0, 0, (float) m_water.width * sc, (float) m_water.height * sc}, {0, 0}, 0, clr);
		}
	}

private:
	Color clr		  = {255, 255, 255, 150};
	Texture2D m_water = LoadTexture("ressources/enviroment/water.png");
};

class AppLayerPriv
{
	friend class AppLayer;

	std::shared_ptr<Water> water = std::make_shared<Water>();
};

AppLayer::AppLayer()
{
	_p = std::make_unique<AppLayerPriv>();
	// Push the UI layer

	gApp()->call_later([]() {
		auto app = PSCore::Application::get();
		if ( !app->get_layer<UILayer>() ) {
			app->push_layer<UILayer>();
		}

		if ( auto director = dynamic_cast<FortunaDirector*>(app->game_director()) )
			director->initialize_entities();
	});

	_p->water->propose_z_index(-10);
	renderer_->submit_renderable(_p->water);
}

AppLayer::~AppLayer()
{
}

void AppLayer::on_update(const float dt)
{
	{ // TODO: Refactor this scope out of the App layer
		auto app = PSCore::Application::get();
		if ( IsKeyPressed(KEY_F3) ) {
			if ( app->get_layer<DebugLayer>() )
				app->pop_layer<DebugLayer>();
			else
				app->push_layer<DebugLayer>();
		}

		if ( IsKeyPressed(KEY_ESCAPE) ) {
			auto& director = gApp()->game_director_ref();
			if ( app->get_layer<PauseLayer>() ) {
				app->pop_layer<PauseLayer>();
				if ( auto app_layer = app->get_layer<AppLayer>() ) {
					app_layer->resume();
					director->set_is_active(true);
				}
			} else {
				app->push_layer<PauseLayer>();
				if ( auto app_layer = app->get_layer<AppLayer>() ) {
					app_layer->suspend();
					director->set_is_active(false);
				}
			}
		}
	}

	if ( !active )
		return;

	for ( const auto& entity: entities() ) {
		if ( auto locked_entity = entity.lock() ) {

			if ( locked_entity->is_active() ) {
				locked_entity->update(dt);
			}

			if ( auto player = dynamic_cast<Player*>(locked_entity.get()) ) {
				misc::map::map_border_wrap_around(*player);
			}
		}
	}
}

void AppLayer::on_render()
{
	if ( renderer_ )
		renderer_->render();
}
