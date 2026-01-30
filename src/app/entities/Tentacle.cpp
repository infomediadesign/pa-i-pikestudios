//
// Created by tylor on 30/01/2026.
//

#include "Tentacle.h"
#include <pscore/application.h>
#include <pscore/viewport.h>

#include <pscore/utils.h>
#include "layers/applayer.h"
#include "player.h"

Tentacle::Tentacle() : PSInterfaces::IEntity("tentacle")
{
	Vector2 frame_grid{9, 2};
	PRELOAD_TEXTURE(ident_, "ressources/entity/tentacle.png", frame_grid);
	m_Tentacle_sprite = FETCH_SPRITE(ident_);

	m_animation_controller = PSCore::sprites::SpriteSheetAnimation(FETCH_SPRITE_TEXTURE(ident_), {{1, 1, PSCore::sprites::Forward, 0}});

	m_animation_controller.add_animation_at_index(0, 0);
}


void Tentacle::init(std::shared_ptr<Tentacle> self, const Vector2& pos)
{
	m_self = self;
	m_pos  = pos;

	m_collider = std::make_unique<PSCore::collision::EntityCollider>(m_self);
	m_collider->register_collision_handler([](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
		if ( auto locked = other.lock() ) {
			if ( auto player = std::dynamic_pointer_cast<Player>(locked) ) {
				player->damage();
			}
		}
	});
}
Tentacle::~Tentacle()
{
}


void Tentacle::render()
{
	if ( auto& vp = gApp()->viewport() ) {
		auto tex = m_Tentacle_sprite;
		vp->draw_in_viewport(tex->m_s_texture, m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), m_pos, 0, RED);
	}
}

void Tentacle::update(float dt)
{
	m_animation_controller.update_animation(dt);

	switch ( m_state ) {
		case 0: {
			IdleUpdate(dt);
			break;
		}
		case 1: {
			AttackingUpdate(dt);
			break;
		}
		case 2: {
			RetreatingUpdate(dt);
			break;
		}
	}
}


void Tentacle::IdleUpdate(float dt)
{
	time_until_attack -= dt;
	if ( time_until_attack <= 0 ) {
		m_state = State::Attacking;
		m_animation_controller.set_animation_at_index(0, 0, 0);
	}
}
void Tentacle::AttackingUpdate(float dt)
{
	time_until_retreat -= dt;

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		m_collider->check_collision(app_layer->entities());
	}

	if ( time_until_retreat <= 0 ) {
		m_state = State::Retreat;
	}
}
void Tentacle::RetreatingUpdate(float dt)
{
	until_reposition -= dt;
	if ( until_reposition <= 0 ) {
		m_state = State::Idle;


		time_until_attack  = max_time_until_attack;
		time_until_retreat = max_time_until_retreat;
		until_reposition   = max_until_reposition;

		SetNewPos();
	}
}
void Tentacle::SetNewPos()
{
	if ( auto& vp = gApp()->viewport() ) {
		Vector2 coords;
		coords.x = PSUtils::gen_rand(0, vp->viewport_base_size().x);
		coords.y = PSUtils::gen_rand(0, vp->viewport_base_size().y);
	}
}

void Tentacle::on_hit()
{
}
