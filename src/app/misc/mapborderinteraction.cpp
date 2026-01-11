#include <algorithm>
#include "mapborderinteraction.h"
#include <vector>
#include "entities/player.h"
#include "raylib.h"
#include <pscore/application.h>
#include "layers/applayer.h"

WrapAroundMode wrapAroundMode = OnScreen;

std::vector<SpawnRequest> spawns;

void map_border_wrap_around(Player& player)
{

	detect_map_border_collision(player, spawns);

	if ( wrapAroundMode == OnScreen ) {
		auto app	  = PSCore::Application::get();
		auto appLayer = app->get_layer<AppLayer>();

		for ( auto& request: spawns ) {
			spawn_new_player_at_opposite_border(request, appLayer);
			printf("Spawned new player at opposite border\n");
		}
		spawns.clear();
	}
}

void process_offscreen_entities()
{
	if ( wrapAroundMode != OnScreen )
		return;

	auto app = PSCore::Application::get();
	AppLayer* appLayer = app->get_layer<AppLayer>();

	for ( auto entity: app->entities() ) {
		if ( auto player = std::dynamic_pointer_cast<Player>(entity.lock()) ) {
			if ( is_off_screen(*player) ) {
				if ( appLayer ) {
					appLayer->destroy_player(player);
					printf("Destroyed offscreen player\n");
				}
			}
		}
	}
}

void detect_map_border_collision(Player& p, std::vector<SpawnRequest>& spawnRequests)
{
	int screenW		   = GetScreenWidth();
	int screenH		   = GetScreenHeight();
	Vector2 player_pos = p.position();

	switch ( wrapAroundMode ) {
		case WrapAroundMode::OnScreen:
			use_on_screen_wrap_around(p, player_pos, screenW, screenH, spawnRequests);
			break;
		case WrapAroundMode::OffScreen:
			use_off_screen_wrap_around(p, player_pos, screenW, screenH);
			break;
		default:
			break;
	}
}

void spawn_new_player_at_opposite_border(const SpawnRequest& request, AppLayer* appLayer)
{
	if ( !appLayer )
		return;

	int screenW = GetScreenWidth();
	int screenH = GetScreenHeight();
	Vector2 pos = request.position;

	float halfW = request.width / 2.0f;
	float halfH = request.height / 2.0f;

	const float margin = 0.01f;

	switch ( request.axis ) {
		case LastCollisionAxis::Horizontal: {
			if ( request.position.x - halfW <= 0.0f ) {
				pos.x = static_cast<float>(screenW) + halfW - margin;
			} else {
				pos.x = -halfW + margin;
			}
			break;
		}

		case LastCollisionAxis::Vertical: {
			if ( request.position.y - halfH <= 0.0f ) {
				pos.y = static_cast<float>(screenH) + halfH - margin;
			} else {
				pos.y = -halfH + margin;
			}
			break;
		}
		default:
			break;
	}

	auto newPlayer = appLayer->spawn_player(pos);

		newPlayer->set_border_collision_active_horizontal(true);
		newPlayer->set_border_collision_active_vertical(true);
		newPlayer->set_velocity(request.velocity);
		newPlayer->set_rotation(request.rotation);
		printf("Spawned new sprite at (%.2f, %.2f)\n", pos.x, pos.y);
	
}

void set_player_position_to_opposite_border(Player& p, LastCollisionAxis axis)
{
	int screenW = GetScreenWidth();
	int screenH = GetScreenHeight();
	Vector2 pos = p.position();

	float halfW = p.get_dest_width() / 2.0f;
	float halfH = p.get_dest_height() / 2.0f;

	switch ( axis ) {
		case LastCollisionAxis::Horizontal: {
			if ( pos.x - halfW <= 0.0f ) {
				pos.x = static_cast<float>(screenW) + halfW;
			} else {
				pos.x = -halfW;
			}
			break;
		}
		case LastCollisionAxis::Vertical: {
			if ( pos.y - halfH <= 0.0f ) {
				pos.y = static_cast<float>(screenH) + halfH;
			} else {
				pos.y = -halfH;
			}
			break;
		}
		default:
			break;
	}
	p.set_position(pos);
}

bool is_off_screen(Player& p)
{
	int screenW = GetScreenWidth();
	int screenH = GetScreenHeight();
	Vector2 pos = p.position();

	float halfW = p.get_dest_width() / 2.0f;
	float halfH = p.get_dest_height() / 2.0f;

	if ( halfW <= 0.0f || halfH <= 0.0f ) {
		return false;
	}

	bool completelyOutside =
			(pos.x + halfW < -10.0f) || (pos.x - halfW > screenW + 10.0f) || (pos.y + halfH < -10.0f) || (pos.y - halfH > screenH + 10.0f);

	return completelyOutside;
}

bool check_collision_horizontal(Player& p, Vector2 player_pos, int screenW)
{
	float halfW = p.get_dest_width() / 2.0f;

	bool fully_inside_horizontal = (player_pos.x - halfW > 0.0f) && (player_pos.x + halfW < screenW);
	if ( fully_inside_horizontal ) {
		p.set_border_collision_active_horizontal(false);
	}

	bool collide_left  = (player_pos.x - halfW <= 0.0f);
	bool collide_right = (player_pos.x + halfW >= screenW);

	bool horizontal_collision = collide_left || collide_right;

	if ( horizontal_collision && !p.get_border_collision_active_horizontal() ) {
		return true;
	} else {
		return false;
	}
}

bool check_collision_vertical(Player& p, Vector2 player_pos, int screenH)
{
	float halfH = p.get_dest_height() / 2.0f;

	bool fully_inside_vertical = (player_pos.y - halfH > 0.0f) && (player_pos.y + halfH < screenH);
	if ( fully_inside_vertical ) {
		p.set_border_collision_active_vertical(false);
	}

	bool collide_top	= (player_pos.y - halfH <= 0.0f);
	bool collide_bottom = (player_pos.y + halfH >= screenH);

	bool vertical_collision = collide_top || collide_bottom;

	if ( vertical_collision && !p.get_border_collision_active_vertical() ) {
		return true;
	} else {
		return false;
	}
}

void request_spawn(Player& p, std::vector<SpawnRequest>& spawnRequests, LastCollisionAxis axis)
{
	printf("Test %f %f\n", p.get_dest_height(), p.get_dest_width());

	switch ( axis ) {
		case LastCollisionAxis::Horizontal:
			p.set_border_collision_active_horizontal(true);
			break;
		case LastCollisionAxis::Vertical:
			p.set_border_collision_active_vertical(true);
			break;
		default:
			break;
	}

	SpawnRequest request;
	request.position  = p.position();
	request.velocity  = p.velocity();
	request.rotation  = p.rotation();
	request.height	  = p.get_dest_height();
	request.width	  = p.get_dest_width();
	request.axis	  = axis;
	spawnRequests.push_back(request);
}

void use_on_screen_wrap_around(Player& p, Vector2 player_pos, int screenW, int screenH, std::vector<SpawnRequest>& spawnRequests)
{
	if ( check_collision_horizontal(p, player_pos, screenW) ) {
		request_spawn(p, spawnRequests, LastCollisionAxis::Horizontal);
	}

	if ( check_collision_vertical(p, player_pos, screenH) ) {
		request_spawn(p, spawnRequests, LastCollisionAxis::Vertical);
	}
}

void use_off_screen_wrap_around(Player& p, Vector2 player_pos, int screenW, int screenH)
{
	if ( check_collision_horizontal(p, player_pos, screenW) && is_off_screen(p) ) {
		set_player_position_to_opposite_border(p, LastCollisionAxis::Horizontal);
	}

	if ( check_collision_vertical(p, player_pos, screenH) && is_off_screen(p) ) {
		set_player_position_to_opposite_border(p, LastCollisionAxis::Vertical);
	}
}

void toggle_wrap_around_mode()
{
	switch ( wrapAroundMode ) {
		case WrapAroundMode::OnScreen:
			wrapAroundMode = WrapAroundMode::OffScreen;
			break;
		case WrapAroundMode::OffScreen:
			wrapAroundMode = WrapAroundMode::OnScreen;
			break;
		default:
			break;
	}
}
