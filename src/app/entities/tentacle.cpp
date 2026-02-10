//
// Created by tylor on 30/01/2026.
//

#include "tentacle.h"

#include <iostream>
#include <pscore/application.h>
#include <pscore/viewport.h>

#include <pscore/utils.h>

#include "coordinatesystem.h"
#include "director.h"
#include "layers/applayer.h"
#include "player.h"

tentacle::tentacle() : PSInterfaces::IEntity("tentacle")
{
	Vector2 frame_grid{9, 2};
	PRELOAD_TEXTURE(ident_, "ressources/entity/tentacle.png", frame_grid);
	m_Tentacle_sprite = FETCH_SPRITE(ident_);

	std::vector<PSCore::sprites::SpriteSheetData> sp_data;
	sp_data.push_back({8, 0.1, PSCore::sprites::Forward, 0});
	sp_data.push_back({6, 0.1, PSCore::sprites::Backward, 1});
	sp_data.push_back({1, 1, PSCore::sprites::Forward, 2});
	sp_data.push_back({1,1, PSCore::sprites::Forward, 3});

	m_animation_controller = PSCore::sprites::SpriteSheetAnimation(FETCH_SPRITE_TEXTURE(ident_), sp_data);

	m_animation_controller.add_animation_at_index(0, 0);
	m_animation_controller.add_animation_at_index(1, 1);
	m_animation_controller.add_animation_at_index(2, 2);
	m_animation_controller.add_animation_at_index(3, 3);
}

void tentacle::init(std::shared_ptr<tentacle> self, const Vector2& pos)
{
	m_self = self;
	m_pos  = pos;

	m_collider = std::make_unique<PSCore::collision::EntityCollider>(m_self);
	m_collider->register_collision_handler([](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
		if ( auto locked = other.lock() ) {
			if ( auto player = std::dynamic_pointer_cast<Player>(locked) ) {
				std::cout << "HOIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIt" << std::endl;
				player->on_hit();
			}
		}
	});
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
				auto tex = m_Tentacle_sprite;
				vp->draw_in_viewport(tex->m_s_texture, m_animation_controller.get_source_rectangle(3).value_or(Rectangle{0}), m_pos, 0, WHITE);
				break;
			}
			case WaterBreak: {
				auto tex = m_Tentacle_sprite;
				vp->draw_in_viewport(tex->m_s_texture, m_animation_controller.get_source_rectangle(0).value_or(Rectangle{0}), m_pos, 0, WHITE);
				break;
			}
			case Attacking: {
				auto tex = m_Tentacle_sprite;
				vp->draw_in_viewport(tex->m_s_texture, m_animation_controller.get_source_rectangle(2).value_or(Rectangle{0}), m_pos, 0, WHITE);
				break;
			}
			case Retreat: {
				auto tex = m_Tentacle_sprite;
				vp->draw_in_viewport(tex->m_s_texture, m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), m_pos, 0, WHITE);
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


void tentacle::IdleUpdate(float dt)
{
	time_until_water_break -= dt;
	if ( time_until_water_break <= 0 ) {
		m_state = State::WaterBreak;
		m_animation_controller.set_animation_at_index(0, 0, 0);
		propose_z_index(-10);
	}
}

void tentacle::WaterBreakUpdate(float dt)
{
	time_until_attack -= dt;
	if ( time_until_attack <= 0 ) {
		m_state = State::Attacking;
		m_animation_controller.set_animation_at_index(2, 0, 2);
	}
}

void tentacle::AttackingUpdate(float dt)
{
	time_until_retreat -= dt;

	if ( time_until_retreat <= 0 ) {
		m_animation_controller.set_animation_at_index(1, 5, 1);
		m_state = State::Retreat;
	}
}
void tentacle::RetreatingUpdate(float dt)
{
	until_reposition -= dt;
	if ( until_reposition <= 0 ) {
		m_state = State::Idle;

		time_until_water_break = PSUtils::gen_rand(4, 8);
		time_until_attack	   = max_time_until_attack;
		time_until_retreat	   = max_time_until_retreat;
		until_reposition	   = max_until_reposition;

		SetNewPos();
		propose_z_index(-40);
	}
}
void tentacle::SetNewPos()
{
	if ( auto& vp = gApp()->viewport() ) {
		Vector2 coords;
		coords.x = PSUtils::gen_rand(0, vp->viewport_base_size().x);
		coords.y = PSUtils::gen_rand(0, vp->viewport_base_size().y);
		set_pos(coords);
	}
}

void tentacle::on_hit()
{
	set_is_active(false);
	if ( const auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
		director->m_b_bounty.add_bounty(225);
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
