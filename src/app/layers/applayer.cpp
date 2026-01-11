#include "applayer.h"
#include "debuglayer.h"
#include "pauselayer.h"

#include <iostream>
#include <memory>
#include <pscore/application.h>
#include <raylib.h>

#include <psinterfaces/renderable.h>

#include <thread>
#include <vector>

#include <entities/player.h>
#include <misc/mapborderinteraction.h>

class AppLayerPriv
{
	friend class AppLayer;

	// WARNING: DO NOT DO THIS.. this is only a temporary solution to try things out. A layer should not be responsible for entites
	std::vector<std::shared_ptr<Player>> players;
};

AppLayer::AppLayer()
{
	_p = std::make_unique<AppLayerPriv>();

	auto initial_player = std::make_shared<Player>();
	_p->players.push_back(initial_player);

	gApp()->register_entity(initial_player);
	renderer_->submit_renderable<Player>(initial_player);
}

std::shared_ptr<Player> AppLayer::spawn_player(const Vector2& position)
{
	auto new_player = std::make_shared<Player>();
	new_player->set_position(position);

	_p->players.push_back(new_player);

	gApp()->register_entity(new_player);
	renderer_->submit_renderable<Player>(new_player);

	return new_player;
}

void AppLayer::destroy_player(std::shared_ptr<Player> player)
{
	renderer_->remove_rendarble<Player>(player);
	gApp()->unregister_entity(player);
	auto& players = _p->players;
	players.erase(std::remove(players.begin(), players.end(), player), players.end());
}

void AppLayer::sync_player_entities()
{
	for ( size_t i = 0; i < _p->players.size(); i++ ) {
		if ( i == 0 ) {
			_p->players[i]->set_is_klone(false);
		} else {
			_p->players[i]->set_is_klone(true);
			_p->players[i]->set_velocity(_p->players[0]->velocity());
			_p->players[i]->set_rotation(_p->players[0]->rotation());
		}
	}
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
	}

	if ( !active )
		return;

	for ( auto entity: PSCore::Application::get()->entities() ) {
		if ( auto locked_entity = entity.lock() )
			locked_entity->update(dt);
		if ( auto player = dynamic_cast<Player*>(entity.lock().get()) ) {
			map_border_wrap_around(*player);
		}
	}

	sync_player_entities();
	process_offscreen_entities();
}

void AppLayer::on_render()
{
	if ( renderer_ )
		renderer_->render();
}
