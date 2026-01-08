#include "applayer.h"
#include "debuglayer.h"
#include "pauselayer.h"

#include <entt/entity/fwd.hpp>
#include <iostream>
#include <memory>
#include <pscore/application.h>
#include <raylib.h>

#include <psinterfaces/renderable.h>

#include <entt/entt.hpp>
#include <thread>
#include <vector>

#include <entities/player.h>

class Water : public PSInterfaces::IRenderable
{
public:
	Water() { propose_z_index(-1);};

	void render() override
	{
		Color tint = (Color){ 255, 255, 255, 128 };
		DrawTextureEx(tex, {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f },0, 8, tint);
	}

	void update(float dt) override {

	}

	Texture2D tex = LoadTexture("ressources/water.png");
};


class AppLayerPriv
{
	friend class AppLayer;

	// WARNING: DO NOT DO THIS.. this is only a temporary solution to try things out. A layer should not be responsible for entites
	std::shared_ptr<Player> player = std::make_shared<Player>();
	std::shared_ptr<Water> wahtu = std::make_shared<Water>();
};

AppLayer::AppLayer()
{
	_p = std::make_unique<AppLayerPriv>();

	gApp()->register_entity(_p->player);
	renderer_->submit_renderable<Player>(_p->player);

	// WAter test
	renderer_->submit_renderable<Water>(_p->wahtu);
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
			if ( app->get_layer<PauseLayer>() ) {
				app->pop_layer<PauseLayer>();
				if ( auto app_layer = app->get_layer<AppLayer>() )
					app_layer->resume();
			} else {
				app->push_layer<PauseLayer>();
				if ( auto app_layer = app->get_layer<AppLayer>() )
					app_layer->suspend();
			}
		}

		if ( IsKeyPressed(KEY_L) ) {
			_p->wahtu->propose_z_index(_p->wahtu->z_index()+1);
		}
		if ( IsKeyPressed(KEY_K) ) {
			_p->wahtu->propose_z_index(_p->wahtu->z_index()-1);
		}
	}

	if ( !active )
		return;

	for ( auto entity: PSCore::Application::get()->entities() ) {
		if ( auto locked_entity = entity.lock() )
			locked_entity->update(dt);
	}
}

void AppLayer::on_render()
{
	if ( renderer_ )
		renderer_->render();
}
