#include <entities/director.h>
#include <layers/applayer.h>
#include <memory>
#include <misc/mapborderinteraction.h>
#include <pscore/application.h>

#include <imgui.h>
#include <entities/shark.h>
#include "psinterfaces/entity.h"

class FortunaDirectorPriv
{
	friend class FortunaDirector;
	std::vector<std::shared_ptr<Player>> players;
	bool on_screen_warp_around = true;

	std::shared_ptr<Shark> test_shark = std::make_shared<Shark>();
};

FortunaDirector::FortunaDirector() : PSInterfaces::IEntity("fortuna_director")
{
	_p = std::make_unique<FortunaDirectorPriv>();

	auto initial_player = std::make_shared<Player>();
	_p->players.push_back(initial_player);

	gApp()->register_entity(initial_player);
	gApp()->register_entity(_p->test_shark);

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		app_layer->renderer()->submit_renderable<Player>(initial_player);
		app_layer->renderer()->submit_renderable<Shark>(_p->test_shark);
	}
}

FortunaDirector::~FortunaDirector()
{
}

void FortunaDirector::update(float dt)
{
	misc::map::process_off_screen_entities();
	sync_player_entities();
}

void FortunaDirector::draw_debug()
{
	if ( ImGui::Checkbox("On Screen Wrap", &_p->on_screen_warp_around) ) {
		misc::map::set_wrap_around_mode(_p->on_screen_warp_around);
	}
}

std::shared_ptr<Player> FortunaDirector::spawn_player(const Vector2& position)
{
	auto new_player = std::make_shared<Player>();
	new_player->set_position(position);

	_p->players.push_back(new_player);

	gApp()->register_entity(new_player);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->submit_renderable<Player>(new_player);

	return new_player;
}

void FortunaDirector::destroy_player(std::shared_ptr<Player> player)
{
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->remove_rendarble<Player>(player);

	gApp()->unregister_entity(player);
	auto& players = _p->players;
	players.erase(std::remove(players.begin(), players.end(), player), players.end());
}

void FortunaDirector::sync_player_entities()
{
	for ( size_t i = 0; i < _p->players.size(); i++ ) {
		if ( i == 0 ) {
			_p->players[i]->set_is_clone(false);
		} else {
			_p->players[i]->set_is_clone(true);
			_p->players[i]->set_velocity(_p->players[0]->velocity());
			_p->players[i]->set_rotation(_p->players[0]->rotation());
		}
	}
}
