#pragma once

#include <psinterfaces/entity.h>
#include <psinterfaces/renderable.h>

#include <raylib.h>
#include <valarray>

#include <misc/smear.h>

class Player : public PSInterfaces::IRenderable
{
public:
	Player();

	void update(float dt) override;

	void render() override;

	Vector2 position();

	Vector2 velocity();

	float target_velocity();

	float max_velocity();

	float rotation();

	float target_rotation();

	void set_position(const Vector2& position);

	void set_velocity(const Vector2& velocity);

	void set_target_velocity(float target_velocity);

	void set_max_velocity(float max_velocity);

	void set_rotation(float rotation);

	void set_target_rotation(float target_rotation);

	void set_interpolation_values(
			float acceleration_fade, float deceleration_fade, float rotation_fade, float input_velocity_multiplier, float input_rotation_multiplier
	);

	void calculate_movement(float dt);

	void set_texture_values(const Texture2D& texture, float rotation_offset);

	void set_animation_values(int animation_max_count, const std::valarray<int>& sprite_sheet, float animation_speed);

	void calculate_animation(float dt);

	// Borderinteration Variables and Methods
	void set_border_collision_active_horizontal(bool active);

	bool border_collision_active_horizontal() const;

	void set_border_collision_active_vertical(bool active);

	bool border_collision_active_vertical() const;

	void set_is_clone(bool active);

	bool is_clone() const;

	float dest_width() const;

	float dest_height() const;


private:
	// Base Movement Variables
	Vector2 m_position	 = {0};
	Vector2 m_velocity	 = {0};
	float m_max_velocity = 0;
	float m_rotation	 = 0;

	// Interpolation Values for the Movement Calculation
	float m_target_velocity			  = 0;
	float m_target_rotation			  = 0;
	float m_acceleration_fade		  = 0;
	float m_deceleration_fade		  = 0;
	float m_rotation_fade			  = 0;
	float m_input_velocity_multiplier = 0;
	float m_input_rotation_multiplier = 0;

	// Variables for Texture Rendering
	Texture2D m_texture		= {0};
	float m_rotation_offset = 0;
	Rectangle m_source		= {0};

	// Variables for Animation
	std::valarray<int> m_sprite_sheet = {1};
	float m_animation_speed			  = 1;

	float m_frame_counter	= 0;
	float m_animation_count = 0;
	float m_animation_frame = 0;

	// Variables for Borderinteration
	bool m_border_collision_active_horizontal = false;
	bool m_border_collision_active_vertical	  = false;
	bool m_is_clone							  = false;

	// Smear Variables
	float m_smear_rotation								= 0;
	std::vector<smear::SmearPoints> m_smear_points		= {{0}, {0}};
	std::vector<float> m_smear_wave						= {0};
	int m_smear_wave_index								= 0;
	float m_smear_wave_time								= 0;
	float m_smear_wave_per_second						= 0.25;
	std::vector<smear::SmearPoints> m_smear_wave_points = {{0}, {0}};
};
