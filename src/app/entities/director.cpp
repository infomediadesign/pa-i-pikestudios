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
#include <pscore/settings.h>
#include <pscore/spawner.h>
#include <pscore/utils.h>
#include <psinterfaces/entity.h>
#include <raylib.h>
#include <raymath.h>


FortunaDirector::FortunaDirector() : PSInterfaces::IEntity("fortuna_director")
{
	_p = std::make_unique<FortunaDirectorPriv>();

	_p->shark_spawner = std::make_unique<PSCore::Spawner<Shark, AppLayer>>(_p->shark_spawn_time, _p->shark_spawn_variation, _p->shark_limit, false);
	_p->projectile_spawner = std::make_unique<PSCore::Spawner<Projectile, AppLayer>>(0.0f);
	_p->tentacle_spawner	   = std::make_unique<PSCore::Spawner<tentacle, AppLayer>>(5.0f, 3, _p->tentacle_limit);
}

void FortunaDirector::initialize_entities()
{
	AppLayer* app_layer = nullptr;
	app_layer			= gApp()->get_layer<AppLayer>();
	if ( !app_layer )
		return;

	_p->shark_spawner->register_spawn_callback([](std::shared_ptr<Shark> shark) {
		// Set the position of the shark to be spawned outsside of the screen
		float x = 0, y = 0;
		int side = PSUtils::gen_rand(0, 3);
		switch ( side ) {
			case 0: // Top
				y = -50;
				break;
			case 1: // Right
				x = GetScreenWidth() + 50;
				break;
			case 2: // Bottom
				y = GetScreenHeight() + 50;
				break;
			case 3: // Left
				x = -50;
				break;
		}
		// Distribute the position randomly along the chosen side
		if ( side == 0 || side == 2 ) { // Top or Bottom
			x = PSUtils::gen_rand(0, GetScreenWidth());
		} else { // Right or Left
			y = PSUtils::gen_rand(0, GetScreenHeight());
		}

		shark->init(shark, {x, y});
	});

	_p->tentacle_spawner->register_spawn_callback([](std::shared_ptr<tentacle> tentacle) {
		tentacle->init(tentacle, {(float) PSUtils::gen_rand(10, 300), (float) PSUtils::gen_rand(10, 300)});
	});

	_p->shark_spawner->resume();
	_p->tentacle_spawner->resume();

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
	if ( !is_active_ )
		return;

	misc::map::process_off_screen_entities();
	sync_player_entities();

	_p->shark_spawner->update(dt);
	_p->tentacle_spawner->update(dt);
}

void FortunaDirector::draw_debug()
{
	if ( ImGui::Checkbox("On Screen Wrap", &_p->on_screen_warp_around) ) {
		misc::map::set_wrap_around_mode(_p->on_screen_warp_around);
	}

	// ImGui::Separator();
	// ImGui::Text("Spawner");

	_p->shark_spawner->draw_debug();
	// static bool shark_sp_active = true;
	// if ( ImGui::Checkbox("Shark Spawner", &shark_sp_active) ) {
	// 	if ( shark_sp_active )
	// 		_p->shark_spawner->resume();
	// 	else
	// 		_p->shark_spawner->suspend();
	// }

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

	// Fire Mode
	static bool use_sequence_mode = true;
	if ( ImGui::Checkbox("Fire In Sequence", &use_sequence_mode) ) {
		Player::FireMode mode = use_sequence_mode ? Player::FireMode::InSequence : Player::FireMode::SameTime;
		for ( auto& player: _p->players ) {
			player->set_fire_mode(mode);
		}
	}

	ImGui::Separator();
	ImGui::Text("Player Upgrades");

	// Invincibility
	if ( ImGui::Checkbox("invincible", &_p->player_invincibility) ) {
		upgrade_player_invincibility(_p->player_invincibility);
	}

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

	// Add Health
	static int health_amount = 1;
	ImGui::Text("Player Health: %d / %d", player_health(), player_max_health());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	ImGui::InputInt("##health_amount", &health_amount);
	ImGui::SameLine();
	if ( ImGui::Button("Add Health") ) {
		if ( player_health() + health_amount > player_max_health() ) {
			set_player_max_health(player_max_health() + health_amount);
			set_player_health(player_health() + health_amount);
		} else {
			set_player_health(player_health() + health_amount);
		}
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

std::shared_ptr<Cannon> FortunaDirector::spawn_cannon(const Vector2& position)
{

	for ( auto cannon: _p->cannons ) {
		if ( !cannon->is_active() ) {
			cannon->set_position(position);
			cannon->set_is_active(true);
			cannon->set_fire_rate(_p->player_current_fire_rate);
			cannon->set_projectile_speed(_p->player_current_projectile_speed);
			cannon->set_range(_p->player_current_fire_range);
			cannon->set_shared_ptr_this(cannon);
			return cannon;
		}
	}
	auto new_cannon = std::make_shared<Cannon>();
	new_cannon->set_position(position);
	new_cannon->set_fire_rate(_p->player_current_fire_rate);
	new_cannon->set_projectile_speed(_p->player_current_projectile_speed);
	new_cannon->set_range(_p->player_current_fire_range);
	_p->cannons.push_back(new_cannon);

	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->register_entity(new_cannon, true);

	return new_cannon;
}

void FortunaDirector::destroy_cannon(std::shared_ptr<Cannon> cannon)
{
	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->unregister_entity(cannon);

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

void FortunaDirector::upgrade_player_invincibility(bool invincibility)
{
	for ( auto player: _p->players ) {
		player->set_is_invincible(invincibility);
	};
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

// Bounty functions

void FortunaDirector::Bounty::set_bounty(const int amount)
{
	m_b_bounty_amount = amount;
}

int FortunaDirector::Bounty::bounty() const
{
	return m_b_bounty_amount;
}

void FortunaDirector::Bounty::add_bounty(const int amount)
{
	m_b_bounty_amount += amount;
	if ( auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
		director->increase_difficulty(m_b_bounty_amount);
	}
}

void FortunaDirector::Bounty::subtract_bounty(const int amount)
{
	m_b_bounty_amount -= amount;
	if ( auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
		director->increase_difficulty(m_b_bounty_amount);
	}
}

void FortunaDirector::increase_difficulty(int bounty)
{
	// Increase shark spawn rate and limit based on bounty
	if ( bounty >= m_b_bounty_amounts.shark_bounty * 4 ) {
		_p->shark_spawner->set_interval(std::max(0.1f, _p->shark_spawn_time - 0.05f * (bounty / 100)));
		_p->shark_spawner->set_limit(std::min(100, _p->shark_limit + (bounty / 200)));
		printf("Bounty: %d, Shark Spawn Interval: %.2f, Shark Limit: %d\n", bounty, _p->shark_spawner->interval(), _p->shark_spawner->limit());
	}

}

/*
void FortunaDirector::increase_difficulty(int bounty)
{
	// Increase shark spawn rate and limit based on bounty
	_p->shark_spawn_time = std::max(0.5f, _p->shark_spawn_time - 0.05f * (bounty / 100));
	_p->shark_limit = std::min(50, _p->shark_limit + (bounty / 100));
}
*/
// Player Health functions
void FortunaDirector::set_player_health(const int health)
{
	_p->player_health = health;
}

int FortunaDirector::player_health() const
{
	return _p->player_health;
}

void FortunaDirector::set_player_max_health(const int max_health)
{
	_p->player_max_health = max_health;
}

int FortunaDirector::player_max_health() const
{
	return _p->player_max_health;
}

float FortunaDirector::player_iframe_duration() const
{
	return _p->player_iframe_duration;
}
