#include "player.h"
#include <raylib.h>

#include <pscore/application.h>
#include <raymath.h>

#include <layers/applayer.h>

#ifndef CALCULATION_VELOCITY_MIN
#define CALCULATION_VELOCITY_MIN 1
#endif

Player::Player()
{

	// WARNING: THIS IS ONLY FOR TESTING
	if ( auto& vp = gApp()->viewport() ) {
		m_position = vp->viewport_base_size() / 2;
	}
	m_max_velocity = 200;
	m_rotation	   = 0;
	set_interpolation_values(6, 2, 4, 1500, 0.9);
	set_texture_values(LoadTexture("ressources/SpaceShipSpriteSheet.png"), 90, 4);
	set_animation_values(2, {1, 4}, 4);
	//
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
	// Check if the input Velocity is in range of the Max Velocity
	// and set the Velocity and Target Velocity to the input Velocity if the check is True
	// or if the check is False it's set to Max Velocity
	m_velocity		  = Vector2Length(velocity) <= m_max_velocity ? velocity : Vector2Scale(Vector2Normalize(velocity), m_max_velocity);
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
	m_rotation		  = rotation;
	m_target_rotation = rotation;
}

void Player::set_target_rotation(const float& target_rotation)
{
	m_target_rotation = target_rotation;
}

void Player::set_interpolation_values(
		const float& acceleration_fade, const float& deceleration_fade, const float& rotation_fade, const float& input_velocity_multiplier,
		const float& input_rotation_multiplier
)
{
	m_acceleration_fade			= acceleration_fade;
	m_deceleration_fade			= deceleration_fade;
	m_rotation_fade				= rotation_fade;
	m_input_velocity_multiplier = input_velocity_multiplier;
	m_input_rotation_multiplier = input_rotation_multiplier;
}

void Player::calculate_movement(const float& dt)
{
	// Linear Interpolation form Rotation to Target Rotation with a regression of Rotation and a static Alpha
	// which ends in an exponential approximation to calculate the rotation
	m_rotation = m_rotation + (m_target_rotation - m_rotation) * std::clamp(m_rotation_fade * dt, 0.0f, 1.0f);

	// Check if the Velocity should increase or decrease and uses right the Linear Interpolation form Velocity to Target Velocity with a regression of
	// Velocity and a static Alpha which ends in an exponential approximation to calculate the Value of the Velocity
	float velocity_value =
			(m_target_velocity - Vector2Length(m_velocity)) > 0
					? Vector2Length(m_velocity) + (m_target_velocity - Vector2Length(m_velocity)) * std::clamp(m_acceleration_fade * dt, 0.0f, 1.0f)
					: Vector2Length(m_velocity) + (m_target_velocity - Vector2Length(m_velocity)) * std::clamp(m_deceleration_fade * dt, 0.0f, 1.0f);

	// Calculate with the Velocity Value and the Rotation the actual 2 Dimensional Velocity
	m_velocity.x = velocity_value * cos(m_rotation * DEG2RAD);
	m_velocity.y = velocity_value * sin(m_rotation * DEG2RAD);

	// Update Position based on Velocity
	m_position.x += m_velocity.x * dt;
	m_position.y += m_velocity.y * dt;
}


void Player::update(const float dt)
{
	if ( !m_is_clone ) {
		// Input Functions to set Target Velocity and Target Rotation
		if ( IsKeyDown(KEY_W) ) {
			m_target_velocity += m_target_velocity < m_max_velocity ? m_input_velocity_multiplier * dt : 0;
		}
		if ( IsKeyDown(KEY_S) ) {
			m_target_velocity -= m_target_velocity > 0 ? m_input_velocity_multiplier * dt : 0;
		}
		if ( IsKeyDown(KEY_D) && Vector2Length(m_velocity) > CALCULATION_VELOCITY_MIN ) {
			m_target_rotation += m_input_rotation_multiplier * Vector2Length(m_velocity) * dt;
		}
		if ( IsKeyDown(KEY_A) && Vector2Length(m_velocity) > CALCULATION_VELOCITY_MIN ) {
			m_target_rotation -= m_input_rotation_multiplier * Vector2Length(m_velocity) * dt;
		}
	}

	calculate_movement(dt);

	// Animation Calculation

	calculate_animation(dt);
}

void Player::set_texture_values(const Texture2D& texture, const float& rotation_offset, const float& base_scale)
{
	m_texture		  = texture;
	m_rotation_offset = rotation_offset;
	m_base_scale	  = base_scale;
}

void Player::set_animation_values(const int& animation_max_count, const std::valarray<int>& sprite_sheet, const float& animation_speed)
{
	m_sprite_sheet.resize(animation_max_count);
	m_sprite_sheet	  = sprite_sheet;
	m_animation_speed = animation_speed;
}

void Player::calculate_animation(const float& dt)
{
	if ( Vector2Length(m_velocity) >= CALCULATION_VELOCITY_MIN && m_animation_count == 0 ) {
		m_animation_count = 1;
		m_animation_frame = 0;
	}
	if ( Vector2Length(m_velocity) < CALCULATION_VELOCITY_MIN && m_animation_count == 1 ) {
		m_animation_count = 0;
		m_animation_frame = 0;
	}

	m_frame_counter++;
	if ( m_frame_counter >= 1 / (dt * m_animation_speed) ) {
		m_frame_counter = 0;
		m_animation_frame++;
		if ( m_animation_frame >= round(Lerp(0, (float) m_sprite_sheet[m_animation_count], Vector2Length(m_velocity) / m_max_velocity)) ) {
			m_animation_frame = 0;
		}
	}
}

void Player::render()
{
	m_source = {
			m_animation_frame * (float) m_texture.width / m_sprite_sheet.max(), m_animation_count * (float) m_texture.height / m_sprite_sheet.size(),
			(float) m_texture.width / m_sprite_sheet.max(), (float) m_texture.height / m_sprite_sheet.size()
	};
	if ( auto& vp = gApp()->viewport() ) {
		vp->draw_in_viewport(m_texture, m_source, m_position, m_rotation + m_rotation_offset, WHITE);
	}
}

// Border Collision Variables and Methods
void Player::set_border_collision_active_horizontal(bool active)
{
	m_border_collision_active_horizontal = active;
}

bool Player::border_collision_active_horizontal() const
{
	return m_border_collision_active_horizontal;
}

void Player::set_border_collision_active_vertical(bool active)
{
	m_border_collision_active_vertical = active;
}

bool Player::border_collision_active_vertical() const
{
	return m_border_collision_active_vertical;
}

void Player::set_is_clone(bool active)
{
	m_is_clone = active;
}

bool Player::is_clone() const
{
	return m_is_clone;
}

float Player::dest_width() const
{
	if ( auto& vp = gApp()->viewport() ) {
		return m_source.width * vp->viewport_scale();
	}
	return m_source.width;
}

float Player::dest_height() const
{
	if ( auto& vp = gApp()->viewport() ) {
		return m_source.height * vp->viewport_scale();
	}
	return m_source.height;
}
