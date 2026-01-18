#include "projectile.h"
#include <raylib.h>
#include <entities/director.h>
#include <pscore/application.h>

#include <raymath.h>

Projectile::Projectile()
{

	IRenderable::propose_z_index(1);
	m_p_position		= {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	m_p_rotation		= 0.0f;
	m_p_texture			= LoadTexture("ressources/test_projectile.png");
	m_p_target_position = {360.0f, 360.f};
	m_p_speed			= 200.0f;
}

void Projectile::update(const float dt)
{
	
	m_p_target_position += m_p_owner_velocity * dt;
	calculate_movement(dt, m_p_target_position);

}

void Projectile::render()
{
	if ( m_p_is_active ) 
	{
		m_p_source	   = {0, 0, (float) m_p_texture.width, (float) m_p_texture.height};
		m_p_dest	   = {m_p_position.x, m_p_position.y, (float) m_p_texture.width, (float) m_p_texture.height};
		Vector2 origin = {m_p_dest.width / 2, m_p_dest.height / 2};
		DrawTexturePro(m_p_texture, m_p_source, m_p_dest, origin, m_p_rotation, WHITE);
	}

}

bool Projectile::is_active()
{
	return m_p_is_active;
}

void Projectile::calculate_movement(const float dt, Vector2& target_position)
{
	m_p_direction		= Vector2Subtract(target_position, m_p_position);
	m_p_travel_distance = Vector2Length(m_p_direction);

	if ( m_p_travel_distance <= 1.0f ) {
		auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
		if ( !director ) {
			return;
		}
		m_p_is_active = false;
		return;
	}

	Vector2 normalized_direction = Vector2Normalize(m_p_direction);
	m_p_velocity.x				 = normalized_direction.x * m_p_speed;
	m_p_velocity.y				 = normalized_direction.y * m_p_speed;

	m_p_position.x += (m_p_velocity.x + m_p_owner_velocity.x) * dt;
	m_p_position.y += (m_p_velocity.y + m_p_owner_velocity.y) * dt;
}


Texture2D Projectile::texture()
{
	return m_p_texture;
}

void Projectile::set_texture(const Texture2D& texture)
{
	m_p_texture = texture;
}

Vector2 Projectile::position()
{
	return m_p_position;
}

void Projectile::set_position(const Vector2& position)
{
	m_p_position = position;
}

float Projectile::rotation()
{
	return m_p_rotation;
}

void Projectile::set_rotation(const float& rotation)
{
	m_p_rotation = rotation;
}

Vector2 Projectile::velocity()
{
	return m_p_velocity;
}

void Projectile::set_velocity(const Vector2& velocity)
{
	m_p_velocity = velocity;
}

Vector2 Projectile::target_position()
{
	return m_p_target_position;
}

void Projectile::set_target_position(const Vector2& target_position)
{
	m_p_target_position = target_position;
}

Vector2 Projectile::direction()
{
	return m_p_direction;
}

void Projectile::set_direction(const Vector2& direction)
{
	m_p_direction = direction;
}

Vector2 Projectile::movement()
{
	return m_p_movement;
}

void Projectile::set_movement(const Vector2& movement)
{
	m_p_movement = movement;
}

float Projectile::speed()
{
	return m_p_speed;
}

void Projectile::set_speed(const float speed)
{
	m_p_speed = speed;
}

float fire_rate();

float Projectile::travel_distance()
{
	return m_p_travel_distance;
}

void Projectile::set_travel_distance(const float travel_distance)
{
	m_p_travel_distance = travel_distance;
}

void Projectile::set_shared_ptr(std::shared_ptr<Projectile>& ptr)
{
	m_p_shared_ptr = ptr;
}

std ::shared_ptr<Projectile> Projectile::shared_ptr()
{
	return m_p_shared_ptr;
}

std::shared_ptr<Player> Projectile::owner()
{
	return m_p_owner;
}

void Projectile::set_owner(std::shared_ptr<Player>& owner)
{
	m_p_owner = owner;
}

Vector2 Projectile::owner_velocity()
{
	return m_p_owner_velocity;
}

void Projectile::set_owner_velocity(const Vector2& velocity)
{
	m_p_owner_velocity = velocity;
}

void Projectile::set_is_active(const bool active)
{
	m_p_is_active = active;
}
