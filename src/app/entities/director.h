#pragma once

#include <memory>

#include <entities/cannon.h>
#include <entities/player.h>
#include <entities/projectile.h>
#include <pscore/application.h>
#include <psinterfaces/entity.h>

#include <entities/lootchest.h>
#include <entities/shark.h>
#include <entities/tentacle.h>
#include <layers/applayer.h>
#include <pscore/spawner.h>
#include <string_view>
#include "entities/hunter.h"
#include "entities/chonkyshark.h"

class FortunaDirectorPriv;
class FortunaDirector : public PSInterfaces::IEntity
{
public:
	FortunaDirector();
	~FortunaDirector();

	void initialize_entities();

	void update(float dt) override;

	void draw_debug() override;

	// Functions to spawn and destroy players
	std::shared_ptr<Player> spawn_player(const Vector2& position);
	void destroy_player(std::shared_ptr<Player> player);
	void sync_player_entities();

	void entity_died(std::shared_ptr<PSInterfaces::IEntity> perpetrator, std::string_view died_type);

	// Functions to spawn and destroy cannons
	std::shared_ptr<Cannon> spawn_cannon(const Vector2& position);
	void destroy_cannon(std::shared_ptr<Cannon> cannon);

	std::shared_ptr<LootChest> spawn_loot_chest(const Vector2& position);

	template<class E, PSCore::ILayerDerived L>
	std::unique_ptr<PSCore::Spawner<E, L>>& spawner();

	struct DropChances
	{
		int add_cannon		  = CFG_VALUE<int>("upgrade_add_cannon_chance", 1);
		int projectile_speed  = CFG_VALUE<int>("upgrade_projectile_speed_chance", 13);
		int fire_range		  = CFG_VALUE<int>("upgrade_fire_range_chance", 13);
		int fire_rate		  = CFG_VALUE<int>("upgrade_fire_rate_chance", 13);
		int health			  = CFG_VALUE<int>("upgrade_health_chance", 13);
		int speed			  = CFG_VALUE<int>("upgrade_speed_chance", 13);
		int rotation_speed	  = CFG_VALUE<int>("upgrade_rotation_speed_chance", 13);
		int piercing_chance	  = CFG_VALUE<int>("upgrade_piercing_chance_chance", 13);
		int luck			  = CFG_VALUE<int>("upgrade_luck_chance", 6);
		int projectile_amount = 100; // CFG_VALUE<int>("upgrade_projectile_amount_chance", 1);
	};

	DropChances drop_chances;

	// Upgrade functions
	void upgrade_player_fire_rate(float amount);
	void upgrade_player_projectile_speed(float amount);
	void upgrade_player_fire_range(float amount);
	void upgrade_player_add_cannon(int amount);
	void upgrade_player_invincibility(bool invincibility);
	void upgrade_player_health(int amount);
	void upgrade_player_speed(float amount);
	void upgrade_player_rotation_speed(float amount);
	void upgrade_player_piercing_chance(float amount);
	void upgrade_player_luck(float amount);
	void upgrade_player_projectile_amount(int amount);

	float player_current_fire_rate() const;
	float player_current_projectile_speed() const;
	float player_current_fire_range() const;
	float player_max_velocity() const;
	float player_input_rotation_mult() const;

	void set_player_piercing_chance(const int chance);
	float player_piercing_chance() const;
	float player_luck() const;

	int player_projectile_amount() const;
	void set_player_projectile_amount(const int amount);

	// Player Health
	void set_player_health(const int health);
	int player_health() const;

	void set_player_max_health(const int max_health);
	int player_max_health() const;

	void set_reroll_amount(const int amount);
	int reroll_amount() const;


	// Bounty
	struct Bounty
	{
	public:
		void set_bounty(const int amount);
		int bounty() const;
		void add_bounty(const int amount);
		void subtract_bounty(const int amount);

	private:
		int m_b_bounty_amount = 0;
	};

	void increase_difficulty(int bounty);

	float player_iframe_duration() const;

	struct BountyAmount
	{
		int shark_bounty	= 40;
		int ship_bounty		= 150;
		int tentacle_bounty = 225;
		int big_shark_bounty = 120;
	};
	Bounty m_b_bounty;
	BountyAmount m_b_bounty_amounts;

	std::unique_ptr<FortunaDirectorPriv>& value_container_ref()
	{
		return _p;
	}

private:
	std::unique_ptr<FortunaDirectorPriv> _p;
};

struct FortunaDirectorPriv
{
	std::vector<std::shared_ptr<Player>> players;
	std::vector<std::shared_ptr<Cannon>> cannons;
	std::vector<std::shared_ptr<LootChest>> loot_chests;
	bool on_screen_warp_around = CFG_VALUE<bool>("on_screen_warp_around", true);

	int upgrade_reroll_amount = CFG_VALUE<int>("upgrade_reroll_amount", 3);

	float player_current_fire_rate		  = CFG_VALUE<float>("player_current_fire_rate", 0.5f);
	float player_current_projectile_speed = CFG_VALUE<float>("player_current_projectile_speed", 300.0f);
	float player_current_fire_range		  = CFG_VALUE<float>("player_current_fire_range", 100.0f);
	float player_current_piercing_chance  = CFG_VALUE<float>("player_current_piercing_chance", 5.0f);
	float player_current_luck			  = CFG_VALUE<float>("player_current_luck", 0.1f);
	float player_projectile_amount		  = CFG_VALUE<float>("player_projectile_amount", 1);

	std::unique_ptr<PSCore::Spawner<Shark, AppLayer>> shark_spawner;
	float shark_spawn_time							  = CFG_VALUE<float>("shark_spawn_time", 1.0f);
	float shark_spawn_variation						  = CFG_VALUE<float>("shark_spawn_variation", 0.0f);
	int shark_limit									  = CFG_VALUE<int>("shark_limit", 10);
	float shark_spawn_increase_base_value			  = CFG_VALUE<float>("shark_spawn_increase_base_value", 0.05f);
	float shark_spawn_increase_bounty_divider		  = CFG_VALUE<float>("shark_spawn_increase_bounty_divider", 100.0f);
	int shark_limit_increase_bounty_divider			  = CFG_VALUE<int>("shark_limit_increase_bounty_divider", 200);
	int shark_max_limit								  = CFG_VALUE<int>("shark_max_limit", 100);
	float shark_min_spawn_time						  = CFG_VALUE<float>("shark_min_spawn_time", 0.1f);
	int shark_start_increase_difficulty_bounty_amount = CFG_VALUE<int>("shark_start_increase_at_bounty", 160);
	int shark_start_decrease_difficulty_bounty_amount = CFG_VALUE<int>("shark_start_decrease_at_bounty", 4000);
	int shark_stop_spawn_bounty_amount				  = CFG_VALUE<int>("shark_stop_spawn_at_bounty", 6000);

	std::unique_ptr<PSCore::Spawner<ChonkyShark, AppLayer>> chonky_shark_spawner;
	float chonky_shark_spawn_time					 = CFG_VALUE<float>("chonky_shark_spawn_time", 5.0f);
	float chonky_shark_spawn_variation				 = CFG_VALUE<float>("chonky_shark_spawn_variation", 2.0f);
	int chonky_shark_limit							 = CFG_VALUE<int>("chonky_shark_limit", 5);
	float chonky_shark_spawn_increase_base_value	 = CFG_VALUE<float>("chonky_shark_spawn_increase_base_value", 0.1f);
	float chonky_shark_spawn_increase_bounty_divider = CFG_VALUE<float>("chonky_shark_spawn_increase_bounty_divider", 100.0f);
	int chonky_shark_limit_increase_bounty_divider	 = CFG_VALUE<int>("chonky_shark_limit_increase_bounty_divider", 200);
	int chonky_shark_max_limit						 = CFG_VALUE<int>("chonky_shark_max_limit", 50);
	float chonky_shark_min_spawn_time				 = CFG_VALUE<float>("chonky_shark_min_spawn_time", 0.5f);
	int chonky_shark_takeover_threshold				 = CFG_VALUE<int>("chonky_shark_takeover_threshold", 7000);

	std::unique_ptr<PSCore::Spawner<tentacle, AppLayer>> tentacle_spawner;
	int tentacle_limit							 = CFG_VALUE<int>("tentacle_limit", 10);
	float tentacle_spawn_time					 = CFG_VALUE<float>("tentacle_spawn_time", 5.0f);
	float tentacle_spawn_variation				 = CFG_VALUE<float>("tentacle_spawn_variation", 3.0f);
	float tentacle_spawn_increase_base_value	 = CFG_VALUE<float>("tentacle_spawn_increase_base_value", 0.05f);
	float tentacle_spawn_increase_bounty_divider = CFG_VALUE<float>("tentacle_spawn_increase_bounty_divider", 100.0f);
	int tentacle_limit_increase_bounty_divider	 = CFG_VALUE<int>("tentacle_limit_increase_bounty_divider", 200);
	int tentacle_max_limit						 = CFG_VALUE<int>("tentacle_max_limit", 100);
	float tentacle_min_spawn_time				 = CFG_VALUE<float>("tentacle_min_spawn_time", 0.5f);
	int tentacle_start_spawn_bounty_amount		 = CFG_VALUE<int>("tentacle_start_spawn_at_bounty", 200);
	bool m_tentacle_spawn_active				 = false;

	std::unique_ptr<PSCore::Spawner<Projectile, AppLayer>> projectile_spawner;
	float player_max_velocity		 = CFG_VALUE<float>("player_max_velocity", 200.0f);
	float player_input_rotation_mult = CFG_VALUE<float>("player_input_rotation_mult", 200.0f);
	float player_input_velocity_mult = CFG_VALUE<float>("player_input_velocity_mult", 1500.0f);
	int player_max_health			 = CFG_VALUE<int>("player_max_health", 3);
	int player_health				 = CFG_VALUE<int>("player_health", 3);
	float player_iframe_duration	 = CFG_VALUE<float>("player_iframe_duration", 0.5f);
	bool player_invincibility		 = CFG_VALUE<bool>("player_invincibility", false);

	std::unique_ptr<PSCore::Spawner<Hunter, AppLayer>> hunter_spawner;
	int hunter_limit						   = CFG_VALUE<int>("hunter_limit", 2);
	float hunter_spawn_time					   = CFG_VALUE<float>("hunter_spawn_time", 10.0f);
	float hunter_spawn_variation			   = CFG_VALUE<float>("hunter_spawn_variation", 3.0f);
	float hunter_spawn_increase_base_value	   = CFG_VALUE<float>("hunter_spawn_increase_base_value", 0.05f);
	float hunter_spawn_increase_bounty_divider = CFG_VALUE<float>("hunter_spawn_increase_bounty_divider", 100.0f);
	int hunter_limit_increase_bounty_divider   = CFG_VALUE<int>("hunter_limit_increase_bounty_divider", 200);
	int hunter_max_limit					   = CFG_VALUE<int>("hunter_max_limit", 5);
	float hunter_min_spawn_time				   = CFG_VALUE<float>("hunter_min_spawn_time", 3.f);
	int hunter_start_spawn_bounty_amount	   = CFG_VALUE<int>("hunter_start_spawn_at_bounty", 400);
	bool hunter_spawn_active				 = false;
};
