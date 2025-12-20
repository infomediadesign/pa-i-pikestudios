#include "player.h"
#include <iostream>
#include <raylib.h>

#include "pscore/application.h"
#include "raymath.h"

Player::Player() {
	
	// WARNING: THIS IS ONLY FOR TESTING
	m_position = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	m_max_velocity = 600;
	m_rotation = 0;
	set_interpolation_values(6, 2, 4, 1500, 0.3);
	set_texture_value(LoadTexture("ressources/SpaceShip.png"), 5, 90, 0.5);
	//
	
	PSCore::Application::get()->register_entity(this);
}

Vector2 Player::position()
{
	return m_position;
}

Vector2 Player::velocity()
{
	return m_velocity;
}

float Player::target_velocity()
{
	return m_target_velocity;
}

float Player::max_velocity()
{
	return m_max_velocity;
}

float Player::rotation()
{
	return m_rotation;
}

float Player::target_rotation()
{
	return m_target_rotation;
}

void Player::set_position(const Vector2& position)
{
	m_position = position;
}

void Player::set_velocity(const Vector2& velocity)
{
	m_velocity = Vector2Length(velocity) <= m_max_velocity ? velocity : Vector2Scale(Vector2Normalize(velocity), m_max_velocity);
	m_target_velocity = Vector2Length(velocity) <= m_max_velocity ? Vector2Length(velocity) : m_max_velocity;
}

void Player::set_target_velocity(const float& target_velocity)
{
	m_target_velocity = target_velocity;
}

void Player::set_max_velocity(const float& max_velocity)
{
	m_max_velocity = max_velocity;
}

void Player::set_rotation(const float& rotation)
{
	m_rotation = rotation;
	m_target_rotation = rotation;
}

void Player::set_target_rotation(const float& target_rotation)
{
	m_target_rotation = target_rotation;
}

void Player::set_interpolation_values(const float& acceleration_fade, const float& deceleration_fade, const float& rotation_fade, const float& input_velocity_multiplier, const float& input_rotation_multiplier)
{
	m_acceleration_fade = acceleration_fade;
	m_deceleration_fade = deceleration_fade;
	m_rotation_fade = rotation_fade;
	m_input_velocity_multiplier = input_velocity_multiplier;
	m_input_rotation_multiplier = input_rotation_multiplier;
}

void Player::movement_calculation(const float& dt)
{
	m_rotation = m_rotation + (m_target_rotation - m_rotation) * fminf((m_rotation_fade * dt), 1);

	float velocity_value = (m_target_velocity - Vector2Length(m_velocity)) > 0
	? Vector2Length(m_velocity) + (m_target_velocity - Vector2Length(m_velocity)) * fminf((m_acceleration_fade * dt), 1)
	: Vector2Length(m_velocity) + (m_target_velocity - Vector2Length(m_velocity)) * fminf((m_deceleration_fade * dt), 1);

	m_velocity.x = velocity_value * cos(m_rotation * DEG2RAD);
	m_velocity.y = velocity_value * sin(m_rotation * DEG2RAD);

	m_position.x += m_velocity.x * dt;
	m_position.y += m_velocity.y * dt;
}


void Player::update(const float dt)
{
	if (IsKeyDown(KEY_W)) m_target_velocity += m_target_velocity < m_max_velocity ? m_input_velocity_multiplier  * dt : 0;
	if (IsKeyDown(KEY_S)) m_target_velocity -= m_target_velocity > 0 ? m_input_velocity_multiplier * dt : 0;
	if (IsKeyDown(KEY_D) && Vector2Length(m_velocity) > 1) m_target_rotation += m_input_rotation_multiplier * Vector2Length(m_velocity) * dt;
	if (IsKeyDown(KEY_A) && Vector2Length(m_velocity) > 1) m_target_rotation -= m_input_rotation_multiplier * Vector2Length(m_velocity) * dt;

	movement_calculation(dt);
}

void Player::set_texture_value(const Texture2D& texture, const float& textures_in_image, const float& rotation_offset, const float& base_scale)
{
	m_texture = texture;
	m_textures_in_image = textures_in_image;
	m_rotation_offset = rotation_offset;
	m_base_scale = base_scale;
}


void Player::render()
{
	m_source = {0,0, (float)m_texture.width / m_textures_in_image, (float)m_texture.height};
	m_dest = {m_position.x, m_position.y, m_source.width * m_base_scale, m_source.height * m_base_scale};
	m_origin = {m_dest.width / 2, m_dest.height / 2};
	DrawTexturePro(m_texture, m_source, m_dest, m_origin, m_rotation + m_rotation_offset, WHITE);
}
