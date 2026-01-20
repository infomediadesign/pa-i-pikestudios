#include <entities/player.h>
#include <entities/shark.h>
#include <imgui.h>
#include <memory>
#include <pscore/application.h>
#include <pscore/utils.h>
#include <pscore/viewport.h>
#include <psinterfaces/entity.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <raymath.h>
#include <utilities.h>

//
// Fin of Shark
//
Fin::Fin(Shark* shark) : PSInterfaces::IEntity("shark_fin"), m_shark(shark)
{
	auto tex		= m_shark->m_shark_sprite;
	auto frame_rect = tex->frame_rect({0, 0});
	m_size			= {frame_rect.width, frame_rect.height};
}

Fin::~Fin()
{
}

void Fin::render()
{
	if ( auto& vp = gApp()->viewport() ) {
		auto tex = m_shark->m_shark_sprite;
		vp->draw_in_viewport(tex->m_s_texture, tex->frame_rect({0, 1}), m_shark->m_pos, m_shark->m_shark_rotation + 90, RED);
	}
}

void Fin::update(float dt)
{
}

void Fin::draw_debug()
{
}

//
// Body of Shark
//
Body::Body(Shark* shark) : PSInterfaces::IEntity("shark_body"), m_shark(shark)
{
	auto tex		= m_shark->m_shark_sprite;
	auto frame_rect = tex->frame_rect({0, 0});
	m_size			= {frame_rect.width, frame_rect.height};
}

Body::~Body()
{
}

void Body::render()
{
	if ( auto& vp = gApp()->viewport() ) {
		auto tex = m_shark->m_shark_sprite;
		vp->draw_in_viewport(tex->m_s_texture, tex->frame_rect({0, 0}), m_shark->m_pos, m_shark->m_shark_rotation + 90, WHITE);
	}
}

void Body::update(float dt)
{
}

void Body::draw_debug()
{
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

	// Has an droppable upgrade
	m_marked = PSUtils::gen_rand(1, 100) > 50;
}

Shark::~Shark()
{
	if ( m_marked ) {
		PS_LOG(LOG_INFO, "Dropped an upgrade");
		// TODO: implement loot drop
	}
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

	m_shark_rotation = utilities::rotation_look_at(m_pos, player_entity->position());

	switch ( m_state ) {
		case Idle: {
			m_state_string = "idle";
			m_state		   = State::Pursuing;
			break;
		}
		case Pursuing: {
			m_state_string = "pursuing";

			m_speed = 100;
			if ( distance > 20.0f ) // stop when close enough
			{
				direction = Vector2Normalize(direction);
				m_pos	  = Vector2Add(m_pos, Vector2Scale(direction, m_speed * dt));
			} else {
				m_state = State::Attacking;
			}

			break;
		}
		case Attacking: {
			m_state_string = "attacking";
			PS_LOG(LOG_INFO, "Attacked the player");
			// TODO: implement damage
			m_state = State::Retreat;
			break;
		}
		case Retreat: {
			m_state_string = "retreat";
			if ( distance > 40.0f )
				m_state = State::Pursuing;

			m_speed	  = 20;
			direction = Vector2Normalize(direction);
			direction = Vector2Negate(direction);
			m_pos	  = Vector2Add(m_pos, Vector2Scale(direction, m_speed * dt));

			break;
		}
	}
}

void Shark::draw_debug()
{
	m_body->draw_debug();
	m_fin->draw_debug();

	Rectangle shark_rec;
	shark_rec = m_shark_sprite->frame_rect({0, 0});

	Vector2 new_pos;
	float scale;
	if ( auto& vp = gApp()->viewport() ) {
		new_pos = vp->position_viewport_to_global(m_pos);
		scale	= vp->viewport_scale();
	}

	shark_rec.x = new_pos.x;
	shark_rec.y = new_pos.y;
	shark_rec.width *= scale;
	shark_rec.height *= scale;
	PSUtils::DrawRectangleLinesRotated(shark_rec, m_shark_rotation + 90, RED);

	DrawText(m_state_string.c_str(), shark_rec.x + 20, shark_rec.y + 20, 12, RED);
}

void Shark::render()
{
	m_body->render();
	m_fin->render();
}

void Shark::set_pos(const Vector2& pos)
{
	m_pos = pos;
}
