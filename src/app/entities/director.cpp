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
}

void FortunaDirector::initialize_entities()
{
	auto initial_player = std::make_shared<Player>();
	_p->players.push_back(initial_player);
	initial_player->set_shared_ptr_this(initial_player);

	gApp()->register_entity(initial_player);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->submit_renderable<Player>(initial_player);
	initial_player->initialize_cannons(2);
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

	for (auto player: _p->players)
		{
		if (!player->is_active())
		{
			player->set_position(position);
			player->set_is_active(true);
			printf("Reusing inactive player\n");
			return player;
		}
	}
	printf("Spawning new player\n");
	auto new_player = std::make_shared<Player>();
	new_player->set_position(position);

	_p->players.push_back(new_player);

	gApp()->register_entity(new_player);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->submit_renderable<Player>(new_player);
	new_player->set_shared_ptr_this(new_player);
	new_player->initialize_cannons(_p->players[0]->cannon_container().size() / 2);

	return new_player;
}

void FortunaDirector::destroy_player(std::shared_ptr<Player> player)
{

	for ( auto cannon: player->cannon_container() ) {
		cannon->set_is_active(false);
	}

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

	for (auto projectile: _p->projectiles)
		{
		if (!projectile->is_active())
		{
			projectile->set_position(position);
			projectile->set_is_active(true);
			printf("Reusing inactive projectile\n");
			return projectile;
		}
	}

	printf("Spawning new projectile\n");
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

std::shared_ptr<Cannon> FortunaDirector::spawn_cannon(const Vector2& position)
{

	for ( auto cannon: _p->cannons ) 
	{
		if ( !cannon->is_active() ) 
		{
			cannon->set_position(position);
			cannon->set_is_active(true);
			printf("Reusing inactive cannon\n");
			return cannon;
		}
	}
	printf("Spawning new cannon\n");
	auto new_cannon = std::make_shared<Cannon>();
	new_cannon->set_position(position);
	_p->cannons.push_back(new_cannon);
	gApp()->register_entity(new_cannon);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->submit_renderable<Cannon>(new_cannon);
	return new_cannon;
}

void FortunaDirector::destroy_cannon(std::shared_ptr<Cannon> cannon)
{
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->renderer()->remove_rendarble<Cannon>(cannon);
	gApp()->unregister_entity(cannon);
	auto& cannons = _p->cannons;
	cannons.erase(std::remove(cannons.begin(), cannons.end(), cannon), cannons.end());
}
