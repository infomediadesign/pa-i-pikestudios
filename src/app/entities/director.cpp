#include <chrono>
#include <entities/cannon.h>
#include <entities/director.h>
#include <entities/projectile.h>
#include <layers/applayer.h>
#include <memory>
#include <misc/mapborderinteraction.h>
#include <pscore/application.h>

#include <entities/shark.h>
#include <imgui.h>
#include "pscore/spawner.h"
#include "pscore/utils.h"
#include "psinterfaces/entity.h"

class FortunaDirectorPriv
{
	friend class FortunaDirector;
	std::vector<std::shared_ptr<Player>> players;
	//std::vector<std::shared_ptr<Projectile>> projectiles;
	std::vector<std::shared_ptr<Cannon>> cannons;
	bool on_screen_warp_around = true;

	// std::shared_ptr<Shark> test_shark = std::make_shared<Shark>();
	float player_current_fire_rate		  = 0.5f;
	float player_current_projectile_speed = 300.0f;
	float player_current_fire_range		  = 100.0f;

	std::unique_ptr<PSCore::Spawner<Shark, AppLayer>> shark_spawner;
	std::unique_ptr<PSCore::Spawner<Projectile, AppLayer>> projectile_spawner;
	float player_max_velocity		 = 200.0f;
	float player_input_rotation_mult = 0.9f;
	float player_input_velocity_mult = 1500;
};

FortunaDirector::FortunaDirector() : PSInterfaces::IEntity("fortuna_director")
{
	_p = std::make_unique<FortunaDirectorPriv>();

	_p->shark_spawner	   = std::make_unique<PSCore::Spawner<Shark, AppLayer>>(std::chrono::duration<double>{10.0f});
	_p->projectile_spawner = std::make_unique<PSCore::Spawner<Projectile, AppLayer>>(std::chrono::duration<double>{0.0f});
}

void FortunaDirector::initialize_entities()
{
	_p->shark_spawner->register_spawn_callback([](std::shared_ptr<Shark> shark) {
		shark->init(shark, {(float) PSUtils::gen_rand(10, 300), (float) PSUtils::gen_rand(10, 300)});
	});

	_p->shark_spawner->resume();

	auto initial_player = std::make_shared<Player>();
	_p->players.push_back(initial_player);
	initial_player->set_shared_ptr_this(initial_player);

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		app_layer->register_entity(initial_player, true);
	}
	initial_player->add_cannons(2);
}


FortunaDirector::~FortunaDirector()
{
}

void FortunaDirector::update(float dt)
{
	misc::map::process_off_screen_entities();
	sync_player_entities();

	_p->shark_spawner->update(dt);
}

void FortunaDirector::draw_debug()
{
	if ( ImGui::Checkbox("On Screen Wrap", &_p->on_screen_warp_around) ) {
		misc::map::set_wrap_around_mode(_p->on_screen_warp_around);
	}

	ImGui::Separator();
	ImGui::Text("Player Speed");

	if ( ImGui::SliderFloat("Max Velocity", &_p->player_max_velocity, 0, 500) ) {
		for ( auto& player: _p->players ) {
			player->set_max_velocity(_p->player_max_velocity);
		}
	}

	if ( ImGui::SliderFloat("Input Velocity Mult.", &_p->player_input_velocity_mult, 0, 3000) ) {
		for ( auto& player: _p->players ) {
			player->set_input_velocity_multiplier(_p->player_input_velocity_mult);
		}
	}

	if ( ImGui::SliderFloat("Input Rotation Mult.", &_p->player_input_rotation_mult, 0, 2) ) {
		for ( auto& player: _p->players ) {
			player->set_input_rotation_multiplier(_p->player_input_rotation_mult);
		}
	}

	ImGui::Separator();
	ImGui::Text("Player Upgrades");

	// Fire Rate
	static float fire_rate_amount = 0.05f;
	ImGui::Text("Fire Rate: %.2f s", _p->player_current_fire_rate);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	ImGui::InputFloat("##fire_rate_amount", &fire_rate_amount, 0.0f, 0.0f, "%.2f");
	ImGui::SameLine();
	if ( ImGui::Button("Upgrade##FireRate") ) {
		upgrade_player_fire_rate(fire_rate_amount);
	}

	// Projectile Speed
	static float speed_amount = 100.0f;
	ImGui::Text("Projectile Speed: %.0f", _p->player_current_projectile_speed);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	ImGui::InputFloat("##speed_amount", &speed_amount, 0.0f, 0.0f, "%.0f");
	ImGui::SameLine();
	if ( ImGui::Button("Upgrade##Speed") ) {
		upgrade_player_projectile_speed(speed_amount);
	}

	// Fire Range
	static float range_amount = 50.0f;
	ImGui::Text("Fire Range: %.0f", _p->player_current_fire_range);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	ImGui::InputFloat("##range_amount", &range_amount, 0.0f, 0.0f, "%.0f");
	ImGui::SameLine();
	if ( ImGui::Button("Upgrade##Range") ) {
		upgrade_player_fire_range(range_amount);
	}

	// Add Cannons
	static int cannon_amount = 1;
	ImGui::Text("Cannons count: %zu", _p->players[0]->cannon_container().size());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	ImGui::InputInt("##cannon_amount", &cannon_amount);
	ImGui::SameLine();
	if ( ImGui::Button("Add Cannons") ) {
		upgrade_player_add_cannon(cannon_amount);
	}
}

std::shared_ptr<Player> FortunaDirector::spawn_player(const Vector2& position)
{

	for ( auto player: _p->players ) {
		if ( !player->is_active() ) {
			player->set_position(position);
			player->set_is_active(true);
			return player;
		}
	}
	auto new_player = std::make_shared<Player>();
	new_player->set_position(position);

	_p->players.push_back(new_player);

	// gApp()->register_entity(new_player);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->register_entity(new_player, true);
	new_player->set_shared_ptr_this(new_player);
	new_player->add_cannons(_p->players[0]->cannon_container().size() / 2);

	return new_player;
}

void FortunaDirector::destroy_player(std::shared_ptr<Player> player)
{

	for ( auto cannon: player->cannon_container() ) {
		cannon->set_is_active(false);
	}

	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->unregister_entity(player);
	// app_layer->renderer()->remove_rendarble<Player>(player);

	// gApp()->unregister_entity(player);
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

// std::shared_ptr<Projectile> FortunaDirector::spawn_projectile(const Vector2& position)
// {

// 	// for ( auto projectile: _p->projectiles ) {
// 	// 	if ( !projectile->is_active() ) {
// 	// 		projectile->set_position(position);
// 	// 		projectile->set_is_active(true);
// 	// 		return projectile;
// 	// 	}
// 	// }

// 	auto new_projectile = std::make_shared<Projectile>();
// 	new_projectile->init(position, new_projectile);

// 	// new_projectile->set_position(position);
// 	//_p->projectiles.push_back(new_projectile);
// 	// gApp()->register_entity(new_projectile);
// 	// if ( auto app_layer = gApp()->get_layer<AppLayer>() )
// 	// 	app_layer->register_entity(new_projectile, true);
// 	// app_layer->renderer()->submit_renderable<Projectile>(new_projectile);
// 	return new_projectile;
// }

// void FortunaDirector::destroy_projectile(std::shared_ptr<Projectile> projectile)
// {
// 	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
// 		app_layer->unregister_entity(projectile);
// 	// app_layer->renderer()->remove_rendarble<Projectile>(projectile);
// 	// gApp()->unregister_entity(projectile);
// 	auto& projectiles = _p->projectiles;
// 	projectiles.erase(std::remove(projectiles.begin(), projectiles.end(), projectile), projectiles.end());
// }

std::shared_ptr<Cannon> FortunaDirector::spawn_cannon(const Vector2& position)
{

	for ( auto cannon: _p->cannons ) {
		if ( !cannon->is_active() ) {
			cannon->set_position(position);
			cannon->set_is_active(true);
			cannon->set_fire_rate(_p->player_current_fire_rate);
			cannon->set_projectile_speed(_p->player_current_projectile_speed);
			cannon->set_range(_p->player_current_fire_range);
			return cannon;
		}
	}
	auto new_cannon = std::make_shared<Cannon>();
	new_cannon->set_position(position);
	new_cannon->set_fire_rate(_p->player_current_fire_rate);
	new_cannon->set_projectile_speed(_p->player_current_projectile_speed);
	new_cannon->set_range(_p->player_current_fire_range);
	_p->cannons.push_back(new_cannon);
	// gApp()->register_entity(new_cannon);
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->register_entity(new_cannon, true);
	// app_layer->renderer()->submit_renderable<Cannon>(new_cannon);
	return new_cannon;
}

void FortunaDirector::destroy_cannon(std::shared_ptr<Cannon> cannon)
{
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->unregister_entity(cannon);
	// app_layer->renderer()->remove_rendarble<Cannon>(cannon);
	// gApp()->unregister_entity(cannon);
	auto& cannons = _p->cannons;
	cannons.erase(std::remove(cannons.begin(), cannons.end(), cannon), cannons.end());
}

void FortunaDirector::upgrade_player_fire_rate(float amount)
{
	_p->player_current_fire_rate -= amount;

	for ( auto player: _p->players ) {
		for ( auto& cannon: player->cannon_container() ) {
			cannon->set_fire_rate(_p->player_current_fire_rate);
		}
	}
}

void FortunaDirector::upgrade_player_projectile_speed(float amount)
{
	_p->player_current_projectile_speed += amount;

	for ( auto player: _p->players ) {
		for ( auto& cannon: player->cannon_container() ) {
			cannon->set_projectile_speed(_p->player_current_projectile_speed);
		}
	}
}

void FortunaDirector::upgrade_player_fire_range(float amount)
{
	_p->player_current_fire_range += amount;

	for ( auto player: _p->players ) {
		for ( auto& cannon: player->cannon_container() ) {
			cannon->set_range(_p->player_current_fire_range);
		}
	}
}

void FortunaDirector::upgrade_player_add_cannon(int amount)
{
	for ( auto player: _p->players ) {
		player->add_cannons(amount);
	}
}

template<>
std::unique_ptr<PSCore::Spawner<Shark, AppLayer>>& FortunaDirector::spawner()
{
	return _p->shark_spawner;
};

template<>
std::unique_ptr<PSCore::Spawner<Projectile, AppLayer>>& FortunaDirector::spawner()
{
	return _p->projectile_spawner;
};