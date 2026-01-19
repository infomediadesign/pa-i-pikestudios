#include <entities/shark.h>
#include <iostream>
#include <memory>
#include <pscore/viewport.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <raymath.h>
#include "entities/player.h"
#include "pscore/application.h"
#include "psinterfaces/entity.h"
#include "utilities.h"

//
// Fin of Shark
//
Fin::Fin(Shark* shark) : PSInterfaces::IEntity("shark_fin"), m_shark(shark)
{
	auto tex = m_shark->m_shark_sprite;
	auto frame_rect = tex->frame_rect({0,0});
	m_size = {frame_rect.width,  frame_rect.height};
}

Fin::~Fin()
{
}

void Fin::render()
{
	if ( auto& vp = gApp()->viewport() ) {
		auto tex = m_shark->m_shark_sprite;
		vp->draw_in_viewport(tex->m_s_sprite, tex->frame_rect({0, 1}), m_shark->pos_to_rel_fin(), m_shark->m_shark_rotation + 90, RED);
	}
}

void Fin::update(float dt)
{
}

void Fin::draw_debug()
{
	auto rel = m_shark->pos_to_rel_fin();
	Rectangle rec{rel.x, rel.y, m_size.x, m_size.y};
	DrawRectangleLinesEx(rec, 2.0f, RED);
}

//
// Body of Shark
//
Body::Body(Shark* shark) : PSInterfaces::IEntity("shark_body"), m_shark(shark)
{
	auto tex = m_shark->m_shark_sprite;
	auto frame_rect = tex->frame_rect({0,0});
	m_size = {frame_rect.width,  frame_rect.height};
}

Body::~Body()
{
}

void Body::render()
{
	if ( auto& vp = gApp()->viewport() ) {
		auto tex = m_shark->m_shark_sprite;
		vp->draw_in_viewport(tex->m_s_sprite, tex->frame_rect({0, 0}), m_shark->m_pos, m_shark->m_shark_rotation + 90, WHITE);
	}
}

void Body::update(float dt)
{
}

void Body::draw_debug()
{
	Vector2 pos = m_shark->m_pos;
	Rectangle rec{pos.x, pos.y, m_size.x, m_size.y};
	DrawRectangleLinesEx(rec, 2.0f, GREEN);
}

//
// The Shark itself
//
Shark::Shark() : PSInterfaces::IEntity("shark")
{
	Vector2 frame_grid{9, 2};
	PRELOAD_TEXTURE(ident_, "ressources/hai.png", frame_grid);
	m_shark_sprite = FETCH_SPRITE(ident_);

	m_body = std::make_shared<Body>(this);
	m_fin  = std::make_shared<Fin>(this);

	m_body->propose_z_index(-1);
	m_fin->propose_z_index(1);
}

Shark::~Shark()
{
}

void Shark::update(float dt)
{
	m_body->update(dt);
	m_fin->update(dt);

	Player* player_entity = nullptr;
	for ( auto entity: gApp()->entities() ) {
		if ( auto locked = entity.lock() ) {
			if ( auto player = dynamic_cast<Player*>(locked.get()) )
				player_entity = player;
		}
	}
	if ( !player_entity )
		return;

	Vector2 player_pos = player_entity->position();

	Vector2 direction = Vector2Subtract(player_pos, m_pos);
	float distance	  = Vector2Length(direction);

	if ( distance > 1.0f ) // stop when close enough
	{
		direction = Vector2Normalize(direction);
		m_pos	  = Vector2Add(m_pos, Vector2Scale(direction, m_speed * dt));
	}

	m_shark_rotation = utilities::rotation_look_at(m_pos, player_entity->position());
}

void Shark::draw_debug()
{
	m_body->draw_debug();
	m_fin->draw_debug();
}

void Shark::render()
{
	m_body->render();
	m_fin->render();
}

Vector2 Shark::pos_to_rel_fin()
{
	Vector2 rel_pos;

	rel_pos.x = (m_pos.x) + (m_fin->m_size.x);
	rel_pos.y = (m_pos.y) + (m_fin->m_size.y / 2);

	return rel_pos;
}
