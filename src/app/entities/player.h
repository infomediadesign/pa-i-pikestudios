#pragma once

#include <optional>
#include <psinterfaces/renderable.h>

#include <entities/cannon.h>
#include <raylib.h>

#include <misc/smear.h>

#include <misc/loottable.h>
#include <pscore/sprite.h>

class Sails;
class Player : public PSInterfaces::IRenderable
{
	friend class Sails;

public:
	Player();

	void update(float dt) override;

	void render() override;

	void draw_debug() override;

	void on_hit() override;

	void on_death();


	std::optional<Vector2> position() const override;

	std::optional<std::vector<Vector2>> bounds() const override;

	void damage();

	Vector2 velocity();

	float target_velocity();

	float max_velocity();

	float rotation();

	float target_rotation();

	void set_position(const Vector2& position);

	void set_velocity(const Vector2& velocity);

	void set_target_velocity(float target_velocity);

	void set_max_velocity(float max_velocity);

	void set_input_velocity_multiplier(float val);
	void set_input_rotation_multiplier(float val);

	void set_rotation(float rotation);

	void set_target_rotation(float target_rotation);

	void set_interpolation_values(
			float acceleration_fade, float deceleration_fade, float rotation_fade, float input_velocity_multiplier, float input_rotation_multiplier,
			float velocity_rotation_downscale
	);

	void calculate_movement(float dt);

	float calculate_rotation_velocity(float frequency, float dt);

	void set_texture_values(const Texture2D& texture, float rotation_offset);

	// Borderinteration Variables and Methods
	void set_border_collision_active_horizontal(bool active);

	bool border_collision_active_horizontal() const;

	void set_border_collision_active_vertical(bool active);

	bool border_collision_active_vertical() const;

	void set_is_clone(bool active);

	bool is_clone() const;

	float dest_width() const;

	float dest_height() const;

	// Cannons & Projectiles Variables and Methods
	std::vector<std::shared_ptr<Cannon>>& cannon_container();
	void set_cannon_container(const std::vector<std::shared_ptr<Cannon>>& container);

	std::shared_ptr<Player> shared_ptr_this();
	void set_shared_ptr_this(std::shared_ptr<Player> ptr);

	void initialize_cannon();

	void fire_cannons(float dt);

	enum class FireMode { InSequence, SameTime };

	FireMode fire_mode() const;
	void set_fire_mode(FireMode mode);

	struct FiringCannonIndex
	{
		int right;
		int left;
	};

	void add_cannons(int amount);

	// invincibleity
	void reset_iframe(float dt);
	void set_is_invincible(bool invincible);


private:
	// Base Movement Variables
	Vector2 m_position	 = {0};
	Vector2 m_velocity	 = {0};
	float m_max_velocity = 0;
	float m_rotation	 = 0;

	// Interpolation Values for the Movement Calculation
	float m_target_velocity				= 0;
	float m_target_rotation				= 0;
	float m_acceleration_fade			= 0;
	float m_deceleration_fade			= 0;
	float m_rotation_fade				= 0;
	float m_input_velocity_multiplier	= 0;
	float m_input_rotation_multiplier	= 0;
	float m_rotation_velocity			= 0;
	float m_velocity_rotation_downscale = 0;

	// Variables for Texture Rendering
	Texture2D m_texture		= {0};
	float m_rotation_offset = 0;
	std::shared_ptr<PSCore::sprites::Sprite> m_sprite;

	// Variables for Animation
	PSCore::sprites::SpriteSheetAnimation m_animation_controller;
	int m_sprite_sheet_animation_index = 2;
	int m_sprite_sheet_frame_index;

	// Variables for Borderinteration
	bool m_border_collision_active_horizontal = false;
	bool m_border_collision_active_vertical	  = false;
	bool m_is_clone							  = false;

	// Variabels and Methods for Cannons & Projectiles
	std::vector<std::shared_ptr<Cannon>> m_cannon_container;
	std::shared_ptr<Player> m_shared_ptr_this;
	FireMode m_fire_mode = FireMode::InSequence;
	bool m_fire_sequence_ongoing = false;
	bool m_fire_sequence_ongoing_left = false;
	bool m_fire_sequence_ongoing_right = false;
	float m_time_since_last_shot_left = 0;
	float m_time_since_last_shot_right = 0;
	FiringCannonIndex m_firing_cannon_index{1, 0};


	// Smear Variables
	Smear m_smear;
	Color m_smear_color = {9, 75, 101, 127};

	// invincibleity Variables
	bool m_can_be_hit		= true;
	bool m_is_invincible	= false;
	float m_iframe_timer	= 0;
	float m_iframe_duration = 5;

	// Sails
	std::shared_ptr<Sails> m_sails;

	// Loot Table
	LootTable m_loot_table;
};

class Sails : public PSInterfaces::IRenderable{
public:
	Sails(Player* player);
	~Sails();

	void update(float dt) override;
	void render() override;
	void draw_debug() override;

private:
	const Player* m_player;

};