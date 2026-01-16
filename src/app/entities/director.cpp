#include <entities/director.h>
#include <layers/applayer.h>
#include <misc/mapborderinteraction.h>
#include <pscore/application.h>
#include <entities/projectile.h>
#include <entities/cannon.h>

#include <imgui.h>

class FortunaDirectorPriv
{
	friend class FortunaDirector;
	std::vector<std::shared_ptr<Player>> players;
	std::vector<std::shared_ptr<Projectile>> projectiles;
	std::vector<std::shared_ptr<Cannon>> cannons;
	bool on_screen_warp_around = true;
};

FortunaDirector::FortunaDirector()
{
	_p = std::make_unique<FortunaDirectorPriv>();

	auto initial_player = std::make_shared<Player>();
	_p->players.push_back(initial_player);

	gApp()->register_entity(initial_player);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->submit_renderable<Player>(initial_player);

	// Test Cannons
	auto test_cannon = std::make_shared<Cannon>();
	_p->cannons	.push_back(test_cannon);
	gApp()->register_entity(test_cannon);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->submit_renderable<Cannon>(test_cannon);
	test_cannon->set_director(this);
	test_cannon->set_parent(initial_player);
	test_cannon->set_positioning(Cannon::CannonPositioning::Left);

		auto test_cannon_2 = std::make_shared<Cannon>();
	_p->cannons.push_back(test_cannon_2);
	gApp()->register_entity(test_cannon_2);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->submit_renderable<Cannon>(test_cannon_2);
	test_cannon_2->set_director(this);
	test_cannon_2->set_parent(initial_player);
	test_cannon_2->set_positioning(Cannon::CannonPositioning::Right);
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

std::shared_ptr<Projectile> FortunaDirector::spawn_projectile(const Vector2& position)
{
	auto new_projectile = std::make_shared<Projectile>();
	new_projectile->set_position(position);
	_p->projectiles.push_back(new_projectile);
	gApp()->register_entity(new_projectile);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->submit_renderable<Projectile>(new_projectile);
	return new_projectile;
}

void FortunaDirector::destroy_projectile(std::shared_ptr<Projectile> projectile)
{
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->remove_rendarble<Projectile>(projectile);
	gApp()->unregister_entity(projectile);
	auto& projectiles = _p->projectiles;
	projectiles.erase(std::remove(projectiles.begin(), projectiles.end(), projectile), projectiles.end());
}
