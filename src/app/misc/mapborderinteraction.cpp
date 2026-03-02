#include "mapborderinteraction.h"

#include <entities/director.h>
#include <entities/player.h>
#include <layers/applayer.h>
#include <memory>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raylib.h>
#include <vector>
#include <psinterfaces/entity.h>

namespace misc {

	namespace map {

		namespace {
			WrapAroundMode wrapAroundMode = OffScreen;
			std::vector<SpawnRequest> spawns;
		} // namespace

		void map_border_wrap_around(Player& player)
		{
			detect_map_border_collision(player, spawns);

			if ( wrapAroundMode == OnScreen && !spawns.empty() ) {
				auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
				for ( const auto& request: spawns ) {
					spawn_new_player_at_opposite_border(request, director);
				}
				spawns.clear();
			}
		}

		void detect_map_border_collision(Player& p, std::vector<SpawnRequest>& spawnRequests)
		{
			int screen_w;
			int screen_h;

			if ( auto& vp = gApp()->viewport() ) {
				screen_w = static_cast<int>(vp->viewport_base_size().x);
				screen_h = static_cast<int>(vp->viewport_base_size().y);
			} else {
				screen_w = GetScreenWidth();
				screen_h = GetScreenHeight();
			}

			Vector2 player_pos = p.position().value();

			switch ( wrapAroundMode ) {
				case WrapAroundMode::OnScreen:
					use_on_screen_wrap_around(p, player_pos, screen_w, screen_h, spawnRequests);
					break;
				case WrapAroundMode::OffScreen:
					use_off_screen_wrap_around(p, player_pos, screen_w, screen_h);
					break;
				default:
					break;
			}
		}

		void spawn_new_player_at_opposite_border(const SpawnRequest& request, FortunaDirector* director)
		{
			if ( !director )
				return;

			float half_w = request.width / 2.0f;
			float half_h = request.height / 2.0f;

			Vector2 pos = calculate_opposite_position(request.position, half_w, half_h, request.axis);

			auto new_player = director->spawn_player(pos);
			new_player->set_border_collision_active_horizontal(true);
			new_player->set_border_collision_active_vertical(true);
			new_player->set_is_clone(true);
			new_player->set_velocity(request.velocity);
			new_player->set_rotation(request.rotation);
		}

		void set_player_position_to_opposite_border(Player& p, LastCollisionAxis axis)
		{
			auto size = p.size().value_or(Vector2{0, 0});
			float half_w = size.x / 2.0f;
			float half_h = size.y / 2.0f;

			Vector2 new_pos = calculate_opposite_position(p.position().value(), half_w, half_h, axis);
			p.set_position(new_pos);
		}

		bool is_off_screen(const PSInterfaces::IEntity* p)
		{
			int screen_w;
			int screen_h;

			if ( auto& vp = gApp()->viewport() ) {
				screen_w = static_cast<int>(vp->viewport_base_size().x);
				screen_h = static_cast<int>(vp->viewport_base_size().y);
			} else {
				screen_w = GetScreenWidth();
				screen_h = GetScreenHeight();
			}

			Vector2 pos = p->position().value();

			auto size = p->size().value_or(Vector2{0, 0});
			float half_w = size.x / 2.0f;
			float half_h = size.y / 2.0f;

			if ( half_w <= 0.0f || half_h <= 0.0f )
				return false;

			constexpr float margin = 10.0f;
			return (pos.x + half_w < -margin) || (pos.x - half_w > screen_w + margin) || (pos.y + half_h < -margin) ||
				   (pos.y - half_h > screen_h + margin);
		}

		bool check_collision_horizontal(Player& p, Vector2 player_pos, int screen_w)
		{
			float half_w = p.size().value_or({0,0}).x / 2.0f;

			bool fully_inside = (player_pos.x - half_w > 0.0f) && (player_pos.x + half_w < screen_w);
			if ( fully_inside )
				p.set_border_collision_active_horizontal(false);

			bool collides = (player_pos.x - half_w <= 0.0f) || (player_pos.x + half_w >= screen_w);
			return collides && !p.border_collision_active_horizontal();
		}

		bool check_collision_vertical(Player& p, Vector2 player_pos, int screen_h)
		{
			float half_h = p.size().value_or({0,0}).y / 2.0f;

			bool fully_inside = (player_pos.y - half_h > 0.0f) && (player_pos.y + half_h < screen_h);
			if ( fully_inside )
				p.set_border_collision_active_vertical(false);

			bool collides = (player_pos.y - half_h <= 0.0f) || (player_pos.y + half_h >= screen_h);
			return collides && !p.border_collision_active_vertical();
		}

		void request_spawn(Player& p, std::vector<SpawnRequest>& spawnRequests, LastCollisionAxis axis)
		{
			if ( axis == LastCollisionAxis::Horizontal )
				p.set_border_collision_active_horizontal(true);
			else if ( axis == LastCollisionAxis::Vertical )
				p.set_border_collision_active_vertical(true);

			spawnRequests.push_back(
					{.position = p.position().value(),
					 .velocity = p.velocity().value_or(Vector2{0, 0}),
					 .rotation = p.rotation().value_or(0),
					 .height   = p.size().value_or(Vector2{0, 0}).y,
					 .width	   = p.size().value_or(Vector2{0, 0}).x,
					 .axis	   = axis}
			);
		}

		void use_on_screen_wrap_around(Player& p, Vector2& player_pos, int screen_w, int screen_h, std::vector<SpawnRequest>& spawnRequests)
		{
			if ( misc::map::check_collision_horizontal(p, player_pos, screen_w) )
				misc::map::request_spawn(p, spawnRequests, LastCollisionAxis::Horizontal);

			if ( misc::map::check_collision_vertical(p, player_pos, screen_h) )
				misc::map::request_spawn(p, spawnRequests, LastCollisionAxis::Vertical);
		}


		void use_off_screen_wrap_around(Player& p, Vector2& player_pos, int screen_w, int screen_h)
		{
			if ( misc::map::check_collision_horizontal(p, player_pos, screen_w) && misc::map::is_off_screen(&p) )
				misc::map::set_player_position_to_opposite_border(p, LastCollisionAxis::Horizontal);

			if ( misc::map::check_collision_vertical(p, player_pos, screen_h) && misc::map::is_off_screen(&p) )
				misc::map::set_player_position_to_opposite_border(p, LastCollisionAxis::Vertical);
		}

		Vector2 calculate_opposite_position(Vector2 pos, float half_w, float half_h, LastCollisionAxis axis)
		{
			int screen_w;
			int screen_h;

			if ( auto& vp = gApp()->viewport() ) {
				screen_w = static_cast<int>(vp->viewport_base_size().x);
				screen_h = static_cast<int>(vp->viewport_base_size().y);
			} else {
				screen_w = GetScreenWidth();
				screen_h = GetScreenHeight();
			}

			switch ( axis ) {
				case LastCollisionAxis::Horizontal:
					pos.x = (pos.x - half_w <= 0.0f) ? static_cast<float>(screen_w) + half_w : -half_w;
					break;
				case LastCollisionAxis::Vertical:
					pos.y = (pos.y - half_h <= 0.0f) ? static_cast<float>(screen_h) + half_h : -half_h;
					break;
				default:
					break;
			}
			return pos;
		}

		void process_off_screen_entities()
		{
			if ( wrapAroundMode != OnScreen )
				return;

			auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
			if ( !director )
				return;

			auto app_layer = gApp()->get_layer<AppLayer>();
			if ( !app_layer )
				return;

			for ( auto& entity: app_layer->entities() ) {
				if ( auto player = std::dynamic_pointer_cast<Player>(entity.lock()) ) {
					if ( misc::map::is_off_screen(player.get()) ) {
						director->destroy_player(player);
						// player->set_is_active(false);
						/*
						for ( auto& cannons: player->cannon_container() )
						{
							cannons->set_is_active(false);
						}
						*/
					}
				}
			}
		}

		void set_wrap_around_mode(bool on_screen)
		{
			wrapAroundMode = on_screen ? OnScreen : OffScreen;
		}

	} // namespace map
} // namespace misc
