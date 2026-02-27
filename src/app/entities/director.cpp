#include <cstdint>
#include <entities/cannon.h>
#include <entities/director.h>
#include <entities/projectile.h>
#include <layers/applayer.h>
#include <layers/upgradelayer.h>
#include <memory>
#include <misc/mapborderinteraction.h>
#include <pscore/application.h>

#include <entities/shark.h>
#include <imgui.h>
#include <pscore/spawner.h>
#include <pscore/utils.h>
#include <psinterfaces/entity.h>
#include <raylib.h>
#include <raymath.h>
#include <entities/hunter.h>
#include <entities/player.h>
#include <entities/tentacle.h>
#include "entities/chonkyshark.h"
#include "entities/lootchest.h"
#include "entities/tentacle.h"


FortunaDirector::FortunaDirector() : PSInterfaces::IEntity("fortuna_director")
{
	_p = std::make_unique<FortunaDirectorPriv>();

	_p->shark_spawner = std::make_unique<PSCore::Spawner<Shark, AppLayer>>(_p->shark_spawn_time, _p->shark_spawn_variation, _p->shark_limit, false);
	_p->projectile_spawner = std::make_unique<PSCore::Spawner<Projectile, AppLayer>>(0.0f, 0, INT32_MAX);
	_p->tentacle_spawner =
			std::make_unique<PSCore::Spawner<tentacle, AppLayer>>(_p->tentacle_spawn_time, _p->tentacle_spawn_variation, _p->tentacle_limit);
	_p->chonky_shark_spawner = std::make_unique<PSCore::Spawner<ChonkyShark, AppLayer>>(
			_p->chonky_shark_spawn_time, _p->chonky_shark_spawn_variation, _p->chonky_shark_limit
	);
	_p->hunter_spawner = std::make_unique<PSCore::Spawner<Hunter, AppLayer>>(_p->hunter_spawn_time, _p->hunter_spawn_variation, _p->hunter_limit);
}

void FortunaDirector::initialize_entities()
{
	AppLayer* app_layer = nullptr;
	app_layer			= gApp()->get_layer<AppLayer>();
	if ( !app_layer )
		return;

	auto set_shark_spawn = [](std::shared_ptr<Shark> shark) {
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
	};

	_p->shark_spawner->register_spawn_callback([set_shark_spawn](std::shared_ptr<Shark> shark) { set_shark_spawn(shark); });

	_p->chonky_shark_spawner->register_spawn_callback([set_shark_spawn](std::shared_ptr<ChonkyShark> shark) { set_shark_spawn(shark); });

	_p->tentacle_spawner->register_spawn_callback([](std::shared_ptr<tentacle> tentacle) {
		tentacle->init(tentacle, {(float) PSUtils::gen_rand(10, 300), (float) PSUtils::gen_rand(10, 300)});
	});

	_p->hunter_spawner->register_spawn_callback([](std::shared_ptr<Hunter> hunter) {
		hunter->init(hunter);
	});

	_p->shark_spawner->resume();

	auto initial_player = std::make_shared<Player>();
	_p->players.push_back(initial_player);
	initial_player->set_shared_ptr_this(initial_player);

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		app_layer->register_entity(initial_player, true);
	}
	initial_player->add_cannons(2);

	upgrade_player_invincibility(_p->player_invincibility);
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
	_p->chonky_shark_spawner->update(dt);
	_p->tentacle_spawner->update(dt);
	_p->hunter_spawner->update(dt);
}

void FortunaDirector::draw_debug()
{
	if ( ImGui::Checkbox("On Screen Wrap", &_p->on_screen_warp_around) ) {
		misc::map::set_wrap_around_mode(_p->on_screen_warp_around);
	}

	_p->shark_spawner->draw_debug();
	_p->chonky_shark_spawner->draw_debug();
	_p->hunter_spawner->draw_debug();
	_p->tentacle_spawner->draw_debug();

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

	// Get Upgrade
	ImGui::Separator();

	if ( ImGui::Button("Get Upgrade") ) {
		if ( gApp()->get_layer<UpgradeLayer>() ) {
			gApp()->call_later([]() {
				auto upgrade_layer = gApp()->get_layer<UpgradeLayer>();
				if ( upgrade_layer ) {
					upgrade_layer->m_current_loot_table_values = upgrade_layer->m_loot_table.loot_table_values(3);
					upgrade_layer->print_loot_table_values(upgrade_layer->m_current_loot_table_values);
				}
				auto app_layer = gApp()->get_layer<AppLayer>();
				if ( app_layer )
					app_layer->suspend();
			});
		} else {
			gApp()->call_later([]() { gApp()->push_layer<UpgradeLayer>(); });
			gApp()->call_later([]() {
				auto upgrade_layer = gApp()->get_layer<UpgradeLayer>();
				if ( upgrade_layer ) {
					upgrade_layer->m_current_loot_table_values = upgrade_layer->m_loot_table.loot_table_values(3);
					upgrade_layer->print_loot_table_values(upgrade_layer->m_current_loot_table_values);
				}
				auto app_layer = gApp()->get_layer<AppLayer>();
				if ( app_layer )
					app_layer->suspend();
			});
		}
	}

	// Add Bounty
	static int bounty_amount = 100;
	ImGui::Text("Bounty: %i", m_b_bounty.bounty());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	ImGui::InputInt("##bounty_amount", &bounty_amount);
	ImGui::SameLine();
	if ( ImGui::Button("Add Bounty##Bounty") ) {
		m_b_bounty.add_bounty(bounty_amount);
	}

	// Spawn Loot Chest
	if ( ImGui::Button("Spawn Loot Chest") ) {
		spawn_loot_chest({(float) PSUtils::gen_rand(10, 300), (float) PSUtils::gen_rand(10, 300)});
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
		upgrade_player_health(health_amount);
	}

	// Add Projectile amount
	static int projectile_amount = 1;
	ImGui::Text("Projectile Amount: %d", player_projectile_amount());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	ImGui::InputInt("##projectile_amount", &projectile_amount);
	ImGui::SameLine();
	if ( ImGui::Button("Upgrade##ProjectileAmount") ) {
		upgrade_player_projectile_amount(projectile_amount);
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
			_p->players[i]->set_velocity(_p->players[0]->velocity().value_or({0, 0}));
			_p->players[i]->set_rotation(_p->players[0]->rotation().value_or(0));
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
			cannon->set_projectile_piercing_chance(_p->player_current_piercing_chance);
			cannon->set_shared_ptr_this(cannon);
			return cannon;
		}
	}
	auto new_cannon = std::make_shared<Cannon>();
	new_cannon->set_position(position);
	new_cannon->set_fire_rate(_p->player_current_fire_rate);
	new_cannon->set_projectile_speed(_p->player_current_projectile_speed);
	new_cannon->set_range(_p->player_current_fire_range);
	new_cannon->set_projectile_piercing_chance(_p->player_current_piercing_chance);
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

std::shared_ptr<LootChest> FortunaDirector::spawn_loot_chest(const Vector2& position)
{
	for ( auto loot: _p->loot_chests ) {
		if ( !loot->is_active() ) {
			loot->init(position, loot);
			loot->set_is_active(true);
			return loot;
		}
	}
	auto new_loot = std::make_shared<LootChest>();
	_p->loot_chests.push_back(new_loot);

	if ( auto app_layer = gApp()->get_layer<AppLayer>() )
		app_layer->register_entity(new_loot, true);

	new_loot->init(position, new_loot);
	return new_loot;
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

void FortunaDirector::upgrade_player_health(int amount)
{
	if ( player_health() + amount > player_max_health() ) {
		set_player_max_health(player_max_health() + amount);
		set_player_health(player_health() + amount);
	} else {
		set_player_health(player_health() + amount);
	}
}

void FortunaDirector::upgrade_player_luck(float amount)
{
	_p->player_current_luck = std::clamp(_p->player_current_luck + amount, -1.0f, 1.0f);
}

void FortunaDirector::upgrade_player_invincibility(bool invincibility)
{
	for ( auto player: _p->players ) {
		player->set_is_invincible(invincibility);
	};
}

void FortunaDirector::upgrade_player_speed(float amount)
{
	_p->player_max_velocity += amount;
	for ( auto player: _p->players ) {
		player->set_max_velocity(_p->player_max_velocity);
	}
}

void FortunaDirector::upgrade_player_rotation_speed(float amount)
{
	_p->player_input_rotation_mult += amount;
	for ( auto player: _p->players ) {
		player->set_input_rotation_multiplier(_p->player_input_rotation_mult);
	}
}

void FortunaDirector::upgrade_player_piercing_chance(float amount)
{
	_p->player_current_piercing_chance += amount;
	for ( auto player: _p->players ) {
		for ( auto& cannon: player->cannon_container() ) {
			cannon->set_projectile_piercing_chance(_p->player_current_piercing_chance);
		}
	}
}

void FortunaDirector::upgrade_player_projectile_amount(int amount)
{
	_p->player_projectile_amount += amount;
}

float FortunaDirector::player_current_fire_rate() const
{
	return _p->player_current_fire_rate;
}

float FortunaDirector::player_current_projectile_speed() const
{
	return _p->player_current_projectile_speed;
}

float FortunaDirector::player_current_fire_range() const
{
	return _p->player_current_fire_range;
}

float FortunaDirector::player_max_velocity() const
{
	return _p->player_max_velocity;
}

float FortunaDirector::player_input_rotation_mult() const
{
	return _p->player_input_rotation_mult;
}

float FortunaDirector::player_piercing_chance() const
{
	return _p->player_current_piercing_chance;
}

void FortunaDirector::set_player_piercing_chance(const int chance)
{
	_p->player_current_piercing_chance = chance;
}

float FortunaDirector::player_luck() const
{
	return _p->player_current_luck;
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

template<>
std::unique_ptr<PSCore::Spawner<tentacle, AppLayer>>& FortunaDirector::spawner()
{
	return _p->tentacle_spawner;
};

template<>
std::unique_ptr<PSCore::Spawner<Hunter, AppLayer>>& FortunaDirector::spawner()
{
	return _p->hunter_spawner;
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
	if ( bounty >= _p->shark_start_increase_difficulty_bounty_amount && bounty <= _p->shark_start_decrease_difficulty_bounty_amount &&
		 bounty <= _p->shark_stop_spawn_bounty_amount ) {
		_p->shark_spawner->set_interval(
				std::max(
						_p->shark_min_spawn_time,
						_p->shark_spawn_time -
								_p->shark_spawn_increase_base_value * (static_cast<float>(bounty) / _p->shark_spawn_increase_bounty_divider)
				)
		);
		_p->shark_spawner->set_limit(std::min(_p->shark_max_limit, _p->shark_limit + (bounty / _p->shark_limit_increase_bounty_divider)));
	}
	// Decrease shark spawn rate and limit if bounty is above the decrease difficulty threshold
	if ( bounty >= _p->shark_start_decrease_difficulty_bounty_amount && bounty <= _p->shark_stop_spawn_bounty_amount ) {
		float bounty_above_threshold = static_cast<float>(bounty - _p->shark_start_decrease_difficulty_bounty_amount);

		float peak_spawn_time = std::max(
				_p->shark_min_spawn_time,
				_p->shark_spawn_time - _p->shark_spawn_increase_base_value * (static_cast<float>(_p->shark_start_decrease_difficulty_bounty_amount) /
																			  _p->shark_spawn_increase_bounty_divider)
		);
		float decreased_spawn_time =
				peak_spawn_time + _p->shark_spawn_increase_base_value * (bounty_above_threshold / _p->shark_spawn_increase_bounty_divider);
		_p->shark_spawner->set_interval(std::min(_p->shark_spawn_time, decreased_spawn_time));

		int decreased_limit = _p->shark_limit + (bounty / _p->shark_limit_increase_bounty_divider);
		int peak_limit		= _p->shark_limit + (_p->shark_start_decrease_difficulty_bounty_amount / _p->shark_limit_increase_bounty_divider);
		_p->shark_spawner->set_limit(
				std::max(_p->shark_limit, peak_limit - (static_cast<int>(bounty_above_threshold) / _p->shark_limit_increase_bounty_divider))
		);
	}
	// Stop shark spawner if bounty is above the stop spawn threshold
	if ( bounty >= _p->shark_stop_spawn_bounty_amount ) {
		_p->shark_spawner->suspend();
	}

	if ( bounty >= _p->chonky_shark_takeover_threshold ) {
		_p->shark_spawner->suspend();
		_p->chonky_shark_spawner->resume();
		_p->chonky_shark_spawner->set_interval(
				std::max(
						_p->chonky_shark_min_spawn_time,
						_p->chonky_shark_spawn_time - _p->chonky_shark_spawn_increase_base_value *
															  (static_cast<float>(bounty) / _p->chonky_shark_spawn_increase_bounty_divider)
				)
		);
		_p->chonky_shark_spawner->set_limit(
				std::min(_p->chonky_shark_max_limit, _p->chonky_shark_limit + (bounty / _p->chonky_shark_limit_increase_bounty_divider))
		);
	}

	// Increase tentacle spawn rate and limit based on bounty
	if ( bounty >= _p->tentacle_start_spawn_bounty_amount ) {
		_p->tentacle_spawner->set_interval(
				std::max(
						_p->tentacle_min_spawn_time,
						_p->tentacle_spawn_time -
								_p->tentacle_spawn_increase_base_value * (static_cast<float>(bounty) / _p->tentacle_spawn_increase_bounty_divider)
				)
		);
		_p->tentacle_spawner->set_limit(std::min(_p->tentacle_max_limit, _p->tentacle_limit + (bounty / _p->tentacle_limit_increase_bounty_divider)));
	}
	// Activate tentacle spawner if bounty threshold is reached
	if ( !_p->m_tentacle_spawn_active && bounty >= _p->tentacle_start_spawn_bounty_amount ) {
		_p->m_tentacle_spawn_active = true;
		_p->tentacle_spawner->resume();
	}

	// Increase hunter spawn rate and limit based on bounty
	if ( bounty >= _p->hunter_start_spawn_bounty_amount ) {
		_p->hunter_spawner->set_interval(
				std::max(
						_p->hunter_min_spawn_time,
						_p->hunter_spawn_time -
								_p->hunter_spawn_increase_base_value * (static_cast<float>(bounty) / _p->hunter_spawn_increase_bounty_divider)
				)
		);
		_p->hunter_spawner->set_limit(std::min(_p->hunter_max_limit, _p->hunter_limit + (bounty / _p->hunter_limit_increase_bounty_divider)));
	}
	// Activate hunter spawner if bounty threshold is reached
	if ( !_p->hunter_spawn_active && bounty >= _p->hunter_start_spawn_bounty_amount ) {
		_p->hunter_spawn_active = true;
		_p->hunter_spawner->resume();
	}
}
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

int FortunaDirector::reroll_amount() const
{
	return _p->upgrade_reroll_amount;
}

void FortunaDirector::set_reroll_amount(const int amount)
{
	_p->upgrade_reroll_amount = amount;
}

<<<<<<< MAGO-225
int FortunaDirector::player_projectile_amount() const
{
	return _p->player_projectile_amount;
}

void FortunaDirector::set_player_projectile_amount(int amount)
{
	_p->player_projectile_amount = amount;
=======
void FortunaDirector::entity_died(std::shared_ptr<PSInterfaces::IEntity> perpetrator, std::string_view died_type)
{
	if ( auto player = std::dynamic_pointer_cast<Player>(perpetrator) ) {
		if ( _p->shark_spawner->entities().size() > 0 && died_type == _p->shark_spawner->entities().at(0)->ident() )
			m_b_bounty.add_bounty(m_b_bounty_amounts.shark_bounty);
		else if ( _p->tentacle_spawner->entities().size() > 0 && died_type == _p->tentacle_spawner->entities().at(0)->ident() )
			m_b_bounty.add_bounty(m_b_bounty_amounts.tentacle_bounty);
		else if ( _p->hunter_spawner->entities().size() > 0 && died_type == _p->hunter_spawner->entities().at(0)->ident() )
			m_b_bounty.add_bounty(m_b_bounty_amounts.ship_bounty);
	};
>>>>>>> main
}
