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

class AppLayerPriv
{
	friend class AppLayer;

	// WARNING: DO NOT DO THIS.. this is only a temporary solution to try things out. A layer should not be responsible for entites
	Player* player = new Player();
};

AppLayer::AppLayer()
{
	_p = std::make_unique<AppLayerPriv>();
}

AppLayer::~AppLayer()
{
	delete _p->player;
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
	}

	if ( !active )
		return;

	for ( auto entity : PSCore::Application::get()->entities() ) {
		entity->update(dt);
	}
}

void AppLayer::on_render()
{
	for ( auto entity : PSCore::Application::get()->entities() ) {
		if ( auto renderable = dynamic_cast<PSInterfaces::IRenderable*>(entity)) {
			renderable->render();
		}
	}
}
