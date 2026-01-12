#pragma once

#include <psinterfaces/entity.h>
#include <psinterfaces/renderable.h>

#include <raylib.h>
#include <valarray>

class Player : public PSInterfaces::IRenderable
{
public:
	Player();

	Vector2 position();

	Vector2 velocity();

	float target_velocity();

	float max_velocity();

	float rotation();

	float target_rotation();

	void set_position(const Vector2& position);

	void set_velocity(const Vector2& velocity);

	void set_target_velocity(const float& target_velocity);

	void set_max_velocity(const float& max_velocity);

	void set_rotation(const float& rotation);

	void set_target_rotation(const float& target_rotation);

	void set_interpolation_values(
			const float& acceleration_fade, const float& deceleration_fade, const float& rotation_fade, const float& input_velocity_multiplier,
			const float& input_rotation_multiplier
	);

	void calculate_movement(const float& dt);

	void update(const float dt) override;

	void set_texture_values(const Texture2D& texture, const float& rotation_offset, const float& base_scale);

	void set_animation_values(const int& animation_max_count, const std::valarray<int>& sprite_sheet, const float& animation_speed);

	void calculate_animation(const float& dt);

	void render() override;

	// Borderinteration Variables and Methods
	void set_border_collision_active_horizontal(bool active);
	bool get_border_collision_active_horizontal() const;
	void set_border_collision_active_vertical(bool active);
	bool get_border_collision_active_vertical() const;
	bool set_is_clone(bool active);
	bool get_is_clone() const;

	float get_dest_width() const
	{
		return m_dest.width;
	}
	float get_dest_height() const
	{
		return m_dest.height;
	}

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
	float m_base_scale		= 1;
	Rectangle m_source		= {0};
	Rectangle m_dest		= {0};
	Vector2 m_origin		= {0};

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
};
