#include <algorithm>
#include "mapborderinteraction.h"
#include <vector>
#include "entities/player.h"
#include "raylib.h"
#include <pscore/application.h>
#include "layers/applayer.h"

namespace {
	WrapAroundMode wrapAroundMode = OnScreen;
	std::vector<SpawnRequest> spawns;
}

void map_border_wrap_around(Player& player)
{
	detect_map_border_collision(player, spawns);

	if ( wrapAroundMode == OnScreen && !spawns.empty() ) {
		auto appLayer = gApp()->get_layer<AppLayer>();
		for ( const auto& request : spawns ) {
			spawn_new_player_at_opposite_border(request, appLayer);
		}
		spawns.clear();
	}
}

void detect_map_border_collision(Player& p, std::vector<SpawnRequest>& spawnRequests)
{
	int screenW = GetScreenWidth();
	int screenH = GetScreenHeight();
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

	float halfW = request.width / 2.0f;
	float halfH = request.height / 2.0f;

	Vector2 pos = calculate_opposite_position(request.position, halfW, halfH, request.axis);

	auto newPlayer = appLayer->spawn_player(pos);
	newPlayer->set_border_collision_active_horizontal(true);
	newPlayer->set_border_collision_active_vertical(true);
	newPlayer->set_is_clone(true);
	newPlayer->set_velocity(request.velocity);
	newPlayer->set_rotation(request.rotation);
}

void set_player_position_to_opposite_border(Player& p, LastCollisionAxis axis)
{
	float halfW = p.get_dest_width() / 2.0f;
	float halfH = p.get_dest_height() / 2.0f;

	Vector2 newPos = calculate_opposite_position(p.position(), halfW, halfH, axis);
	p.set_position(newPos);
}

bool is_off_screen(Player& p)
{
	int screenW = GetScreenWidth();
	int screenH = GetScreenHeight();
	Vector2 pos = p.position();

	float halfW = p.get_dest_width() / 2.0f;
	float halfH = p.get_dest_height() / 2.0f;

	if ( halfW <= 0.0f || halfH <= 0.0f )
		return false;

	constexpr float margin = 10.0f;
	return (pos.x + halfW < -margin) || 
	       (pos.x - halfW > screenW + margin) || 
	       (pos.y + halfH < -margin) || 
	       (pos.y - halfH > screenH + margin);
}

bool check_collision_horizontal(Player& p, Vector2 player_pos, int screenW)
{
	float halfW = p.get_dest_width() / 2.0f;

	bool fully_inside = (player_pos.x - halfW > 0.0f) && (player_pos.x + halfW < screenW);
	if ( fully_inside )
		p.set_border_collision_active_horizontal(false);

	bool collides = (player_pos.x - halfW <= 0.0f) || (player_pos.x + halfW >= screenW);
	return collides && !p.get_border_collision_active_horizontal();
}

bool check_collision_vertical(Player& p, Vector2 player_pos, int screenH)
{
	float halfH = p.get_dest_height() / 2.0f;

	bool fully_inside = (player_pos.y - halfH > 0.0f) && (player_pos.y + halfH < screenH);
	if ( fully_inside )
		p.set_border_collision_active_vertical(false);

	bool collides = (player_pos.y - halfH <= 0.0f) || (player_pos.y + halfH >= screenH);
	return collides && !p.get_border_collision_active_vertical();
}

void request_spawn(Player& p, std::vector<SpawnRequest>& spawnRequests, LastCollisionAxis axis)
{
	if ( axis == LastCollisionAxis::Horizontal )
		p.set_border_collision_active_horizontal(true);
	else if ( axis == LastCollisionAxis::Vertical )
		p.set_border_collision_active_vertical(true);

	spawnRequests.push_back({
		.position = p.position(),
		.velocity = p.velocity(),
		.rotation = p.rotation(),
		.height   = p.get_dest_height(),
		.width    = p.get_dest_width(),
		.axis     = axis
	});
}

void use_on_screen_wrap_around(Player& p, Vector2 player_pos, int screenW, int screenH, std::vector<SpawnRequest>& spawnRequests)
{
	if ( check_collision_horizontal(p, player_pos, screenW) )
		request_spawn(p, spawnRequests, LastCollisionAxis::Horizontal);

	if ( check_collision_vertical(p, player_pos, screenH) )
		request_spawn(p, spawnRequests, LastCollisionAxis::Vertical);
}

Vector2 calculate_opposite_position(Vector2 pos, float halfW, float halfH, LastCollisionAxis axis)
{
	int screenW = GetScreenWidth();
	int screenH = GetScreenHeight();

	switch ( axis ) {
		case LastCollisionAxis::Horizontal:
			pos.x = (pos.x - halfW <= 0.0f) ? static_cast<float>(screenW) + halfW : -halfW;
			break;
		case LastCollisionAxis::Vertical:
			pos.y = (pos.y - halfH <= 0.0f) ? static_cast<float>(screenH) + halfH : -halfH;
			break;
		default:
			break;
	}
	return pos;
}

void use_off_screen_wrap_around(Player& p, Vector2 player_pos, int screenW, int screenH)
{
	if ( check_collision_horizontal(p, player_pos, screenW) && is_off_screen(p) )
		set_player_position_to_opposite_border(p, LastCollisionAxis::Horizontal);

	if ( check_collision_vertical(p, player_pos, screenH) && is_off_screen(p) )
		set_player_position_to_opposite_border(p, LastCollisionAxis::Vertical);
}

void process_offscreen_entities()
{
	if ( wrapAroundMode != OnScreen )
		return;

	auto appLayer = gApp()->get_layer<AppLayer>();
	if ( !appLayer )
		return;

	for ( auto& entity : gApp()->entities() ) {
		if ( auto player = std::dynamic_pointer_cast<Player>(entity.lock()) ) {
			if ( is_off_screen(*player) )
				appLayer->destroy_player(player);
		}
	}
}

void toggle_wrap_around_mode()
{
	wrapAroundMode = (wrapAroundMode == OnScreen) ? OffScreen : OnScreen;
}
