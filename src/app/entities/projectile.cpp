#include "projectile.h"
#include <cmath>
#include <coordinatesystem.h>
#include <cstdlib>
#include <entities/director.h>
#include <iostream>
#include <layers/applayer.h>
#include <memory>
#include <optional>
#include <ostream>
#include <pscore/application.h>
#include <pscore/collision.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <psinterfaces/entity.h>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "pscore/utils.h"

Projectile::Projectile() : PSInterfaces::IEntity("projectile")
{
	m_p_z_index = 5;
	IRenderable::propose_z_index(m_p_z_index);
	m_p_position = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	m_p_rotation = 0.0f;
	Vector2 frame_grid{1, 1};
	m_p_sprite	= PRELOAD_TEXTURE(ident_, "resources/entity/projectile.png", frame_grid);
	m_p_texture = m_p_sprite->m_s_texture;
	Vector2 hit_anim_frame_grid{1, 1};

	// Hit Anim
	m_p_hit_anim_sprite = PRELOAD_TEXTURE("projectile_hit_anim", "resources/vfx/default_hit.png", hit_anim_frame_grid);

	m_p_hit_anim_texture	 = m_p_hit_anim_sprite->m_s_texture;
	m_p_animation_controller = PSCore::sprites::SpriteSheetAnimation(m_p_hit_anim_texture, {{9, 0.1, PSCore::sprites::Forward, m_p_z_index}});
	m_p_animation_controller.add_animation_at_index(0, m_p_z_index);

	// No Hit Anim
	m_p_no_hit_anim_sprite = PRELOAD_TEXTURE("projectile_no_hit_anim", "resources/vfx/water_splash.png", hit_anim_frame_grid);

	m_p_no_hit_anim_texture	   = m_p_no_hit_anim_sprite->m_s_texture;
	m_p_no_hit_anim_controller = PSCore::sprites::SpriteSheetAnimation(m_p_no_hit_anim_texture, {{10, 0.1, PSCore::sprites::Forward, m_p_z_index}});
	m_p_no_hit_anim_controller.add_animation_at_index(0, m_p_z_index);
}

void Projectile::init(const Vector2& position, std::shared_ptr<Projectile> self)
{
	set_position(position);
	m_p_shared_ptr = self;
	m_collider	   = std::make_unique<PSCore::collision::EntityCollider>(m_p_shared_ptr);
	m_collider->register_collision_handler([this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
		if ( auto locked = other.lock() )
			if ( locked->is_active() ) {
				locked->on_hit();
				on_hit();
			}
	});
}

void Projectile::update(const float dt)
{
	if ( !m_p_hit_aninm_playing && !m_p_no_hit_anim_playing ) {
		calculate_movment(dt);

		if ( position()->x == 0 || position()->y == 0 )
			return;

		if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
			m_collider->check_collision(app_layer->entities(), [this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point) {
				if ( auto l = other.lock() ) {
					bool is_player = l->ident() == "player";
					bool is_same   = l->ident() == ident_;
					bool is_cannon = l->ident() == "cannon";
					bool is_loot   = l->ident() == "loot_chest";
					return !(is_player || is_same || is_cannon || is_loot);
				}
				return true;
			});
		}
	}
	play_hit_anim(dt);
	play_no_hit_anim(dt);
	update_pierce_hit_anims(dt);
}

void Projectile::render()
{
	for ( auto& pierce: m_p_pierce_hit_anims ) {
		if ( !pierce.finished ) {
			if ( auto& vp = gApp()->viewport() ) {
				vp->draw_in_viewport(
						m_p_hit_anim_texture, pierce.anim_controller.get_source_rectangle(m_p_z_index).value_or(Rectangle{0}), pierce.position,
						pierce.rotation, WHITE
				);
			}
		}
	}

	if ( is_active_ && !m_p_hit_aninm_playing && !m_p_no_hit_anim_playing ) {
		m_p_source = {0, 0, (float) m_p_texture.width, (float) m_p_texture.height};
		if ( auto& vp = gApp()->viewport() ) {
			vp->draw_in_viewport(m_p_texture, m_p_source, m_p_position, m_p_rotation, WHITE);
		}
	}
	if ( m_p_hit_aninm_playing ) {
		if ( auto& vp = gApp()->viewport() ) {
			vp->draw_in_viewport(
					m_p_hit_anim_texture, m_p_animation_controller.get_source_rectangle(m_p_z_index).value_or(Rectangle{0}), m_p_hit_anim_pos,
					m_p_rotation, WHITE
			);
		}
	}
	if ( m_p_no_hit_anim_playing ) {
		if ( auto& vp = gApp()->viewport() ) {
			vp->draw_in_viewport(
					m_p_no_hit_anim_texture, m_p_no_hit_anim_controller.get_source_rectangle(m_p_z_index).value_or(Rectangle{0}), m_p_position,
					m_p_rotation, WHITE
			);
		}
	}
}

void Projectile::on_hit()
{
	if ( can_pierce() ) {
		PierceHitAnim pierce;
		pierce.position		   = m_p_position;
		pierce.rotation		   = m_p_rotation;
		pierce.anim_controller = PSCore::sprites::SpriteSheetAnimation(m_p_hit_anim_texture, {{9, 0.1, PSCore::sprites::Forward, m_p_z_index}});
		pierce.anim_controller.add_animation_at_index(0, m_p_z_index);
		pierce.anim_controller.set_animation_at_index(0, 0, m_p_z_index);
		m_p_pierce_hit_anims.push_back(std::move(pierce));
		return;
	}
	m_p_hit_anim_pos = m_p_position;
	m_p_animation_controller.set_animation_at_index(0, 0, m_p_z_index);
	m_p_hit_aninm_playing = true;
}

void Projectile::play_hit_anim(float dt)
{
	if ( m_p_hit_aninm_playing ) {
		m_p_animation_controller.update_animation(dt);
		if ( m_p_animation_controller.get_sprite_sheet_frame_index(m_p_z_index) == 8 ) {
			m_p_animation_controller.set_animation_at_index(0, 0, m_p_z_index);
			m_p_hit_aninm_playing = false;
			m_p_pierce_hit_anims.clear();
			set_is_active(false);
		}
	}
}

void Projectile::play_no_hit_anim(float dt)
{
	if ( m_p_no_hit_anim_playing ) {
		m_p_no_hit_anim_controller.update_animation(dt);
		if ( m_p_no_hit_anim_controller.get_sprite_sheet_frame_index(m_p_z_index) == 9 ) {
			m_p_no_hit_anim_controller.set_animation_at_index(0, 0, m_p_z_index);
			m_p_no_hit_anim_playing = false;
			m_p_pierce_hit_anims.clear();
			set_is_active(false);
		}
	}
}

void Projectile::update_pierce_hit_anims(float dt)
{
	for ( auto& pierce: m_p_pierce_hit_anims ) {
		if ( !pierce.finished ) {
			pierce.anim_controller.update_animation(dt);
			if ( pierce.anim_controller.get_sprite_sheet_frame_index(m_p_z_index) == 8 ) {
				pierce.finished = true;
			}
		}
	}
	for ( auto index = m_p_pierce_hit_anims.begin(); index != m_p_pierce_hit_anims.end(); ) {
		if ( index->finished ) {
			index = m_p_pierce_hit_anims.erase(index);
		} else {
			++index;
		}
	}
}

void Projectile::apply_drag(const float dt)
{
	float frame_drag = std::pow(m_p_drag_per_second, dt);

	m_p_velocity.x *= frame_drag;
	m_p_velocity.y *= frame_drag;
	m_p_owner_velocity.x *= frame_drag;
	m_p_owner_velocity.y *= frame_drag;
}

void Projectile::calculate_movment(const float dt)
{
	if ( !m_p_fiering_cannon ) {
		return;
	}

	apply_drag(dt);

	Vector2 combined_velocity = {m_p_velocity.x + m_p_owner_velocity.x, m_p_velocity.y + m_p_owner_velocity.y};

	m_p_position.x += combined_velocity.x * dt;
	m_p_position.y += combined_velocity.y * dt;

	m_p_travel_distance += Vector2Length(combined_velocity) * dt;

	if ( m_p_travel_distance >= m_p_max_range ) {
		m_p_no_hit_anim_playing = true;
		m_p_no_hit_anim_controller.set_animation_at_index(0, 0, m_p_z_index);
	}
}

void Projectile::launch()
{
	if ( !m_p_fiering_cannon ) {
		return;
	}

	m_p_position = m_p_fiering_cannon->position().value_or(Vector2{0, 0});

	float cannon_rot = m_p_fiering_cannon->rotation();
	float rad		 = cannon_rot * (PI / 180.0f);

	m_p_velocity = {cosf(rad) * m_p_speed, sinf(rad) * m_p_speed};

	if ( m_p_owner ) {
		m_p_owner_velocity = m_p_owner->velocity();
	}

	Vector2 combined = {m_p_velocity.x + m_p_owner_velocity.x, m_p_velocity.y + m_p_owner_velocity.y};
	m_p_rotation	 = atan2f(combined.y, combined.x) * (180.0f / PI);

	m_p_travel_distance = 0.0f;
	m_p_pierce_hit_anims.clear();
}

bool Projectile::can_pierce() const
{
	float roll = PSUtils::gen_rand_float(0.01f, 100.0f);
	return roll <= m_p_piercing_chance;
}

float Projectile::piercing_chance() const
{
	return m_p_piercing_chance;
}

void Projectile::set_piercing_chance(const float chance)
{
	m_p_piercing_chance = chance;
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

float Projectile::max_range()
{
	return m_p_max_range;
}

void Projectile::set_max_range(const float max_range)
{
	m_p_max_range = max_range;
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
	if ( is_active() )
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(m_p_position);
			float scale	   = vp->viewport_scale();

			std::vector<Vector2> hitbox_points = {{1 * scale, 1 * scale}, {1 * scale, -1 * scale}, {-1 * scale, -1 * scale}, {-1 * scale, 1 * scale}};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, m_p_rotation, hitbox_points);
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

		Vector2 cannon_screen	  = vp->position_viewport_to_global(cannon_pos);
		Vector2 projectile_screen = vp->position_viewport_to_global(m_p_position);

		DrawLineEx(cannon_screen, projectile_screen, 2.0f, RED);
	}
}
