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
}

void Projectile::init(const Vector2& position, std::shared_ptr<Projectile> self)
{
	set_position(position);
	m_p_shared_ptr = self;
	m_collider	   = std::make_unique<PSCore::collision::EntityCollider>(m_p_shared_ptr);
	m_collider->register_collision_handler([this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
		if ( auto locked = other.lock() )
			if ( locked->is_active() ) {
				set_is_active(false);
				locked->on_hit();
			}

			
	});
}

void Projectile::update(const float dt)
{
	calculate_movment(dt);

	if ( position()->x == 0 || position()->y == 0 )
		return;

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		m_collider->check_collision(app_layer->entities(), [this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point) {
			if ( auto l = other.lock() ) {
				bool is_player = l->ident() == "player";
				bool is_same   = l->ident() == ident_;
				bool is_cannon = l->ident() == "cannon";
				return !(is_player || is_same || is_cannon);
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

void Projectile::calculate_movment(const float dt)
{
	fire_from_cannon(dt);
	parent_to_cannon();
}

void Projectile::calculate_parenting()
{
	if ( !m_p_fiering_cannon ) {
		return;
	}

	Vector2 cannon_pos = m_p_fiering_cannon->position().value_or(Vector2{0, 0});
	float cannon_rot   = m_p_fiering_cannon->rotation();

	Vector2 diff = Vector2Subtract(m_p_target_position, cannon_pos);
	float rad = -cannon_rot * (PI / 180.0f);
	Vector2 target_local_offset = {
		diff.x * cosf(rad) - diff.y * sinf(rad),
		diff.x * sinf(rad) + diff.y * cosf(rad)
	};

	m_p_initial_distance = Vector2Length(target_local_offset);
	m_p_local_direction = Vector2Normalize(target_local_offset);

	m_p_local_offset = {0, 0};
	m_p_position = cannon_pos;
}

void Projectile::fire_from_cannon(const float dt)
{
	if ( !m_p_fiering_cannon ) {
		return;
	}

	float current_distance = Vector2Length(m_p_local_offset);

	if ( current_distance >= m_p_initial_distance ) {
		is_active_ = false;
		return;
	}

	m_p_local_offset.x += m_p_local_direction.x * m_p_speed * dt;
	m_p_local_offset.y += m_p_local_direction.y * m_p_speed * dt;

	m_p_travel_distance = Vector2Length(m_p_local_offset);
}

void Projectile::parent_to_cannon()
{
	if ( !m_p_fiering_cannon ) {
		return;
	}
	Vector2 cannon_pos = m_p_fiering_cannon->position().value_or(Vector2{0, 0});
	float cannon_rot   = m_p_fiering_cannon->rotation();

	m_p_position = coordinatesystem::point_relative_to_global_rightup(
		cannon_pos,
		cannon_rot,
		m_p_local_offset
	);
	m_p_rotation = cannon_rot;
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

std::shared_ptr<Cannon> Projectile::fiering_cannon()
{
	return m_p_fiering_cannon;
}

void Projectile::set_fiering_cannon(const std::shared_ptr<Cannon>& cannon)
{
	m_p_fiering_cannon = cannon;
}

std::optional<std::vector<Vector2>> Projectile::bounds() const
{
	Rectangle projectile_rec;
	projectile_rec = m_p_sprite->frame_rect({0, 0});

	if ( is_active() )
		if ( auto& vp = gApp()->viewport() ) {

			auto scaled_pos = vp->position_viewport_to_global(m_p_position);

			auto p1 = coordinatesystem::point_relative_to_global_rightup(
					scaled_pos, m_p_rotation, {projectile_rec.width / 2, projectile_rec.height / 2}
			);
			auto p2 = coordinatesystem::point_relative_to_global_rightup(
					scaled_pos, m_p_rotation, {-projectile_rec.width / 2, projectile_rec.height / 2}
			);
			auto p3 = coordinatesystem::point_relative_to_global_rightup(
					scaled_pos, m_p_rotation, {-projectile_rec.width / 2, -projectile_rec.height / 2}
			);
			auto p4 = coordinatesystem::point_relative_to_global_rightup(
					scaled_pos, m_p_rotation, {projectile_rec.width / 2, -projectile_rec.height / 2}
			);

			std::vector<Vector2> v{p1, p2, p3, p4};
			return v;
		}

	return std::nullopt;
};
void Projectile::draw_debug()
{
	if ( !is_active_ || !m_p_fiering_cannon ) {
		return;
	}

	if ( auto& vp = gApp()->viewport() ) {
		Vector2 cannon_pos = m_p_fiering_cannon->position().value_or(Vector2{0, 0});
		
		Vector2 cannon_screen = vp->position_viewport_to_global(cannon_pos);
		Vector2 projectile_screen = vp->position_viewport_to_global(m_p_position);

		DrawLineEx(cannon_screen, projectile_screen, 2.0f, RED);
	}
}
