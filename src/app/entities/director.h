#pragma once

#include <memory>

#include <entities/cannon.h>
#include <entities/player.h>
#include <entities/projectile.h>
#include <pscore/application.h>
#include <psinterfaces/entity.h>

#include <entities/shark.h>
#include <layers/applayer.h>
#include <pscore/spawner.h>
#include <entities/tentacle.h>
#include <entities/lootchest.h>

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

	// Functions to spawn and destroy projectiles
	// std::shared_ptr<Projectile> spawn_projectile(const Vector2& position);
	// void destroy_projectile(std::shared_ptr<Projectile> projectile);

	// Functions to spawn and destroy cannons
	std::shared_ptr<Cannon> spawn_cannon(const Vector2& position);
	void destroy_cannon(std::shared_ptr<Cannon> cannon);

	std::shared_ptr<LootChest> spawn_loot_chest(const Vector2& position);

	template<class E, PSCore::ILayerDerived L>
	std::unique_ptr<PSCore::Spawner<E, L>>& spawner();

	struct DropChances
	{
		float add_cannon		= CFG_VALUE<int>("upgrade_add_cannon_chance", 1);
		float projectile_speed = CFG_VALUE<int>("upgrade_projectile_speed_chance", 13);
		float fire_range		= CFG_VALUE<int>("upgrade_fire_range_chance", 13);
		float fire_rate		= CFG_VALUE<int>("upgrade_fire_rate_chance", 13);
		float health			= CFG_VALUE<int>("upgrade_health_chance", 13);
		float speed			= CFG_VALUE<int>("upgrade_speed_chance", 13);
		float rotation_speed	= CFG_VALUE<int>("upgrade_rotation_speed_chance", 13);
		float piercing_chance	 = CFG_VALUE<int>("upgrade_piercing_chance_chance", 13);
		float luck				= CFG_VALUE<int>("upgrade_luck_chance", 6);
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

	float player_current_fire_rate() const;
	float player_current_projectile_speed() const;
	float player_current_fire_range() const;
	float player_max_velocity() const;
	float player_input_rotation_mult() const;

	void set_player_piercing_chance(const int chance);
	float player_piercing_chance() const;
	float player_luck() const;

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
	float player_current_luck			  = 0.1; // CFG_VALUE<float>("player_current_luck", 0.1f);

	std::unique_ptr<PSCore::Spawner<Shark, AppLayer>> shark_spawner;
	float shark_spawn_time		= CFG_VALUE<float>("shark_spawn_time", 1.0f);
	float shark_spawn_variation = CFG_VALUE<float>("shark_spawn_variation", 0.0f);
	int shark_limit				= CFG_VALUE<int>("shark_limit", 10);
	float shark_spawn_increase_base_value		= CFG_VALUE<float>("shark_spawn_increase_base_value", 0.05f);
	float shark_spawn_increase_bounty_divider	= CFG_VALUE<float>("shark_spawn_increase_bounty_divider", 100.0f);
	int shark_limit_increase_bounty_divider		= CFG_VALUE<int>("shark_limit_increase_bounty_divider", 200);
	int shark_max_limit							= CFG_VALUE<int>("shark_max_limit", 100);
	float shark_min_spawn_time					= CFG_VALUE<float>("shark_min_spawn_time", 0.1f);
	int shark_start_increase_difficulty_bounty_amount = CFG_VALUE<int>("shark_start_increase_at_bounty", 160);
	int shark_start_decrease_difficulty_bounty_amount = CFG_VALUE<int>("shark_start_decrease_at_bounty", 4000);
	int shark_stop_spawn_bounty_amount				  = CFG_VALUE<int>("shark_stop_spawn_at_bounty", 6000);

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
};