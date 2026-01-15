#include <entities/shark.h>
#include <iostream>
#include <memory>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include "entities/player.h"
#include "pscore/application.h"

//
// Fin of Shark
//
Fin::Fin(Shark* shark) : m_shark(shark)
{
}

Fin::~Fin()
{
}

void Fin::render()
{
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
Body::Body(Shark* shark) : m_shark(shark)
{
}

Body::~Body()
{
}

void Body::render()
{
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
Shark::Shark()
{
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
	for ( auto entity : gApp()->entities() ) {
		if ( auto locked = entity.lock() ) {
			if ( auto player = dynamic_cast<Player*>(locked.get()) )
				player_entity = player;
		}
	}
	if (!player_entity)
		return;
	
	
	
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
