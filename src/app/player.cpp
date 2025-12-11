#include "player.h"

#include "raymath.h"

float Player::target_velocity()
{
	return m_target_velocity;
}

float Player::target_rotation()
{
	return m_target_rotation;
}

void Player::set_target_velocity(const float& target_velocity)
{
	m_target_velocity = target_velocity;
}

void Player::set_target_rotation(const float& target_rotation)
{
	m_target_rotation = target_rotation;
}

void Player::set_interpolation_values(const float& acceleration_fade, const float& deceleration_fade, const float& rotation_fade)
{
	m_acceleration_fade = acceleration_fade;
	m_deceleration_fade = deceleration_fade;
	m_rotation_fade = rotation_fade;
}

void Player::move(const float dt)
{
	m_rot = m_rot + (m_target_rotation - m_rot) * (m_rotation_fade * dt);

	float velocity_value = (m_target_velocity - Vector2Length(m_vel)) > 0
	? Vector2Length(m_vel) + (m_target_velocity - Vector2Length(m_vel)) * (m_acceleration_fade * dt)
	: Vector2Length(m_vel) + (m_target_velocity - Vector2Length(m_vel)) * (m_deceleration_fade * dt);

	m_vel.x = velocity_value * cos(m_rot * DEG2RAD);
	m_vel.y = velocity_value * sin(m_rot * DEG2RAD);

	m_pos.x += m_vel.x * dt;
	m_pos.y += m_vel.y * dt;
}
