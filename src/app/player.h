#pragma once
#include <psinterfaces/movable.h>
#include <psinterfaces/renderable.h>


class Player : public PSInterfaces::IMovable, PSInterfaces::IRenderable
{
public:
	Player();

	~Player();

	float target_velocity();

	float target_rotation();

	void set_target_velocity(const float& target_velocity);

	void set_target_rotation(const float& target_rotation);

	void set_interpolation_values(const float& acceleration_fade, const float& deceleration_fade, const float& rotation_fade);

	void move(const float dt) override;

private:
	float m_target_velocity;
	float m_target_rotation;
	float m_acceleration_fade;
	float m_deceleration_fade;
	float m_rotation_fade;
};
