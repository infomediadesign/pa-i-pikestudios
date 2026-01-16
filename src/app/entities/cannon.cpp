#include "cannon.h"
#include <raylib.h>
#include <entities/director.h>
#include <coordinatesystem.h>

Cannon::Cannon()
{
	IRenderable::propose_z_index(2);
	m_c_position = {100.0f, 100.0f};
	m_c_rotation = 0.0f;
	m_c_texture  = LoadTexture("ressources/test_cannon.png");
	m_c_range	 = 500.0f;
	m_c_time_since_last_shot = 0.0f;
	m_c_fire_rate_in_s		 = 0.5f;
	m_c_projectile_speed	 = 1000.0f;
}

void Cannon::update(const float dt)
{
	m_c_time_since_last_shot += dt;
	set_position_to_parent();
	set_rotation_to_parent();

	if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) ) 
	{
		if ( m_c_positioning == CannonPositioning::Left ) 
		{
			fire();
		}
	}

	if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ) 
	{
		if ( m_c_positioning == CannonPositioning::Right ) 
		{
			fire();
		}
	}
}

void Cannon::render()
{
	m_c_source = {0, 0, (float)m_c_texture.width, (float)m_c_texture.height};
	m_c_dest   = {m_c_position.x, m_c_position.y, (float)m_c_texture.width, (float)m_c_texture.height};
	Vector2 origin = {m_c_dest.width / 2, m_c_dest.height / 2};
	DrawTexturePro(m_c_texture, m_c_source, m_c_dest, origin, m_c_rotation, WHITE);
}

void Cannon::fire()
{
	if ( m_c_time_since_last_shot >= m_c_fire_rate_in_s && m_c_director) 
	{
		auto new_projectile = m_c_director->spawn_projectile(m_c_position);
		new_projectile->set_speed(m_c_projectile_speed);
		new_projectile->set_target_position(calculate_projectile_target_position());
		new_projectile->set_director(m_c_director);
		new_projectile->set_shared_ptr(new_projectile);
		printf("Cannon fired a projectile at position (%f, %f)\n", m_c_position.x, m_c_position.y);
		m_c_time_since_last_shot = 0.0f;
	}
}

Vector2 Cannon::calculate_projectile_target_position()
{
	Vector2 direction = {cosf(m_c_rotation * (PI / 180.0f)), sinf(m_c_rotation * (PI / 180.0f))};
	Vector2 target_position = {
			m_c_position.x + direction.x * m_c_range,
			m_c_position.y + direction.y * m_c_range
	};
	return target_position;
}

void Cannon::set_position_to_parent()
{
	if ( m_c_parent ) {
		
		switch ( m_c_positioning ) 
		{
			case Cannon::CannonPositioning::Right:
				set_position(coordinatesystem::point_relative_to_global_rightup(m_c_parent->position(), m_c_parent->rotation(), Vector2{0, 10}));
				break;

			case Cannon::CannonPositioning::Left:
				set_position(coordinatesystem::point_relative_to_global_rightup(m_c_parent->position(), m_c_parent->rotation(), Vector2{0, -10}));
				break;
		}
	}
}

void Cannon::set_rotation_to_parent()
{
	if ( m_c_parent ) {

		switch ( m_c_positioning ) {
			case Cannon::CannonPositioning::Right:
				set_rotation(m_c_parent->rotation() + 90);
				break;

			case Cannon::CannonPositioning::Left:
				set_rotation(m_c_parent->rotation() - 90);
				break;
		}
	}
}

void Cannon::set_director(FortunaDirector* director)
{
	m_c_director = director;
}

FortunaDirector* Cannon::director()
{
	return m_c_director;
}

Texture2D Cannon::texture()
{
	return m_c_texture;
}

void Cannon::set_texture(const Texture2D& texture)
{
	m_c_texture = texture;
}

Vector2 Cannon::position()
{
	return m_c_position;
}

void Cannon::set_position(const Vector2& position)
{
	m_c_position = position;
}

float Cannon::rotation()
{
	return m_c_rotation;
}

void Cannon::set_rotation(const float rotation)
{
	m_c_rotation = rotation;
}

float Cannon::range()
{
	return m_c_range;
}

void Cannon::set_range(const float range)
{
	m_c_range = range;
}

float Cannon::fire_rate()
{
	return m_c_fire_rate_in_s;
}

void Cannon::set_fire_rate(const float fire_rate)
{
	m_c_fire_rate_in_s = fire_rate;
}

float Cannon::projectile_speed()
{
	return m_c_projectile_speed;
}

void Cannon::set_projectile_speed(const float projectile_speed)
{
	m_c_projectile_speed = projectile_speed;
}

Vector2 Cannon::projectile_target_position()
{
	return m_c_projectile_target_position;
}

void Cannon::set_projectile_target_position(const Vector2& target_position)
{
	m_c_projectile_target_position = target_position;
}

std::shared_ptr<Player> Cannon::parent()
{
	return m_c_parent;
}

void Cannon::set_parent(std::shared_ptr<Player> parent)
{
	m_c_parent = parent;
}

Cannon::CannonPositioning Cannon::positioning()
{
	return m_c_positioning;
}

void Cannon::set_positioning(const Cannon::CannonPositioning positioning)
{
		m_c_positioning = positioning;
}
