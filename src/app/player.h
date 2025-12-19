#pragma once

#include <psinterfaces/entity.h>
#include <psinterfaces/renderable.h>

#include <raylib.h>

class Player : public PSInterfaces::IRenderable
{
public:
	Player();

	float target_velocity();

	float target_rotation();

	void set_target_velocity(const float& target_velocity);

	void set_target_rotation(const float& target_rotation);

	void set_interpolation_values(const float& acceleration_fade, const float& deceleration_fade, const float& rotation_fade);

	void update(const float dt) override;

	void set_texture_value();

	void render() override;

private:
	float m_target_velocity;
	float m_target_rotation;
	float m_acceleration_fade;
	float m_deceleration_fade;
	float m_rotation_fade;

	Texture2D m_texture;
	float m_textures_in_image;
	float m_rotation_offset;
	Rectangle m_source;
	Rectangle m_dest;
	Vector2 m_origin;
	
	float m_rot;
	Vector2 m_vel;
	Vector2 m_pos;
};
