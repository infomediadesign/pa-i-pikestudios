#pragma once

#include <memory>

#include <entities/player.h>
#include <psinterfaces/entity.h>
#include <entities/projectile.h>
#include <entities/cannon.h>

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
	std::shared_ptr<Projectile> spawn_projectile(const Vector2& position);
	void destroy_projectile(std::shared_ptr<Projectile> projectile);


	// Functions to spawn and destroy cannons
	std::shared_ptr<Cannon> spawn_cannon(const Vector2& position);
	void destroy_cannon(std::shared_ptr<Cannon> cannon);

	// Upgrade functions
	void upgrade_player_fire_rate(float amount);
	void upgrade_player_projectile_speed(float amount);
	void upgrade_player_fire_range(float amount);
	void upgrade_player_add_cannon(int amount);

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

private:
	std::unique_ptr<FortunaDirectorPriv> _p;
	Bounty m_b_bounty;
};
