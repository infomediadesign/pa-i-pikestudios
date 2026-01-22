#include "cannon.h"
#include <coordinatesystem.h>
#include <entities/director.h>
#include <memory>
#include <pscore/application.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <raylib.h>

Cannon::Cannon() : PSInterfaces::IEntity("cannon")
{
	IRenderable::propose_z_index(2);
	m_c_position = {100.0f, 100.0f};
	m_c_rotation = 0.0f;
	Vector2 frame_grid{1, 1};
	m_c_sprite	= PRELOAD_TEXTURE(ident_, "ressources/entity/test_cannon.png", frame_grid);
	m_c_texture	= m_c_sprite->m_s_texture;
	m_c_range	 = 500.0f;
	m_c_time_since_last_shot = 0.0f;
	m_c_fire_rate_in_s		 = 0.5f;
	m_c_projectile_speed	 = 1000.0f;
	m_c_parent_position_x_offset = 0.0f;
	m_c_parent_position_y_offset = 10.0f;
}

void Cannon::update(const float dt)
{

	set_position_to_parent();
	set_rotation_to_parent();

	m_c_time_since_last_shot += dt;
	if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) ) {
		if ( m_c_positioning == CannonPositioning::Left ) {
			fire();
		}
	}

	if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ) {
		if ( m_c_positioning == CannonPositioning::Right ) {
			fire();
		}
	}
}

void Cannon::render()
{
	if ( m_c_is_active ) {
		m_c_source = {0, 0, (float) m_c_texture.width, (float) m_c_texture.height};
		if ( auto& vp = gApp()->viewport() ) {
			vp->draw_in_viewport(m_c_texture, m_c_source, m_c_position, m_c_rotation, WHITE);
		}
	}
}

bool Cannon::is_active() const
{
	return m_c_is_active;
}

void Cannon::set_is_active(const bool active)
{
	m_c_is_active = active;
}

void Cannon::fire()
{
	if ( m_c_time_since_last_shot >= m_c_fire_rate_in_s ) {
		auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
		if ( !director ) {
			return;
		}
		auto new_projectile = director->spawn_projectile(m_c_position);
		new_projectile->set_speed(m_c_projectile_speed);
		new_projectile->set_target_position(calculate_projectile_target_position());
		new_projectile->set_shared_ptr(new_projectile);
		if ( m_c_parent ) {
			new_projectile->set_owner_velocity(m_c_parent->velocity());
		}
		m_c_time_since_last_shot = 0.0f;
	}
}

Vector2 Cannon::calculate_projectile_target_position()
{
	Vector2 direction		= {cosf(m_c_rotation * (PI / 180.0f)), sinf(m_c_rotation * (PI / 180.0f))};
	Vector2 target_position = {m_c_position.x + direction.x * m_c_range, m_c_position.y + direction.y * m_c_range};
	return target_position;
}

void Cannon::set_position_to_parent()
{
	if ( !m_c_parent ) {
		return;
	}

	switch ( m_c_positioning ) {
		case Cannon::CannonPositioning::Right:
			set_position(
					coordinatesystem::point_relative_to_global_rightup(
							m_c_parent->position().value_or({0, 0}), m_c_parent->rotation(),
							Vector2{m_c_parent_position_x_offset, m_c_parent_position_y_offset}
					)
			);
			break;

		case Cannon::CannonPositioning::Left:
			set_position(
					coordinatesystem::point_relative_to_global_rightdown(
							m_c_parent->position().value_or({0, 0}), m_c_parent->rotation(),
							Vector2{m_c_parent_position_x_offset, m_c_parent_position_y_offset}
					)
			);
			break;
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

Texture2D Cannon::texture()
{
	return m_c_texture;
}

void Cannon::set_texture(const Texture2D& texture)
{
	m_c_texture = texture;
}

std::optional<Vector2> Cannon::position() const
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

float Cannon::dest_width()
{
	return m_c_dest.width;
}

float Cannon::dest_height()
{
	return m_c_dest.height;
}

float Cannon::parent_position_x_offset()
{
	return m_c_parent_position_x_offset;
}

void Cannon::set_parent_position_x_offset(const float offset)
{
	m_c_parent_position_x_offset = offset;
}

float Cannon::parent_position_y_offset()
{
	return m_c_parent_position_y_offset;
}

void Cannon::set_parent_position_y_offset(const float offset)
{
	m_c_parent_position_y_offset = offset;
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
