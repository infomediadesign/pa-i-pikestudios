#pragma once

#include <memory>

#include <entities/cannon.h>
#include <entities/player.h>
#include <entities/projectile.h>
#include <psinterfaces/entity.h>
#include <pscore/application.h>

#include <entities/shark.h>
#include <layers/applayer.h>
#include <pscore/spawner.h>

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

	template<class E, PSCore::ILayerDerived L>
	std::unique_ptr<PSCore::Spawner<E, L>>& spawner();

	// Upgrade functions
	void upgrade_player_fire_rate(float amount);
	void upgrade_player_projectile_speed(float amount);
	void upgrade_player_fire_range(float amount);
	void upgrade_player_add_cannon(int amount);

	// Player Health
	void set_player_health(const int health);
	int player_health() const;

	void set_player_max_health(const int max_health);
	int player_max_health() const;

	//Bounty
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

	struct BountyAmount
	{
		int shark_bounty	= 40;
		int ship_bounty		= 150;
		int tentacle_bounty = 225;
	};
	Bounty m_b_bounty;
	BountyAmount m_b_bounty_amounts;

private:
	
	std::unique_ptr<FortunaDirectorPriv> _p;
};
