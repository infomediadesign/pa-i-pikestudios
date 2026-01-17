#include "player.h"
#include <raylib.h>
#include <entities/director.h>

#include <pscore/application.h>
#include <raymath.h>

#include <layers/applayer.h>

#ifndef CALCULATION_VELOCITY_MIN
#define CALCULATION_VELOCITY_MIN 1
#endif

Player::Player()
{

	// WARNING: THIS IS ONLY FOR TESTING
	m_position	   = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	m_max_velocity = 600;
	m_rotation	   = 0;
	set_interpolation_values(6, 2, 4, 1500, 0.3);
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
	m_rotation = m_rotation + (m_target_rotation - m_rotation) * fmaxf(0, fminf((m_rotation_fade * dt), 1));

	// Check if the Velocity should increase or decrease and uses right the Linear Interpolation form Velocity to Target Velocity with a regression of
	// Velocity and a static Alpha which ends in an exponential approximation to calculate the Value of the Velocity
	float velocity_value =
			(m_target_velocity - Vector2Length(m_velocity)) > 0
					? Vector2Length(m_velocity) + (m_target_velocity - Vector2Length(m_velocity)) * fmaxf(0, fminf((m_acceleration_fade * dt), 1))
					: Vector2Length(m_velocity) + (m_target_velocity - Vector2Length(m_velocity)) * fmaxf(0, fminf((m_deceleration_fade * dt), 1));

	// Calculate with the Velocity Value and the Rotation the actual 2 Dimensional Velocity
	m_velocity.x = velocity_value * cos(m_rotation * DEG2RAD);
	m_velocity.y = velocity_value * sin(m_rotation * DEG2RAD);

	// Update Position based on Velocity
	m_position.x += m_velocity.x * dt;
	m_position.y += m_velocity.y * dt;
}


void Player::update(const float dt)
{
	if ( !m_is_clone ) 
	{
		if ( IsKeyPressed(KEY_G) ) 
		{
			initialize_cannons(2);
		}
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
		if ( m_animation_frame >= round(Lerp(0, (float)m_sprite_sheet[m_animation_count], Vector2Length(m_velocity) / m_max_velocity)) ) {
			m_animation_frame = 0;
		}
	}
}

void Player::initialize_cannons(int amount)
{
	for (int i = 0; i < amount; i++ ) 
	{
		add_cannons();
	}
}

void Player::add_cannons()
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( !director ) 
	{
		printf("Error: Could not add cannons, director is null!\n");
		return;
	}
	if (m_cannon_container.size() <= 1) 
	{
		
		auto new_cannon = director->spawn_cannon(m_position);
		m_cannon_container.push_back(new_cannon);
		new_cannon->set_parent(m_shared_ptr_this);
		new_cannon->set_positioning(Cannon::CannonPositioning::Left);

		auto new_cannon_2 = director->spawn_cannon(m_position);
		m_cannon_container.push_back(new_cannon_2);
		new_cannon_2->set_parent(m_shared_ptr_this);
		new_cannon_2->set_positioning(Cannon::CannonPositioning::Right);
	} 
	else
	{
		float x_offset	= -20.0f * (m_cannon_container.size() / 2);
		auto new_cannon = director->spawn_cannon(m_position);
		m_cannon_container.push_back(new_cannon);
		new_cannon->set_parent(m_shared_ptr_this);
		new_cannon->set_positioning(Cannon::CannonPositioning::Left);
		new_cannon->set_parent_position_x_offset(x_offset);

		auto new_cannon_2 = director->spawn_cannon(m_position);
		m_cannon_container.push_back(new_cannon_2);
		new_cannon_2->set_parent(m_shared_ptr_this);
		new_cannon_2->set_positioning(Cannon::CannonPositioning::Right);
		new_cannon_2->set_parent_position_x_offset(x_offset);
	}

}

void Player::render()
{
	m_source = {
			m_animation_frame * (float) m_texture.width / m_sprite_sheet.max(), m_animation_count * (float) m_texture.height / m_sprite_sheet.size(),
			(float) m_texture.width / m_sprite_sheet.max(), (float) m_texture.height / m_sprite_sheet.size()
	};
	m_dest	 = {m_position.x, m_position.y, m_source.width * m_base_scale, m_source.height * m_base_scale};
	m_origin = {m_dest.width / 2, m_dest.height / 2};
	DrawTexturePro(m_texture, m_source, m_dest, m_origin, m_rotation + m_rotation_offset, WHITE);
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

bool Player::set_is_clone(bool active)
{
	m_is_clone = active;
	return m_is_clone;
}

bool Player::is_clone() const
{
	return m_is_clone;
}

float Player::dest_width() const
{
	return m_dest.width;
}

float Player::dest_height() const
{
	return m_dest.height;
}

std::vector<std::shared_ptr<Cannon>>& Player::cannon_container()
{
	return m_cannon_container;
}

void Player::set_cannon_container(const std::vector<std::shared_ptr<Cannon>>& container)
{
	m_cannon_container = container;
}

std::shared_ptr<Player> Player::shared_ptr_this()
{
	return m_shared_ptr_this;
}

void Player::set_shared_ptr_this(std::shared_ptr<Player> ptr)
{
	m_shared_ptr_this = ptr;
}
