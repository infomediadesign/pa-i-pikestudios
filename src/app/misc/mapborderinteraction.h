#pragma once

#include <memory>
#include <vector>
#include "raylib.h"

class Player;
class AppLayer;

enum class LastCollisionAxis { None, Horizontal, Vertical };

struct SpawnRequest
{
	Vector2 position;
	Vector2 velocity;
	float rotation;
	float height;
	float width;
	LastCollisionAxis axis;
};

enum WrapAroundMode { OnScreen, OffScreen };

void map_border_wrap_around(Player& player);
void spawn_new_player_at_opposite_border(const SpawnRequest& request, AppLayer* appLayer);
void set_player_position_to_opposite_border(Player& p, LastCollisionAxis axis);
bool is_off_screen(Player& p);
void detect_map_border_collision(Player& p, std::vector<SpawnRequest>& spawnRequests);
bool check_collision_horizontal(Player& p, Vector2 player_pos, int screenW);
bool check_collision_vertical(Player& p, Vector2 player_pos, int screenH);
void request_spawn(Player& p, std::vector<SpawnRequest>& spawnRequests, LastCollisionAxis axis);
void use_on_screen_wrap_around(Player& p, Vector2 player_pos, int screenW, int screenH, std::vector<SpawnRequest>& spawnRequests);
void use_off_screen_wrap_around(Player& p, Vector2 player_pos, int screenW, int screenH);
void toggle_wrap_around_mode();
void process_offscreen_entities();
