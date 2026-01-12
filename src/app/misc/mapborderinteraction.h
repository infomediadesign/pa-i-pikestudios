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

void map_border_wrap_around(Player& player); // Main Function to be called in the AppLayer Update Loop
void spawn_new_player_at_opposite_border(const SpawnRequest& request, AppLayer* appLayer); // If WrapAroundMode OnScreen: Spawns a new Player at the opposite Border based on the SpawnRequest
void set_player_position_to_opposite_border(Player& p, LastCollisionAxis axis);//If WrapAroundMode OffScreen: Sets the Player Position to the opposite Border based on the LastCollisionAxis
bool is_off_screen(Player& p); // Checks if the Player is completely off the Screen
void detect_map_border_collision(Player& p, std::vector<SpawnRequest>& spawnRequests); // Detects if the Player collides with the Map Border and handles the Wrap Around based on the current WrapAroundMode
bool check_collision_horizontal(Player& p, Vector2 player_pos, int screenW); // Checks if the Player collides with the left or right Border
bool check_collision_vertical(Player& p, Vector2 player_pos, int screenH); // Checks if the Player collides with the top or bottom Border
void request_spawn(Player& p, std::vector<SpawnRequest>& spawnRequests, LastCollisionAxis axis); // Creates a SpawnRequest and adds it to the spawnRequests Vector
Vector2 calculate_opposite_position(Vector2 pos, float halfW, float halfH, LastCollisionAxis axis); // Calculates the opposite Position based on the given Axis
void use_on_screen_wrap_around(Player& p, Vector2 player_pos, int screenW, int screenH, std::vector<SpawnRequest>& spawnRequests); // Handles the On-Screen Wrap Around Logic
void use_off_screen_wrap_around(Player& p, Vector2 player_pos, int screenW, int screenH); // Handles the Off-Screen Wrap Around Logic
void process_offscreen_entities(); // Processes entities that are off-screen and destroys them if WrapAroundMode is OffScreen
void toggle_wrap_around_mode(); // Toggles between On-Screen and Off-Screen Wrap Around Modes
