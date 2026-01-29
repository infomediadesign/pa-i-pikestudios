#include "projectile.h"
#include <coordinatesystem.h>
#include <entities/director.h>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <pscore/application.h>
#include <pscore/collision.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <raylib.h>

#include <layers/applayer.h>
#include <psinterfaces/entity.h>
#include <raymath.h>
#include <vector>

Projectile::Projectile() : PSInterfaces::IEntity("projectile")
{

	IRenderable::propose_z_index(1);
	m_p_position = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	m_p_rotation = 0.0f;
	Vector2 frame_grid{1, 1};
	m_p_sprite			= PRELOAD_TEXTURE(ident_, "ressources/entity/test_projectile.png", frame_grid);
	m_p_texture			= m_p_sprite->m_s_texture;
	m_p_target_position = {360.0f, 360.f};
	m_p_speed			= 200.0f;
}

void Projectile::init(const Vector2& position, std::shared_ptr<Projectile> self)
{
	set_position(position);
	m_p_shared_ptr = self;
	m_collider	   = std::make_unique<PSCore::collision::EntityCollider>(m_p_shared_ptr);
	m_collider->register_collision_handler([this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
		set_is_active(false);
		if ( auto locked = other.lock() )
			locked->set_is_active(false);
	});
}

void Projectile::update(const float dt)
{
	m_p_target_position += m_p_owner_velocity * dt;
	calculate_movement(dt, m_p_target_position);

	if ( position()->x == 0 || position()->y == 0 )
		return;

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		m_collider->check_collision(app_layer->entities(), [this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point) {
			if ( auto l = other.lock() ) {
				bool is_player = l->ident() == "player";
				bool is_same   = l->ident() == ident_;
				return !(is_player || is_same);
			}

			return true;
		});
	}
}

void Projectile::render()
{
	if ( is_active_ ) {
		m_p_source = {0, 0, (float) m_p_texture.width, (float) m_p_texture.height};
		if ( auto& vp = gApp()->viewport() ) {
			vp->draw_in_viewport(m_p_texture, m_p_source, m_p_position, m_p_rotation, WHITE);
		}
	}
}

void Projectile::draw_debug()
{
	if ( bounds().has_value() ) {
		for ( int i = 0; i < bounds().value().size(); i++ ) {
			if ( i < bounds().value().size() - 1 ) {
				DrawLineV(bounds().value().at(i), bounds().value().at(i + 1), GREEN);
			} else {
				DrawLineV(bounds().value().at(i), bounds().value().at(0), GREEN);
			}
		}
	}
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
		is_active_ = false;
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

std::optional<Vector2> Projectile::position() const
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

std::optional<std::vector<Vector2>> Projectile::bounds() const
{
	if ( is_active() )
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(m_p_position);
			float scale	   = vp->viewport_scale();

			std::vector<Vector2> hitbox_points = {
					{1 * scale, 1 * scale}, {1 * scale, -1 * scale}, {-1 * scale, -1 * scale}, {-1 * scale, 1 * scale}
			};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, m_p_rotation, hitbox_points);
		}

	return std::nullopt;

	return std::nullopt;
};
