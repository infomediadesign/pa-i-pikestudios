#pragma once
#include <memory>
#include <optional>
#include <pscore/collision.h>
#include <pscore/settings.h>
#include <pscore/sprite.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include "pscore/sprite.h"
#include <vector>

class Player;
class FortunaDirector;
class Cannon;

class Projectile : public PSInterfaces::IRenderable
{
public:
	Projectile();
	~Projectile()
	{
	}
	void update(const float dt) override;
	void render() override;
	void on_hit() override;

	std::optional<std::vector<Vector2>> bounds() const override;

	void init(const Vector2& position, std::shared_ptr<Projectile> self);

	Texture2D texture();
	void set_texture(const Texture2D& texture);

	std::optional<Vector2> position() const override;
	void set_position(const Vector2& position);

	float rotation();
	void set_rotation(const float& rotation);

	Vector2 velocity();
	void set_velocity(const Vector2& velocity);

	Vector2 target_position();
	void set_target_position(const Vector2& target_position);

	Vector2 direction();
	void set_direction(const Vector2& direction);

	Vector2 movement();
	void set_movement(const Vector2& movement);

	float speed();
	void set_speed(const float speed);

	float travel_distance();
	void set_travel_distance(const float travel_distance);

	float max_range();
	void set_max_range(const float max_range);

	std::shared_ptr<Projectile> shared_ptr();
	void set_shared_ptr(std::shared_ptr<Projectile>& ptr);

	std::shared_ptr<Player> owner();
	void set_owner(std::shared_ptr<Player>& owner);

	Vector2 owner_velocity();
	void set_owner_velocity(const Vector2& velocity);

	std::shared_ptr<Cannon> fiering_cannon();
	void set_fiering_cannon(const std::shared_ptr<Cannon>& cannon);

	void calculate_movment(const float dt);

	bool can_pierce() const;
	int piercing_chance() const;
	void set_piercing_chance(const int chance);

	void draw_debug() override;
	void launch();

	void play_hit_anim(float dt);
	void play_no_hit_anim(float dt);

	void apply_drag(const float dt);
	void update_pierce_hit_anims(float dt);

private:

	int m_p_z_index;
	Vector2 m_p_position;
	Vector2 m_p_velocity;
	Vector2 m_p_target_position;
	Vector2 m_p_direction;
	Vector2 m_p_movement;
	Vector2 m_p_local_offset;
	float m_p_initial_distance;
	float m_p_rotation;
	float m_p_speed;
	float m_p_travel_distance;
	Vector2 m_p_owner_velocity;
	int m_p_piercing_chance = 50;

	Texture2D m_p_texture;
	std::shared_ptr<PSCore::sprites::Sprite> m_p_sprite;
	Rectangle m_p_source;
	Rectangle m_p_dest;
	bool m_p_is_first_tick = true;
	std::shared_ptr<Projectile> m_p_shared_ptr;
	std::shared_ptr<Player> m_p_owner;
	std::shared_ptr<Cannon> m_p_fiering_cannon;
	
	PSCore::sprites::SpriteSheetAnimation m_p_animation_controller;
	Texture2D m_p_hit_anim_texture;
	std::shared_ptr<PSCore::sprites::Sprite> m_p_hit_anim_sprite;

	PSCore::sprites::SpriteSheetAnimation m_p_no_hit_anim_controller;
	Texture2D m_p_no_hit_anim_texture;
	std::shared_ptr<PSCore::sprites::Sprite> m_p_no_hit_anim_sprite;

	bool m_p_hit_aninm_playing = false;
	bool m_p_no_hit_anim_playing = false;
	Vector2 m_p_hit_anim_pos;

	struct PierceHitAnim {
		Vector2 position;
		float rotation;
		PSCore::sprites::SpriteSheetAnimation anim_controller;
		bool finished = false;
	};
	std::vector<PierceHitAnim> m_p_pierce_hit_anims;
	
	std::unique_ptr<PSCore::collision::EntityCollider> m_collider;

	Vector2 m_p_local_direction;
	float m_p_max_range		  = 500.0f;
	float m_p_drag_per_second = 0.92f;
};
