//
// Created by tylor on 30/01/2026.
//

#include "tentacle.h"

#include <algorithm>
#include <pscore/application.h>
#include <pscore/viewport.h>

#include <pscore/utils.h>
#include <raylib.h>

#include <layers/applayer.h>
#include <pscore/collision.h>
#include <pscore/settings.h>
#include "coordinatesystem.h"
#include "director.h"
#include "player.h"

tentacle::tentacle() : PSInterfaces::IEntity("tentacle")
{
	Vector2 frame_grid{9, 2};
	PRELOAD_TEXTURE(ident_, "resources/entity/tentacle.png", frame_grid);
	m_tentacle_sprite = FETCH_SPRITE(ident_);

	std::vector<PSCore::sprites::SpriteSheetData> sp_data;
	sp_data.push_back({9, 0.1, PSCore::sprites::Forward, 1});
	sp_data.push_back({7, 0.1, PSCore::sprites::Backward, 1});
	sp_data.push_back({2, 1, PSCore::sprites::KeyFrame, 1});
	sp_data.push_back({4, 0.15, PSCore::sprites::Forward, 0});

	m_animation_controller = PSCore::sprites::SpriteSheetAnimation(m_tentacle_sprite->m_s_texture, sp_data);

	m_animation_controller.add_animation_at_index(3, 0);
	m_animation_controller.add_animation_at_index(2, 1);

	m_animation_controller.set_animation_at_index(2, 1, 1);

	time_until_water_break = static_cast<float>(PSUtils::gen_rand(4, 8));
	time_until_retreat	   = max_time_until_retreat;
}

void tentacle::init(std::shared_ptr<tentacle> self, const Vector2& pos)
{
	m_self = self;
	m_pos  = pos;

	m_collider = std::make_unique<PSCore::collision::EntityCollider>(m_self);
	m_collider->register_collision_handler(
			[](std::vector<std::weak_ptr<PSInterfaces::IEntity>> others, const Vector2& pos) {
				for ( const auto& other: others ) {
					if ( auto locked = other.lock() ) {
						if ( auto player = std::dynamic_pointer_cast<Player>(locked) ) {
							player->on_hit();

							FortunaDirector* director;
							if ( !(director = dynamic_cast<FortunaDirector*>(gApp()->game_director())) )
								return;

							const float repel_strenght = CFG_VALUE<int>("tentacle_repel_bounce_strenght", 50);
							if ( auto& spawner = director->spawner<tentacle, AppLayer>() ) {
								Vector2 repel_force =
										PSCore::collision::entity_repel_force<Player>(player, spawner->primitive_entities(), 50, repel_strenght);

								repel_force.x = std::clamp(repel_force.x, (repel_strenght * 2) * -1, repel_strenght * 2);
								repel_force.y = std::clamp(repel_force.y, (repel_strenght * 2) * -1, repel_strenght * 2);

								player->apply_repel_force(repel_force);
							}
						}
					}
				}
			},
			0.1f
	);
}

std::optional<Vector2> tentacle::position() const
{
	return m_pos;
}

tentacle::~tentacle()
{
}


void tentacle::render()
{
	if ( const auto& vp = gApp()->viewport() ) {
		switch ( m_state ) {
			case Idle: {
				m_animation_controller.set_animation_at_index(2, 1, 1);
				propose_z_index(-40);
				vp->draw_in_viewport(
						m_tentacle_sprite->m_s_texture, m_animation_controller.get_source_rectangle(0).value_or(Rectangle{0}), m_pos, 0, WHITE
				);
				break;
			}
			case WaterBreak: {
				if ( m_animation_controller.get_sprite_sheet_animation_index(1).value_or(-1) != 0 ) {
					m_animation_controller.set_animation_at_index(0, 0, 1);
				}
				propose_z_index(-2);
				vp->draw_in_viewport(
						m_tentacle_sprite->m_s_texture, m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), m_pos, 0, WHITE
				);
				break;
			}
			case Attacking: {
				if ( m_animation_controller.get_sprite_sheet_animation_index(1).value_or(-1) != 2 ) {
					m_animation_controller.set_animation_at_index(2, 0, 1);
				}
				propose_z_index(-2);
				vp->draw_in_viewport(
						m_tentacle_sprite->m_s_texture, m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), m_pos, 0, WHITE
				);
				break;
			}
			case Retreat: {
				if ( m_animation_controller.get_sprite_sheet_animation_index(1).value_or(-1) != 1 ) {
					m_animation_controller.set_animation_at_index(1, 5, 1);
				}
				propose_z_index(-2);
				vp->draw_in_viewport(
						m_tentacle_sprite->m_s_texture, m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), m_pos, 0, WHITE
				);
				break;
			}
		}
	}
}

void tentacle::update(float dt)
{
	m_animation_controller.update_animation(dt);

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		m_collider->check_collision(app_layer->entities());
	}

	if ( is_active_ ) {
		switch ( m_state ) {
			case Idle: {
				IdleUpdate(dt);
				break;
			}
			case WaterBreak: {
				WaterBreakUpdate(dt);
				break;
			}
			case Attacking: {
				AttackingUpdate(dt);
				break;
			}
			case Retreat: {
				RetreatingUpdate(dt);
				break;
			}
		}
	}
}


void tentacle::IdleUpdate(float dt)
{
	time_until_water_break -= dt;
	if ( time_until_water_break <= 0 ) {
		time_until_water_break = static_cast<float>(PSUtils::gen_rand(4, 8));
		m_state				   = State::WaterBreak;
	}
}

void tentacle::WaterBreakUpdate(float dt)
{
	if ( m_animation_controller.get_sprite_sheet_frame_index(1).value_or(-1) == 8 ) {
		m_state = State::Attacking;
	}
}

void tentacle::AttackingUpdate(float dt)
{
	time_until_retreat -= dt;
	if ( time_until_retreat <= 0 ) {
		time_until_retreat = max_time_until_retreat;
		m_state			   = State::Retreat;
		is_check_valid_	   = false;
	}
}
void tentacle::RetreatingUpdate(float dt)
{
	if ( m_animation_controller.get_sprite_sheet_frame_index(1).value_or(-1) == 0 ) {
		m_state			= State::Idle;
		is_check_valid_ = true;
		SetNewPos();
	}
}
void tentacle::SetNewPos()
{
	if ( auto& vp = gApp()->viewport() ) {
		Vector2 coords = {0, 0};
		coords.x = static_cast<float>(PSUtils::gen_rand(m_spawn_area_margin, static_cast<int>(vp->viewport_base_size().x) - m_spawn_area_margin));
		coords.y = static_cast<float>(PSUtils::gen_rand(m_spawn_area_margin, static_cast<int>(vp->viewport_base_size().y) - m_spawn_area_margin));

		FortunaDirector* director;
		if ( !(director = dynamic_cast<FortunaDirector*>(gApp()->game_director())) )
			return;

		std::vector<Vector2> other_positions;
		if ( auto& spawner = director->spawner<tentacle, AppLayer>() ) {
			for ( auto entity: spawner->primitive_entities() ) {
				if ( !entity->position().has_value() || uid_ == entity->uid() || !entity->is_active() || !entity->is_check_valid() )
					continue;

				other_positions.push_back(entity->position().value());
			}
			for ( int i = 0; i < 3; i++ ) {
				bool valid = true;
				for ( int j = 0; auto position: other_positions ) {
					if ( Vector2Length(coords - position) < m_spawn_distance ) {
						coords = other_positions.at(j) + Vector2Scale(Vector2Normalize(coords - position), m_spawn_distance * 1.05f);
						valid  = false;
					}
					j++;
				}
				if ( valid )
					break;
			}
			coords.x = std::clamp(
					coords.x, static_cast<float>(m_spawn_area_margin), vp->viewport_base_size().x - static_cast<float>(m_spawn_area_margin)
			);
			coords.y = std::clamp(
					coords.y, static_cast<float>(m_spawn_area_margin), vp->viewport_base_size().y - static_cast<float>(m_spawn_area_margin)
			);
		}
		set_pos(coords);
	}
}

void tentacle::on_hit()
{
	set_is_active(false);
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( director ) {
		director->stats.tentacles_killed++;
		determine_gem_drop();
	}
}

void tentacle::set_pos(const Vector2& pos)
{
	m_pos = pos;
}

std::optional<std::vector<Vector2>> tentacle::bounds() const
{
	if ( m_state == State::Idle ) {
		return std::nullopt;
	}
	if ( is_active() )
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(m_pos);
			float scale	   = vp->viewport_scale();

			std::vector<Vector2> hitbox_points = {{-10 * scale, 39 * scale}, {25 * scale, 9 * scale}, {-20 * scale, -1 * scale}};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, 0, hitbox_points);
		}

	return std::nullopt;
};

void tentacle::draw_debug()
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

void tentacle::set_is_active(bool active)
{
	if ( active ) {
		// Reset all states
		time_until_water_break = static_cast<float>(PSUtils::gen_rand(4, 8));
		time_until_retreat	   = max_time_until_retreat;
		m_animation_controller.set_animation_at_index(2, 1, 1);
		m_state			= State::Idle;
		is_check_valid_ = true;
		propose_z_index(-40);
		SetNewPos();
	}
	is_active_ = active;
}

void tentacle::determine_gem_drop()
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( !director ) {
		return;
	}
	if ( PSUtils::gen_rand_float(0, 100) <= director->gem_drop_chance() ) {
		director->spawn_gemstone(m_pos);
	}
}

tentacle::State tentacle::state()
{
	return m_state;
}
