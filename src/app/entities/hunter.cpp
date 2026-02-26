#include <coordinatesystem.h>
#include <entities/cannon.h>
#include <entities/director.h>
#include <entities/hunter.h>
#include <entities/player.h>
#include <memory>
#include <misc/mapborderinteraction.h>
#include <pscore/application.h>
#include <pscore/settings.h>
#include <pscore/utils.h>
#include <pscore/viewport.h>
#include <psinterfaces/entity.h>
#include <raylib.h>
#include <raymath.h>
#include <vector>

class HunterPriv
{
	friend class Hunter;

	PSCore::sprites::SpriteSheetAnimation animation_controller;
	std::shared_ptr<PSCore::sprites::Sprite> sprite;
	std::vector<Vector2> current_patrol_path;

	// movement
	Vector2 pos, prev_pos;
	float rotation				   = 0;
	float previous_rotation		   = 0;
	float point_t				   = 0;
	int current_point_index		   = 0;
	float speed					   = CFG_VALUE<float>("hunter_speed", 10);
	bool in_view				   = false;
	float avoidance_steer_strength = CFG_VALUE<float>("hunter_avoidance_steer_strength", 1000.f);

	Vector2 velocity = {0, 0};

	// Movement decoration
	Smear smear;
	void update_smear(float dt);
	float max_velocity		= 1;
	float rotation_velocity = 0;

	// wreck movement
	Vector2 forward_vec = {0, 0};

	// shooting
	float fire_intervall		  = 1.f;
	float time_since_last_shot	  = 0.f;
	int last_fired_cannon_index	  = 0;
	float cannon_projectile_range = CFG_VALUE<float>("hunter_cannon_projectile_range", 100.f);
	float cannon_projectile_speed = CFG_VALUE<float>("hunter_cannon_projectile_speed", 200.f);

	// iframes
	float invulnerability_time			 = CFG_VALUE<float>("hunter_invulnerability_time", 0.5f);
	float remaining_invulnerability_time = 0.f;

	// animation
	bool to_wreck_anim_playing = false;
	bool to_death_anim_playing = false;

	std::unique_ptr<PSCore::collision::EntityCollider> collider;

	Hunter::State current_state = Hunter::State::Patrolling;

	std::vector<std::shared_ptr<Cannon>> cannons;
};

void HunterPriv::update_smear(float dt)
{
	// calculate the rotation velocity for the smear effect
	if ( abs(rotation - previous_rotation) < 1 ) {
		rotation_velocity = (rotation - previous_rotation) / dt;
	}

	previous_rotation = rotation;

	smear.update_smear(rotation_velocity, -0.5, 10, dt);

	if ( auto& vp = gApp()->viewport() ) {

		Vector2 position_absolute	 = vp->position_viewport_to_global(pos);
		float scale					 = vp->viewport_scale();
		Vector2 smear_right_position = coordinatesystem::point_relative_to_global_leftup(position_absolute, rotation, Vector2Scale({20, 5}, scale));
		Vector2 smear_left_position	 = coordinatesystem::point_relative_to_global_leftdown(position_absolute, rotation, Vector2Scale({20, 5}, scale));
		Vector2 smear_forward_position =
				coordinatesystem::point_relative_to_global_rightup(position_absolute, rotation, Vector2Scale({24, 0}, scale));

		smear.calculate_linear_smear(smear_right_position, Vector2Length(velocity), rotation, 0.6f * scale, 0, 0);
		smear.calculate_linear_smear(smear_left_position, Vector2Length(velocity), rotation, 0.6f * scale, 0, 1);
		smear.calculate_exponential_smear(
				smear_forward_position, Vector2Length(velocity), rotation, 0.5f * scale, 0, 0.15f * scale, 0.25f * scale, 2
		);
		smear.calculate_exponential_smear(
				smear_forward_position, Vector2Length(velocity), rotation, 0.5f * scale, 0, -0.15f * scale, -0.25f * scale, 3
		);

		smear.add_smear_wave(0.1, 10, Vector2Length(velocity), max_velocity, dt, 0);

		smear.update_smear_wave({0, 1}, Linear, 1, 10, Vector2Length(velocity), max_velocity, dt);
	}
}

Hunter::Hunter() : PSInterfaces::IEntity("hunter")
{
	_p = std::make_unique<HunterPriv>();

	Vector2 frame_grid{3, 4};
	_p->sprite = PRELOAD_TEXTURE(ident_, "resources/entity/enemy_ship.png", frame_grid);

	std::vector<PSCore::sprites::SpriteSheetData> sp_data{
			{1, 1, PSCore::sprites::KeyFrame, 1},
			{1, 1, PSCore::sprites::KeyFrame, 2},
			{7, 0.1, PSCore::sprites::Forward, 1},
			{1, 1, PSCore::sprites::KeyFrame, 1},
			{7, 0.1, PSCore::sprites::Forward, 1}
	};

	_p->animation_controller = PSCore::sprites::SpriteSheetAnimation(FETCH_SPRITE_TEXTURE(ident_), sp_data);

	_p->animation_controller.add_animation_at_index(0, 1);
	_p->animation_controller.add_animation_at_index(1, 2);

	propose_z_index(30);
};

void Hunter::update(float dt)
{
	if ( !is_active_ )
		return;

	if ( _p->current_patrol_path.empty() )
		_p->current_patrol_path = gen_patrol_path();

	switch ( _p->current_state ) {
		case Patrolling: {
			_p->prev_pos = _p->pos;
			traverse_path_(dt);
			avoid_other_hunters_(dt);

			_p->forward_vec = coordinatesystem::vector_forward(_p->rotation);
			_p->velocity	= Vector2Scale(Vector2Subtract(_p->pos, _p->prev_pos), 1 / dt);

			for ( auto& cannon: _p->cannons ) {
				cannon->update(dt);
			}

			Vector2 to_player;
			if ( !player_in_range_(&to_player) )
				break;

			float side	  = (_p->forward_vec.x * to_player.y) - (_p->forward_vec.y * to_player.x);
			bool is_right = side < 0;

			fire_available_cannon_(is_right, dt);

			break;
		}
		case Wreck: {
			_p->current_patrol_path.clear();

			_p->pos = Vector2Add(_p->pos, _p->velocity * dt);

			if ( _p->animation_controller.get_sprite_sheet_frame_index(1).value_or(0) == 6 ) {
				if ( _p->to_wreck_anim_playing ) {
					_p->animation_controller.set_animation_at_index(3, 0, 1);
					_p->to_wreck_anim_playing = false;
				} else if ( _p->to_death_anim_playing ) {
					_p->to_death_anim_playing = false;
					set_is_active(false);
				}
			}

			break;
		}
	}

	if ( auto layer = gApp()->get_layer<AppLayer>() )
		_p->collider->check_collision(layer->entities());

	_p->update_smear(dt);
	_p->animation_controller.update_animation(dt);

	_p->remaining_invulnerability_time = std::max(0.f, _p->remaining_invulnerability_time - dt);

	if ( misc::map::is_off_screen(this) && _p->in_view )
		set_is_active(false);
	else if ( !misc::map::is_off_screen(this) && !_p->in_view )
		_p->in_view = true;
}

void Hunter::render()
{
	if ( auto& vp = gApp()->viewport() ) {
		if ( Vector2Length(_p->velocity) > 0 && !_p->to_death_anim_playing ) {
			_p->smear.draw_smear(0, Linear, 2 * vp->viewport_scale(), 1, {9, 75, 101, 127});
			_p->smear.draw_smear(1, Linear, 2 * vp->viewport_scale(), 1, {9, 75, 101, 127});
			_p->smear.draw_smear(2, Exponential, 2 * vp->viewport_scale(), 1, {9, 75, 101, 127});
			_p->smear.draw_smear(3, Exponential, 2 * vp->viewport_scale(), 1, {9, 75, 101, 127});
			_p->smear.draw_smear_wave(Vector2Length(_p->velocity), _p->max_velocity, 2 * vp->viewport_scale(), 1, {9, 75, 101, 127});
		}

		vp->draw_in_viewport(
				_p->sprite->m_s_texture, _p->animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), _p->pos, _p->rotation + 90, WHITE
		);

		for ( auto& cannon: _p->cannons ) {
			cannon->render();
		}

		if ( _p->current_state == Patrolling )
			vp->draw_in_viewport(
					_p->sprite->m_s_texture, _p->animation_controller.get_source_rectangle(2).value_or(Rectangle{0}), _p->pos, _p->rotation + 90,
					WHITE
			);
	}
}

void Hunter::draw_debug()
{
	if ( _p->current_patrol_path.empty() )
		return;

	float scale;
	if ( auto& vp = gApp()->viewport() ) {
		scale = vp->viewport_scale();

		std::vector<Vector2> debug_points = _p->current_patrol_path;
		for ( auto& point: debug_points ) {
			point.x *= scale;
			point.y *= scale;

			DrawRectanglePro({point.x, point.y, 10 * scale, 10 * scale}, {5 * scale, 5 * scale}, 0, GREEN);
		}

		Vector2* arr = new Vector2[debug_points.size()];
		std::copy(debug_points.begin(), debug_points.end(), arr);
		DrawSplineCatmullRom(arr, debug_points.size(), 3, RED);

		if ( bounds().has_value() ) {
			for ( int i = 0; i < bounds().value().size(); i++ ) {
				if ( i < bounds().value().size() - 1 ) {
					DrawLineV(bounds().value().at(i), bounds().value().at(i + 1), GREEN);
				} else {
					DrawLineV(bounds().value().at(i), bounds().value().at(0), GREEN);
				}
			}
		}

		auto vp_pos = vp->position_viewport_to_global(_p->pos);

		DrawLineV(vp_pos, vp_pos + Vector2Scale(_p->forward_vec, 100), RED);
	}
}

std::pair<Vector2, Vector2> Hunter::gen_path_egde()
{
	Vector2 vp_size;

	{
		auto& vp = gApp()->viewport();
		vp_size	 = vp->viewport_base_size();
	}

	Vector2 p1, p2;

	enum MapEdge {
		Left = 0,
		Right,

		Top,
		Bottom
	};

	MapEdge map_edge;

	auto gen_tip = [&map_edge, &vp_size, this]() -> Vector2 {
		MapEdge rand_map_edge = static_cast<MapEdge>(PSUtils::gen_rand(MapEdge::Left, MapEdge::Bottom));
		if ( rand_map_edge == map_edge )
			map_edge = static_cast<MapEdge>(PSUtils::clamp_with_overflow<int>(0, MapEdge::Bottom, map_edge + 1));
		else
			map_edge = rand_map_edge;

		float x, y;
		switch ( map_edge ) {
			case Left:
				x = -size().value().x;
				y = PSUtils::gen_rand(20, (vp_size.y) - 20);
				break;
			case Right:
				x = vp_size.x + size().value().x;
				y = PSUtils::gen_rand(20, (vp_size.y) - 20);
				break;
			case Top:
				x = PSUtils::gen_rand(20, (vp_size.y) - 20);
				y = -size().value().y;
				break;
			case Bottom:
				x = PSUtils::gen_rand(20, (vp_size.y) - 20);
				y = vp_size.x + size().value().y;
		}

		return {x, y};
	};

	p1 = gen_tip();
	p2 = gen_tip();

	return {p1, p2};
};

std::vector<Vector2> Hunter::gen_patrol_path()
{
	std::vector<Vector2> path_points;

	Vector2 vp_size;

	{
		auto& vp = gApp()->viewport();
		vp_size	 = vp->viewport_base_size();
	}

	auto tips = gen_path_egde();

	path_points.push_back(tips.first);
	path_points.push_back(tips.first);

	// Calculate vector from start to end
	Vector2 start = tips.first;
	Vector2 end	  = tips.second;
	Vector2 diff  = Vector2Subtract(end, start);
	float dist	  = Vector2Length(diff);
	Vector2 dir	  = Vector2Normalize(diff);
	Vector2 perp  = {-dir.y, dir.x}; // Perpendicular vector

	// Generate 2 intermediate points
	for ( int i = 1; i <= 3; i++ ) {
		float t		  = i / 3.0f;
		Vector2 point = Vector2Add(start, Vector2Scale(diff, t));

		// Add random perpendicular offset
		// Scale offset based on distance to allow for nice arcs, but keep it within reasonable bounds
		float offset_scale = dist * 0.3f;
		float offset	   = PSUtils::gen_rand(-offset_scale, offset_scale);
		point			   = Vector2Add(point, Vector2Scale(perp, offset));

		// Clamp point within viewport bounds with some margin
		point.x = std::clamp(point.x, 20.0f, vp_size.x - 20.0f);
		point.y = std::clamp(point.y, 20.0f, vp_size.y - 20.0f);

		path_points.push_back(point);
	}

	path_points.push_back(tips.second);
	path_points.push_back(tips.second);

	return path_points;
};

Hunter::~Hunter() {};

std::optional<std::vector<Vector2>> Hunter::bounds() const
{
	if ( is_active_ )
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(_p->pos);
			float scale	   = vp->viewport_scale();

			std::vector<Vector2> hitbox_points;

			switch ( _p->current_state ) {
				case Patrolling:
					hitbox_points = {{25 * scale, 0 * scale},  {12 * scale, 7 * scale},	  {-21 * scale, 7 * scale},
									 {-23 * scale, 0 * scale}, {-21 * scale, -7 * scale}, {12 * scale, -7 * scale}};
					break;
				case Wreck:
					hitbox_points = {
							{20 * scale, 0 * scale},
							{8 * scale, 6 * scale},
							{-20 * scale, 0 * scale},
							{-15 * scale, -6 * scale},
							{8 * scale, -6 * scale}
					};
					break;
			}

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, _p->rotation, hitbox_points);
		}

	return std::nullopt;
}

std::optional<Vector2> Hunter::position() const
{
	return _p->pos;
}

void Hunter::set_is_active(bool active)
{
	is_active_ = active;
	for ( auto& cannon: _p->cannons ) {
		cannon->set_is_active(active);
	}

	_p->current_patrol_path.clear();
	_p->current_state		  = State::Patrolling;
	_p->current_point_index	  = 0;
	_p->point_t				  = 0.0f;
	_p->forward_vec			  = {0, 0};
	_p->in_view				  = false;
	_p->to_wreck_anim_playing = false;
	_p->to_death_anim_playing = false;

	_p->animation_controller.set_animation_at_index(0, 0, 1);
}

void Hunter::traverse_path_(float dt)
{
	_p->point_t += dt * (_p->speed / 100);

	if ( _p->point_t >= 1.0f ) {
		_p->point_t -= 1.0f;
		_p->current_point_index++;

		// If we run out of segments (need 4 points for Catmull-Rom), reset
		if ( _p->current_point_index > _p->current_patrol_path.size() - 4 ) {
			_p->current_point_index = 0;
			set_is_active(false); // Deactivate the hunter until the next spawn
		}
	}

	if ( _p->current_patrol_path.size() >= 4 ) {
		_p->pos = GetSplinePointCatmullRom(
				_p->current_patrol_path[_p->current_point_index], _p->current_patrol_path[_p->current_point_index + 1],
				_p->current_patrol_path[_p->current_point_index + 2], _p->current_patrol_path[_p->current_point_index + 3], _p->point_t
		);

		// calcultae rotation
		Vector2 next_pos = GetSplinePointCatmullRom(
				_p->current_patrol_path[_p->current_point_index], _p->current_patrol_path[_p->current_point_index + 1],
				_p->current_patrol_path[_p->current_point_index + 2], _p->current_patrol_path[_p->current_point_index + 3],
				std::min(_p->point_t + 0.01f, 1.0f)
		);

		_p->rotation = atan2f(next_pos.y - _p->pos.y, next_pos.x - _p->pos.x) * (180.0f / PI);
	}
};

// void Hunter::avoid_other_hunters_(float dt)
// {
// }
void Hunter::avoid_other_hunters_(float dt)
{
	if ( !_p->current_patrol_path.size() )
		return;

	auto app_layer = gApp()->get_layer<AppLayer>();
	if ( !app_layer )
		return;

	Vector2 *next_path_point, *prev_path_point;
	try {
		next_path_point = &_p->current_patrol_path.at(_p->current_point_index + 2);
		prev_path_point = &_p->current_patrol_path.at(_p->current_point_index + 1);
	} catch ( const std::out_of_range& e ) {
		return;
	}

	Vector2 separation = {0, 0};
	int count		   = 0;

	const float avoid_radius = Vector2Length(Vector2Subtract(*next_path_point, *prev_path_point)) * 0.5;

	for ( auto& weak: app_layer->entities() ) {
		if ( auto other = weak.lock() ) {
			if ( other.get() == this )
				continue;

			auto other_hunter = std::dynamic_pointer_cast<Hunter>(other);
			if ( !other_hunter )
				continue;

			auto other_pos_opt = other_hunter->position();
			if ( !other_pos_opt.has_value() )
				continue;

			Vector2 diff = Vector2Subtract(_p->pos, other_pos_opt.value());
			float dist	 = Vector2Length(diff);
			if ( dist > 0 && dist < avoid_radius ) {
				// steer away, stronger when closer
				separation = Vector2Add(separation, Vector2Scale(Vector2Normalize(diff), 1.0f / dist));
				++count;
			}
		}
	}

	if ( count > 0 ) {
		separation = Vector2Scale(separation, 1.0f / count);

		float steer_strength = _p->avoidance_steer_strength;
		*next_path_point	 = Vector2Add(*next_path_point, Vector2Scale(separation, steer_strength * dt));
	}
}

void Hunter::init(std::shared_ptr<Hunter> self)
{
	_p->collider = std::make_unique<PSCore::collision::EntityCollider>(self);
	_p->collider->register_collision_handler(
			[](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
				if ( auto locked = other.lock() ) {

					FortunaDirector* director;
					if ( !(director = dynamic_cast<FortunaDirector*>(gApp()->game_director())) )
						return;

					if ( auto player = std::dynamic_pointer_cast<Player>(locked) ) {

						player->on_hit();

						const float repel_strenght = CFG_VALUE<int>("hunter_repel_bounce_strenght", 50);
						if ( auto& spawner = director->spawner<Hunter, AppLayer>() ) {
							Vector2 repel_force =
									PSCore::collision::entity_repel_force<Player>(player, spawner->primitive_entities(), 50, repel_strenght);

							repel_force.x = std::clamp(repel_force.x, (repel_strenght * 2) * -1, repel_strenght * 2);
							repel_force.y = std::clamp(repel_force.y, (repel_strenght * 2) * -1, repel_strenght * 2);

							player->apply_repel_force(repel_force);
						}
					}
				}
			},
			0.1f
	);

	float cannon_width;
	if ( !_p->cannons.empty() )
		cannon_width = static_cast<float>((_p->cannons[0]->texture().width));

	_p->cannons.reserve(6);

	int offsets[]{5, 5, -6, -6, -11, -11};
	while ( _p->cannons.size() < 6 ) {
		auto cannon = std::make_shared<Cannon>();

		cannon->set_parent(self);
		cannon->set_positioning(_p->cannons.size() % 2 == 0 ? Cannon::CannonPositioning::Right : Cannon::CannonPositioning::Left);
		cannon->set_parent_position_x_offset(offsets[_p->cannons.size()]);
		cannon->set_parent_position_y_offset(7);
		cannon->set_shared_ptr_this(cannon);

		cannon->set_projectile_speed(_p->cannon_projectile_speed);
		cannon->set_range(_p->cannon_projectile_range);

		_p->cannons.push_back(cannon);
	}
};

void Hunter::on_hit()
{
	if ( _p->remaining_invulnerability_time > 0 )
		return;
	_p->remaining_invulnerability_time = _p->invulnerability_time;

	switch ( _p->current_state ) {
		case Patrolling: {
			_p->current_state = State::Wreck;
			for ( auto& cannon: _p->cannons ) {
				cannon->set_is_active(false);
			}

			_p->to_wreck_anim_playing = true;
			_p->animation_controller.set_animation_at_index(2, 0, 1);
			break;
		}
		case Wreck: {
			_p->to_death_anim_playing = true;
			_p->animation_controller.set_animation_at_index(4, 0, 1);
			// TODO: if has upgrade, drop it
			break;
		}
	}
}

std::optional<float> Hunter::rotation() const
{
	return _p->rotation;
}

std::optional<Vector2> Hunter::velocity() const
{
	return _p->velocity;
}

std::optional<Vector2> Hunter::size() const
{
	auto h = _p->animation_controller.get_source_rectangle(1).value_or(Rectangle{0}).height;
	auto w = _p->animation_controller.get_source_rectangle(1).value_or(Rectangle{0}).width;
	return Vector2{w, h};
}

bool Hunter::player_in_range_(Vector2* to_player)
{
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
		return false;

	*to_player	  = Vector2Subtract(player_entity->position().value_or({0, 0}), _p->pos);
	auto distance = Vector2Length(*to_player);

	return distance <= CFG_VALUE<float>("hunter_shooting_distance", 150.f);
};

void Hunter::fire_available_cannon_(bool right_side, float dt)
{
	int cannon_to_be_fired = PSUtils::gen_rand(0, 2);
	if ( cannon_to_be_fired == _p->last_fired_cannon_index )
		cannon_to_be_fired = PSUtils::clamp_with_overflow(0, 2, cannon_to_be_fired + 1);

	if ( _p->time_since_last_shot >= _p->fire_intervall ) {
		int to_be_fired = right_side ? (cannon_to_be_fired * 2) + 1 : cannon_to_be_fired * 2;

		_p->cannons[to_be_fired]->fire();

		_p->time_since_last_shot	= 0.f;
		_p->last_fired_cannon_index = cannon_to_be_fired;
	} else {
		_p->time_since_last_shot += dt;
	}
};

float Hunter::gen_phase_offset_()
{
	return PSUtils::gen_rand(0.f, 1.f);
};
