#include <entities/director.h>
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
#include <vector>
#include "coordinatesystem.h"
#include "layers/applayer.h"
#include "pscore/collision.h"

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
		vp->draw_in_viewport(
				tex->m_s_texture, m_shark->m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), m_shark->m_pos,
				m_shark->m_shark_rotation + 90, WHITE
		);

		if ( m_shark->m_state == Shark::Pursuing ) {
			m_smear.draw_smear(0, Exponential, 2 * vp->viewport_scale(), 1, m_smear_color);
			m_smear.draw_smear(1, Exponential, 2 * vp->viewport_scale(), 1, m_smear_color);
		}
	}
}

void Fin::update(float dt)
{
	if ( auto& vp = gApp()->viewport() ) {
		Vector2 position_absolute = vp->position_viewport_to_global(m_shark->m_pos);
		float scale				  = vp->viewport_scale();
		Vector2 smear_forward_position =
				coordinatesystem::point_relative_to_global_rightup(position_absolute, m_shark->m_shark_rotation, Vector2Scale({5, -0.5}, scale));

		m_smear.calculate_exponential_smear(
				smear_forward_position, m_shark->m_speed, m_shark->m_shark_rotation, 0.2f * scale, 0, 0.03f * scale, 0.05f * scale, 0
		);
		m_smear.calculate_exponential_smear(
				smear_forward_position, m_shark->m_speed, m_shark->m_shark_rotation, 0.2f * scale, 0, -0.03f * scale, -0.05f * scale, 1
		);
	}
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
		vp->draw_in_viewport(
				tex->m_s_texture, m_shark->m_animation_controller.get_source_rectangle(-1).value_or(Rectangle{0}), m_shark->m_pos,
				m_shark->m_shark_rotation + 90, WHITE
		);
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
	PRELOAD_TEXTURE(ident_, "resources/entity/shark.png", frame_grid);
	m_shark_sprite = FETCH_SPRITE(ident_);

	m_animation_controller = PSCore::sprites::SpriteSheetAnimation(
			FETCH_SPRITE_TEXTURE(ident_), {{9, 0.1, PSCore::sprites::Forward, -1}, {9, 0.1, PSCore::sprites::Forward, 1}}
	);

	m_animation_controller.add_animation_at_index(0, -1);
	m_animation_controller.add_animation_at_index(1, 1);

	m_body = std::make_shared<Body>(this);
	m_fin  = std::make_shared<Fin>(this);

	m_body->propose_z_index(-30);
	m_fin->propose_z_index(-10);

	// Has an droppable upgrade
	float drop_roll = static_cast<float>(PSUtils::gen_rand(0, 1000)) / 1000.0f;
	m_marked		= drop_roll < m_drop_upgrade_chance;
}

void Shark::init(std::shared_ptr<Shark> self, const Vector2& pos)
{
	m_self = self;
	m_pos  = pos;

	m_director = dynamic_cast<FortunaDirector*>(gApp()->game_director());

	m_collider = std::make_unique<PSCore::collision::EntityCollider>(m_self);
	m_collider->register_collision_handler([](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
		if ( auto locked = other.lock() ) {
			if ( auto player = std::dynamic_pointer_cast<Player>(locked) ) {
				player->on_hit();
			}
		}
	});

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		app_layer->renderer()->submit_renderable(m_body);
		app_layer->renderer()->submit_renderable(m_fin);
	}
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

	m_animation_controller.update_animation(dt);

	m_rotation_velocity = calculate_rotation_velocity(0.01, dt);

	std::shared_ptr<Player> player_entity;

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		for ( auto entity: app_layer->entities() ) {
			if ( auto locked = entity.lock() ) {
				if ( auto player = std::dynamic_pointer_cast<Player>(locked) )
					player_entity = player;
			}
		}
	}
	if ( !player_entity )
		return;

	Vector2 player_pos = player_entity->position().value();

	Vector2 direction = Vector2Subtract(player_pos, m_pos);
	float distance	  = Vector2Length(direction);

	m_shark_rotation = utilities::rotation_look_at(m_pos, player_pos);

	auto& spawner = m_director->spawner<Shark, AppLayer>();

	Vector2 separation =
			PSCore::collision::entity_repel_force<Shark, Shark, AppLayer>(m_self, *spawner, m_horde_separation_distance, m_horde_separation_strength);

	// Cohesion: steer toward the average position of nearby sharks
	Vector2 cohesion{0, 0};
	{
		Vector2 center_sum{0, 0};
		int neighbor_count = 0;

		for ( const auto& other: spawner->entities() ) {
			if ( !other || !other->is_active() || other.get() == this )
				continue;

			Vector2 other_pos = other->position().value_or(Vector2{0, 0});
			float dist		  = Vector2Distance(m_pos, other_pos);

			if ( dist < m_horde_cohesion_radius && dist > 0.001f ) {
				center_sum = Vector2Add(center_sum, other_pos);
				neighbor_count++;
			}
		}

		if ( neighbor_count > 0 ) {
			Vector2 center_of_mass = Vector2Scale(center_sum, 1.0f / (float) neighbor_count);
			Vector2 to_center	   = Vector2Subtract(center_of_mass, m_pos);
			if ( Vector2Length(to_center) > 0.001f ) {
				cohesion = Vector2Scale(Vector2Normalize(to_center), m_horde_cohesion_strength);
			}
		}
	}

	Vector2 horde_force = Vector2Add(separation, cohesion);

	switch ( m_state ) {
		case Idle: {
			m_state_string = "idle";
			m_state		   = State::Pursuing;
			break;
		}
		case Pursuing: {
			m_state_string = "pursuing";
			if ( distance > m_pursue_stop_distance ) // stop when close enough
			{
				Vector2 pursue_dir = Vector2Scale(Vector2Normalize(direction), m_speed);
				Vector2 combined   = Vector2Add(pursue_dir, horde_force);
				combined		   = Vector2Normalize(combined);

				m_pos = Vector2Add(m_pos, Vector2Scale(combined, m_speed * dt));

				if ( m_horde_sync_rotation )
					m_shark_rotation = utilities::rotation_look_at(m_pos, Vector2Add(m_pos, combined));
			} else {
				m_state = State::Attacking;
			}

			break;
		}
		case Attacking: {
			m_state_string = "attacking";
			PS_LOG(LOG_INFO, "Attacked the player");
			if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
				m_collider->check_collision(app_layer->entities());
			}
			// TODO: implement damage
			m_state = State::Retreat;
			break;
		}
		case Retreat: {
			m_state_string = "retreat";
			if ( distance > m_retreat_reengage_distance )
				m_state = State::Pursuing;

			Vector2 retreat_dir = Vector2Scale(Vector2Normalize(Vector2Negate(direction)), m_retreat_speed);
			Vector2 combined	= Vector2Add(retreat_dir, horde_force);
			combined			= Vector2Normalize(combined);

			m_pos = Vector2Add(m_pos, Vector2Scale(combined, m_retreat_speed * dt));

			if ( m_horde_sync_rotation )
				m_shark_rotation = utilities::rotation_look_at(m_pos, Vector2Add(m_pos, combined));

			break;
		}
	}
}

void Shark::on_hit()
{
	set_is_active(false);
	printf("hit shark\n");
	m_director->m_b_bounty.add_bounty(m_director->m_b_bounty_amounts.shark_bounty);
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

	if ( bounds().has_value() ) {
		for ( int i = 0; i < bounds().value().size(); i++ ) {
			if ( i < bounds().value().size() - 1 ) {
				DrawLineV(bounds().value().at(i), bounds().value().at(i + 1), GREEN);
			} else {
				DrawLineV(bounds().value().at(i), bounds().value().at(0), GREEN);
			}
		}
	}

	DrawText(m_state_string.c_str(), shark_rec.x + 20, shark_rec.y + 20, 12, RED);

	// Shark Debug values
	// ImGui::Text("Shark Speed: %.0f", m_speed);
	// ImGui::SameLine();
	// ImGui::SetNextItemWidth(60);
	// static float buffer = 0;
	// ImGui::InputFloat("##range_amount", &buffer, 0.0f, 0.0f, "%.0f");
	// ImGui::SameLine();
	// if ( ImGui::Button("Upgrade##Speed") ) {
	// 	m_speed = buffer;
	// }
}

void Shark::render()
{
}

void Shark::set_pos(const Vector2& pos)
{
	m_pos = pos;
}

std::optional<std::vector<Vector2>> Shark::bounds() const
{
	if ( is_active() )
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(m_pos);
			float scale	   = vp->viewport_scale();

			std::vector<Vector2> hitbox_points = {{15 * scale, 0 * scale}, {0 * scale, 8 * scale}, {-15 * scale, 0 * scale}, {0 * scale, -8 * scale}};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, m_shark_rotation, hitbox_points);
		}

	return std::nullopt;
};

std::optional<Vector2> Shark::position() const
{
	return m_pos;
};

void Shark::set_is_active(bool active)
{
	is_active_ = active;
	m_body->set_is_active(active);
	m_fin->set_is_active(active);
}

float Shark::calculate_rotation_velocity(float frequency, float dt)
{
	static float timer			   = 0;
	static float last_rotation	   = 0;
	static float rotation_velocity = 0;

	timer += dt;

	if ( timer >= frequency ) {
		timer = 0;

		rotation_velocity = m_shark_rotation - last_rotation;

		last_rotation = m_shark_rotation;
	}

	return rotation_velocity;
}
